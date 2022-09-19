# Compiling from source

## macOS

### Requirements

* Homebrew

* libpng: `brew install libpng`

* cmake: `brew install cmake`

* boost: `brew install boost`

Installing `zlib` through homebrew is not necessary because macOS already provides these libraries.

### How to compile

```sh
git clone --recursive https://github.com/bedrock-viz/bedrock-viz.git
# patch
cd bedrock-viz
git apply -p0 patches/leveldb-1.22.patch
git apply -p0 patches/pugixml-disable-install.patch
# make
mkdir build && cd build
cmake ..
make
```

If all goes well, there will be a "bedrock-viz" in `build/`

## Ubuntu (tested & supported on Ubuntu 20.04; 21.04)

### Requirements

* cmake: `sudo apt install cmake`

* libpng and zlib: `sudo apt install libpng++-dev zlib1g-dev`

* boost `sudo apt install libboost-program-options-dev`

* `sudo apt install -y build-essential`

* we use C++ 17 filesystem library, so if you are on an old distro like Ubuntu 18.04, you will need to add g++-8: `sudo apt install g++-8`

### How to compile

```sh
git clone --recursive https://github.com/bedrock-viz/bedrock-viz.git
# patch
cd bedrock-viz
git apply -p0 patches/leveldb-1.22.patch
git apply -p0 patches/pugixml-disable-install.patch
# make
mkdir build && cd build
cmake ..
make
```

If all goes well, there will, eventually, be a "bedrock-viz" in `build/`. If you have lots of cores and a good amount of memory, that `make` command will happily consume all of them with a `-j` command line option, or `-j n` to use _n_ of them. This is especially useful on the first build when it has to chew through a large amount of 3rd party libraries. 

### Compiling on Ubuntu 18.04 and similar vintage distros
If you are on an older distro, you may need to step up to G++ version 8 with these instructions anywhere before the call to `cmake`. You can check your g++ version with this:
```shell
g++ --version
```
If the version output is less than 8, you'll need to install version 8 as listed above and add this to your build environment:
```shell
# switch from gcc-7 to gcc-8 if you are on an older Ubuntu
export CC=/usr/bin/gcc-8
export CXX=/usr/bin/g++-8
```

## Other Linux distros
Use the above directions as a roadmap and adapt the package manager commands. If you find new dependencies please let us know so this document can be updated. Patches welcome.

## Windows

### Requirements

* Visual Studio 2019: https://visualstudio.microsoft.com/vs/ - Community edition is free

* vcpkg: https://github.com/Microsoft/vcpkg

* cmake: https://cmake.org/download/

* libpng: `vcpkg install libpng:x64-windows`

* zlib: `vcpkg install zlib:x64-windows`

* boost-program-options: `vcpkg install boost-program-options:x64-windows`

### How to compile

```sh
# turn off autocrlf
git config --global core.autocrlf false
git config --global core.eol lf

git clone --recursive https://github.com/bedrock-viz/bedrock-viz.git
cd bedrock-viz

# patch
git apply -p0 patches/leveldb-1.22.patch
git apply -p0 patches/pugixml-disable-install.patch
# make
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=<path-to-vcpkg>/scripts/buildsystems/vcpkg.cmake -G "Visual Studio 16 2019" -A x64
# in visual studio command line tools
msbuild bedrock-viz.sln
```
