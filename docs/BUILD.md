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

## Windows