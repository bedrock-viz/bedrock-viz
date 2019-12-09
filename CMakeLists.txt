cmake_minimum_required(VERSION 3.10)
project(bedrock-viz VERSION 0.1.1)

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
  add_compile_definitions(BEDROCK_VIZ_DEBUG)
endif(CMAKE_BUILD_TYPE STREQUAL Debug)

configure_file(src/config.h.in config.h)

add_executable(bedrock-viz
  "src/asset.cc"
  "src/minecraft.cc"
  "src/args.cc"
  "src/main.cc"
  "src/logger.cc"
  "src/xml.cc" 
  "src/nbt.cc" 
  "src/util.cc")

if(MSVC)
  target_sources(bedrock-viz PRIVATE "src/getopt/getopt.c")
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

find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)
find_package(LibXml2 REQUIRED)

include_directories(
  ${PNG_INCLUDE_DIR} 
  ${ZLIB_INCLUDE_DIR} 
  ${LIBXML2_INCLUDE_DIR} 
  ${PROJECT_SOURCE_DIR}/third_party/leveldb/include
  ${PROJECT_SOURCE_DIR}/third_party/libnbtplusplus/include
  ${PROJECT_SOURCE_DIR}/third_party/spdlog/include
)

target_include_directories(bedrock-viz PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # GCC requires -lstdc++fs to use <filesystem>
  target_link_libraries(bedrock-viz stdc++fs)
endif()

target_link_libraries(bedrock-viz 
  leveldb spdlog nbt++
  ${PNG_LIBRARIES} 
  ${ZLIB_LIBRARIES} 
  ${CMAKE_THREAD_LIBS_INIT}
  ${LIBXML2_LIBRARIES})

file(COPY data
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(COPY static
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS bedrock-viz DESTINATION bin)
install(DIRECTORY data/ DESTINATION share/bedrock-viz/data)
install(DIRECTORY static/ DESTINATION share/bedrock-viz/static)
