cmake_minimum_required(VERSION 3.10)
project(bedrock-viz VERSION 0.1.2)

set(CMAKE_CXX_STANDARD 17)

option(BEDROCK_VIZ_BUILD_TESTS "Build unit tests" OFF)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
  add_compile_definitions(BEDROCK_VIZ_DEBUG)
endif(CMAKE_BUILD_TYPE STREQUAL Debug)

configure_file(src/config.h.in config.h)

file(GLOB_RECURSE SRC_FILES src/*.cc)
file(GLOB_RECURSE HEADER_FILES src/*.h)

list(FILTER SRC_FILES EXCLUDE REGEX ".*_test.cc$")

add_executable(bedrock-viz
  ${SRC_FILES}

  ${HEADER_FILES}
)

if(MSVC)
  # TODO remove all insecure c functions
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif(MSVC)

set(LEVELDB_BUILD_TESTS OFF CACHE INTERNAL "Don't build LevelDB tests")
set(LEVELDB_BUILD_BENCHMARKS OFF CACHE INTERNAL "Don't build LevelDB benckmarks")
set(LEVELDB_INSTALL OFF CACHE INTERNAL "Don't install LevelDB headers")

add_subdirectory(third_party/leveldb)
set(NBT_BUILD_TESTS OFF CACHE INTERNAL "Don't build nbt++ tests")

add_subdirectory(third_party/libnbtplusplus)
add_subdirectory(third_party/spdlog)
add_subdirectory(third_party/pugixml)

find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost 1.67 COMPONENTS program_options REQUIRED)


include_directories(
  ${PNG_INCLUDE_DIR} 
  ${ZLIB_INCLUDE_DIR} 
  ${Boost_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/third_party/leveldb/include
  ${PROJECT_SOURCE_DIR}/third_party/libnbtplusplus/include
  ${PROJECT_SOURCE_DIR}/third_party/spdlog/include
  ${PROJECT_SOURCE_DIR}/third_party/pugixml/include
)

target_include_directories(bedrock-viz PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # GCC requires -lstdc++fs to use <filesystem>
  target_link_libraries(bedrock-viz stdc++fs)
endif()

target_link_libraries(bedrock-viz 
  leveldb spdlog nbt++ pugixml::static
  ${PNG_LIBRARIES} 
  ${ZLIB_LIBRARIES} 
  ${Boost_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT})

file(COPY data
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(COPY static
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS bedrock-viz DESTINATION bin)
install(DIRECTORY data/ DESTINATION share/bedrock-viz/data)
install(DIRECTORY static/ DESTINATION share/bedrock-viz/static)

# From LevelDB's CMakeLists.txt
if(BEDROCK_VIZ_BUILD_TESTS)
  enable_testing()
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  set(BUILD_GMOCK OFF CACHE INTERNAL "Don't bulid gmock")

  # This project is tested using GoogleTest.
  add_subdirectory("third_party/googletest")

  file(GLOB_RECURSE SRC_FILES_NO_MAIN src/*.cc)
  list(FILTER SRC_FILES_NO_MAIN EXCLUDE REGEX ".*_test.cc$")
  list(FILTER SRC_FILES_NO_MAIN EXCLUDE REGEX ".*/main.cc$")

  function(viz_test test_file)
    get_filename_component(test_target_name "${test_file}" NAME_WE)
    add_executable("${test_target_name}" 
      ${SRC_FILES_NO_MAIN}
      ${HEADER_FILES}
      "${test_file}"
    )

    if(MSVC)
      # TODO remove all insecure c functions
      add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    endif(MSVC)

    target_link_libraries("${test_target_name}" 
      gtest 
      leveldb spdlog nbt++ pugixml::static
      ${PNG_LIBRARIES} 
      ${ZLIB_LIBRARIES} 
      ${CMAKE_THREAD_LIBS_INIT}
    )

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      # GCC requires -lstdc++fs to use <filesystem>
      target_link_libraries("${test_target_name}" stdc++fs)
    endif()

    target_include_directories("${test_target_name}" PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

    add_test(NAME "${test_target_name}" COMMAND "${test_target_name}")
  endfunction(viz_test)

  viz_test("src/example_test.cc")

endif(BEDROCK_VIZ_BUILD_TESTS)
