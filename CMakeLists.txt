cmake_minimum_required(VERSION 3.10)
project(bedrock-viz VERSION 0.1.2)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_TESTS "Build unit tests" OFF)
option(CODE_COVERAGE "Enable coverage reporting" OFF)

set(BIN_NAME bedrock-viz)
set(LIB_NAME libbedrock-viz)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
  add_compile_definitions(BEDROCK_VIZ_DEBUG)
endif(CMAKE_BUILD_TYPE STREQUAL Debug)

add_library(coverage_config INTERFACE)

if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  message("Code Coverage Enabled")
  target_compile_options(coverage_config INTERFACE -O0 -g --coverage)
  target_link_libraries(coverage_config INTERFACE --coverage)
endif()

configure_file(include/config.h.in config.h)

file(GLOB_RECURSE SRC_FILES src/*.cc)
list(FILTER SRC_FILES EXCLUDE REGEX ".*/main.cc$")
file(GLOB_RECURSE HEADER_FILES include/*.h)
file(GLOB_RECURSE TEST_FILES tests/*.cc)

add_library(${LIB_NAME} STATIC ${SRC_FILES})

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
find_package(Boost COMPONENTS program_options REQUIRED)

include_directories(
  ${PNG_INCLUDE_DIR} 
  ${ZLIB_INCLUDE_DIR} 
  ${Boost_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/third_party/leveldb/include
  ${PROJECT_SOURCE_DIR}/third_party/libnbtplusplus/include
  ${PROJECT_SOURCE_DIR}/third_party/spdlog/include
  ${PROJECT_SOURCE_DIR}/third_party/pugixml/include
)

target_include_directories(${LIB_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${LIB_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # GCC requires -lstdc++fs to use <filesystem>
  target_link_libraries(${LIB_NAME} stdc++fs)
endif()

target_link_libraries(${LIB_NAME} 
  coverage_config
  leveldb spdlog nbt++ pugixml::static
  ${PNG_LIBRARIES} 
  ${ZLIB_LIBRARIES} 
  ${Boost_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT})

file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY static DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(${BIN_NAME} src/main.cc)
target_link_libraries(${BIN_NAME} PRIVATE ${LIB_NAME})
target_include_directories(${BIN_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

add_executable(legend utils/legend.cc)
target_link_libraries(legend PRIVATE ${LIB_NAME})
target_include_directories(legend PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS ${BIN_NAME} DESTINATION bin)
install(DIRECTORY data/ DESTINATION share/bedrock-viz/data)
install(DIRECTORY static/ DESTINATION share/bedrock-viz/static)

# From LevelDB's CMakeLists.txt
if(BUILD_TESTS)
  enable_testing()
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  set(BUILD_GMOCK OFF CACHE INTERNAL "Don't bulid gmock")

  # This project is tested using GoogleTest.
  add_subdirectory("third_party/googletest")

  set(test_target_name "bedrock-viz-test")
  add_executable("${test_target_name}" 
      "${TEST_FILES}"
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
    ${LIB_NAME}
  )
  
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # GCC requires -lstdc++fs to use <filesystem>
    target_link_libraries("${test_target_name}" stdc++fs)
  endif()

  target_include_directories(${test_target_name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
  add_test(NAME "UnitTests" COMMAND "${test_target_name}")
  add_test(NAME "IntegrationTests" COMMAND "${test_target_name}")
  add_test(NAME "MiscTests" COMMAND "${test_target_name}")
endif(BUILD_TESTS)
