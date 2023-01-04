FROM ubuntu:20.04 as builder

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y cmake g++ git libboost-program-options-dev libpng++-dev zlib1g-dev

COPY . /bedrock-viz

RUN cd /bedrock-viz && \
    patch -p0 < patches/leveldb-1.22.patch && \
    mkdir -p build && cd build && \
    cmake .. && \
    make && \
    make install

FROM ubuntu:20.04

RUN apt-get update && \
    apt-get install -y libpng16-16 libboost-program-options-dev && \
    rm -rf /var/cache/apt

COPY --from=builder /usr/local/share/bedrock-viz /usr/local/share/bedrock-viz
COPY --from=builder /usr/local/bin/bedrock-viz /usr/local/bin/

ENTRYPOINT ["/usr/local/bin/bedrock-viz"]
