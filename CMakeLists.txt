cmake_minimum_required(VERSION 3.15)
project(bedrock-viz VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
add_executable(bedrock-viz 
  "src/main.cc" 
  "src/xml.cc" 
  "src/nbt.cc" 
  "src/util.cc")

set(LEVELDB_BUILD_TESTS OFF CACHE INTERNAL "Don't build LevelDB tests")
set(LEVELDB_BUILD_BENCHMARKS OFF CACHE INTERNAL "Don't build LevelDB benckmarks")
set(LEVELDB_INSTALL OFF CACHE INTERNAL "Don't install LevelDB headers")

add_subdirectory(leveldb)
set(NBT_BUILD_TESTS OFF CACHE INTERNAL "Don't build nbt++ tests")

add_subdirectory(libnbtplusplus)

find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)
find_package(LibXml2 REQUIRED)

include_directories(
  ${PNG_INCLUDE_DIR} 
  ${ZLIB_INCLUDE_DIR} 
  ${LIBXML2_INCLUDE_DIR} 
  ${PROJECT_SOURCE_DIR}/leveldb/include 
  ${PROJECT_SOURCE_DIR}/libnbtplusplus/include
)

target_link_libraries(bedrock-viz leveldb nbt++
  ${PNG_LIBRARIES} ${ZLIB_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${LIBXML2_LIBRARIES})
