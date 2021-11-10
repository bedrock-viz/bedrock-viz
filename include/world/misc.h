#pragma once

#include <cstdint>
#include <cstddef>

namespace mcpe_viz {
    uint8_t getBlockId_LevelDB_v2(const char* p, int32_t x, int32_t z, int32_t y);
    uint8_t getBlockId_LevelDB_v3(const char* p, int32_t x, int32_t z, int32_t y);
    uint8_t getBlockData_LevelDB_v2(const char* p, int32_t x, int32_t z, int32_t y);
    uint8_t getBlockData_LevelDB_v3(const char* p, size_t plen, int32_t x, int32_t z, int32_t y);
    int32_t convertChunkV7toV3(const char* cdata, size_t cdata_size, int16_t* emuchunk);
    uint8_t getBlockData_LevelDB_v3__fake_v7(const int16_t* p, size_t plen, int32_t x, int32_t z, int32_t y);
    uint8_t getBlockSkyLight_LevelDB_v2(const char* p, int32_t x, int32_t z, int32_t y);
    uint8_t getBlockBlockLight_LevelDB_v2(const char* p, int32_t x, int32_t z, int32_t y);
    uint8_t getColData_Height_LevelDB_v2(const char* buf, int32_t x, int32_t z);
    uint32_t getColData_GrassAndBiome_LevelDB_v2(const char* buf, int32_t x, int32_t z);
    uint8_t getBlockSkyLight_LevelDB_v3(const char* p, size_t plen, int32_t x, int32_t z, int32_t y);
    uint8_t getBlockBlockLight_LevelDB_v3(const char* p, size_t plen, int32_t x, int32_t z, int32_t y);
    int32_t
        setupBlockVars_v7(const char* cdata, int32_t& blocksPerWord, int32_t& bitsPerBlock, int32_t& blockOffset,
            int32_t& paletteOffset);
//    uint8_t
//        getBlockId_LevelDB_v7(const char* p, int blocksPerWord, int bitsPerBlock, int32_t x, int32_t z, int32_t y);
    uint8_t getColData_Height_LevelDB_v3(const char* buf, int32_t x, int32_t z);
    uint32_t getColData_GrassAndBiome_LevelDB_v3(const char* buf, int32_t buflen, int32_t x, int32_t z);
    int32_t _calcOffsetBlock_LevelDB_v3_fullchunk(int32_t x, int32_t z, int32_t y);
    uint8_t getData_LevelDB_v3_fullchunk(const char* p, int32_t x, int32_t z, int32_t y);

    inline uint8_t _getBitFromByte(const char* cdata, int32_t bitnum) {
            int byteStart = bitnum / 8;
            int byteOffset = bitnum % 8;
            return cdata[byteStart] & (1 << byteOffset);
    }

    inline int32_t getBitsFromBytes_8(const char* cdata, int32_t bitstart, int32_t bitlen)
    {
            unsigned byteStart = bitstart / 8;
            unsigned byteOffset = bitstart % 8;

            uint8_t byte_low = cdata[byteStart];
            uint8_t byte_high = cdata[byteStart + 1];
            uint16_t value = byte_low + (byte_high << 8u);
            value = value >> byteOffset;
            uint16_t mask = (1u << unsigned(bitlen)) - 1;
            return value & mask;
    }

    // todo - this is probably quite slow
    inline int32_t getBitsFromBytes_large(const char* cdata, int32_t bitstart, int32_t bitlen)
    {
        int32_t ret = 0;
        for (int b = 0; b < bitlen; b++) {
            uint8_t bit = _getBitFromByte(cdata, bitstart + b);
            if (bit) {
                ret |= 1 << b;
                // ret |= 1 << (bitlen-1-b);
            }
        }
        return ret;
    }

    inline int32_t getBitsFromBytes(const char* cdata, int32_t bitstart, int32_t bitlen)
    {
        if (bitlen <= 8) {
            return getBitsFromBytes_8(cdata, bitstart, bitlen);
        }
        return getBitsFromBytes_large(cdata, bitstart, bitlen);
    }

    inline uint8_t getBlockId_LevelDB_v7(const char* p, int blocksPerWord, int bitsPerBlock,
                                  int32_t x, int32_t z, int32_t y) {
        if (blocksPerWord == 0) {
            // This feels wrong, but what else can we use when the blocks per word is zero?
            // This prevents a math exception (divide by zero) and does not *seem* to have any
            // negative impact on the generated maps.
            return 0;
        }
        int blockPos = (((x * 16) + z) * 16) + y;
        int wordStart = blockPos / blocksPerWord;
        int bitOffset = (blockPos % blocksPerWord) * bitsPerBlock;
        int bitStart = wordStart * 4 * 8 + bitOffset;
        return getBitsFromBytes(p, bitStart, bitsPerBlock);
    }
}
