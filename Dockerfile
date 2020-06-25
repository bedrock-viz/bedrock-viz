FROM ubuntu:18.04 as builder

RUN apt-get update && \
    apt-get install -y cmake libxml2-dev g++-8 libpng-dev git

COPY . /bedrock-viz

RUN cd /bedrock-viz && \
    patch -p0 < patches/leveldb-1.22.patch && \
    mkdir -p build && cd build && \
    export CC=/usr/bin/gcc-8 && \
    export CXX=/usr/bin/g++-8 && \
    cmake .. && \
    make && \
    make install

FROM ubuntu:18.04

RUN apt-get update && \
    apt-get install -y libpng16-16 && \
    rm -rf /var/cache/apt

COPY --from=builder /usr/local/share/bedrock-viz /usr/local/share/bedrock-viz
COPY --from=builder /usr/local/bin/bedrock-viz /usr/local/bin/

ENTRYPOINT ["/usr/local/bin/bedrock-viz"]
