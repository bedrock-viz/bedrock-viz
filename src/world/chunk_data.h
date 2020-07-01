#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <leveldb/db.h>

#include "../util.h"

namespace mcpe_viz {
    // todobig - perhaps this is silly (storing all this info per-chunk)
    class ChunkData_LevelDB {
    public:
        // todobig - move to private?
        int32_t chunkX, chunkZ;
        //todozooz -- new 16-bit block-id's (instead of 8-bit) are a BIG issue - this needs attention here
        uint16_t blocks[16][16];
        uint8_t data[16][16];
        uint32_t grassAndBiome[16][16];
        uint8_t topBlockY[16][16];
        uint8_t heightCol[16][16];
        uint8_t topLight[16][16];
        int32_t chunkFormatVersion;

        // we parse the block (et al) data in a chunk from leveldb
        ChunkData_LevelDB() {
            // clear the data we track
            memset(blocks, 0, sizeof(blocks));

            // todobig - clears are redundant?
            memset(data, 0, sizeof(data));
            //memset(grassAndBiome, 0, 16*16*sizeof(uint32_t));
            memset(topBlockY, 0, sizeof(topBlockY));
            //memset(heightCol,0, 16*16*sizeof(uint8_t));
            memset(topLight, 0, sizeof(topLight));

            chunkFormatVersion = -1;
        }

        int32_t _do_chunk_v2(int32_t tchunkX, int32_t tchunkZ, const char* cdata,
            int32_t dimensionId, const std::string& dimName,
            const bool* fastBlockHideList, const bool* fastBlockForceTopList,
            const bool* fastBlockToGeoJSON);


        int32_t _do_chunk_v3(int32_t tchunkX, int32_t tchunkY, int32_t tchunkZ, const char* cdata, size_t cdata_size,
            int32_t dimensionId, const std::string& dimName,
            const bool* fastBlockHideList, const bool* fastBlockForceTopList,
            const bool* fastBlockToGeoJSON);


        int32_t _do_chunk_v7(int32_t tchunkX, int32_t tchunkY, int32_t tchunkZ, const char* cdata, size_t cdata_size,
            int32_t dimensionId, const std::string& dimName,
            const bool* fastBlockHideList, const bool* fastBlockForceTopList,
            const bool* fastBlockToGeoJSON);


        int32_t _do_chunk_biome_v3(int32_t tchunkX, int32_t tchunkZ, const char* cdata, int32_t cdatalen);
    };

}