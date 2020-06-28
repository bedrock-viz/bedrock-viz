# Compiling from source

## macOS

### Requirements

* Homebrew

* libpng: `brew install libpng`

* cmake: `brew install cmake`

Installing `zlib` and `libxml2` through homebrew is not necessary because macOS already provides these libraries.

### How to compile

```sh
git clone --recursive https://github.com/bedrock-viz/bedrock-viz.git
# patch leveldb
cd bedrock-viz
patch -p0 < patches/leveldb-1.22.patch
# make
mkdir build && cd build
cmake ..
make
```

If all goes well, there will be a "bedrock-viz" in `build/`

## Ubuntu (tested on ubuntu 18.04)

### Requirements

* cmake: `sudo apt install cmake`

* libxml2: `sudo apt install libxml2-dev`

* libpng: `sudo apt install libpng++-dev`

* we use C++ 17 filesystem library, so we need g++-8: `sudo apt install g++-8`

### How to compile

```sh
git clone --recursive https://github.com/bedrock-viz/bedrock-viz.git
# patch leveldb
cd bedrock-viz
patch -p0 < patches/leveldb-1.22.patch
# make
mkdir build && cd build
# switch gcc-7 to gcc-8
export CC=/usr/bin/gcc-8
export CXX=/usr/bin/g++-8
cmake ..
make
```

If all goes well, there will be a "bedrock-viz" in `build/`

## Windows

### Requirements

* Visual Studio 2019: https://visualstudio.microsoft.com/vs/ - Community edition is free

* vcpkg: https://github.com/Microsoft/vcpkg

* cmake: https://cmake.org/download/

* patch for windows: <http://gnuwin32.sourceforge.net/packages/patch.htm>

* libpng: `vcpkg install libpng:x64-windows`

* zlib: `vcpkg install zlib:x64-windows`

* libxml2: `vcpkg install libxml2:x64-windows`

### How to compile

```sh
git clone --recursive https://github.com/bedrock-viz/bedrock-viz.git
cd bedrock-viz

# patch leveldb
# !!! change line ending of patch file to CRLF first
patch -p0 -i patches/leveldb-1.22.patch

# make
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=<path-to-vcpkg>/scripts/buildsystems/vcpkg.cmake -G "Visual Studio 16 2019" -A x64
# in visual studio command line tools
msbuild bedrock-viz.sln
```