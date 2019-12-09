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
        setupBlockVars_v7(const char* cdata, int32_t& blocksPerWord, int32_t& bitsPerBlock, bool& paddingFlag,
            int32_t& offsetBlockInfoList, int32_t& extraOffset);
    uint8_t
        getBlockId_LevelDB_v7(const char* p, int blocksPerWord, int bitsPerBlock, int32_t x, int32_t z, int32_t y);
    uint8_t getColData_Height_LevelDB_v3(const char* buf, int32_t x, int32_t z);
    uint32_t getColData_GrassAndBiome_LevelDB_v3(const char* buf, int32_t buflen, int32_t x, int32_t z);
    int32_t _calcOffsetBlock_LevelDB_v3_fullchunk(int32_t x, int32_t z, int32_t y);
    uint8_t getData_LevelDB_v3_fullchunk(const char* p, int32_t x, int32_t z, int32_t y);
}
