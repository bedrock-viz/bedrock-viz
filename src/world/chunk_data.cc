#include "chunk_data.h"

#include "../define.h"
#include "../global.h"
#include "../nbt.h"
#include "misc.h"
#include "point_conversion.h"
#include "common.h"
#include "../utils/unknown_recorder.h"
#include "../minecraft/v2/block.h"

namespace mcpe_viz {
    int32_t ChunkData_LevelDB::_do_chunk_v2(int32_t tchunkX, int32_t tchunkZ, const char* cdata,
        int32_t dimensionId, const std::string& dimName,
        const bool* fastBlockHideList, const bool* fastBlockForceTopList,
        const bool* fastBlockToGeoJSON)
    {
        chunkX = tchunkX;
        chunkZ = tchunkZ;
        chunkFormatVersion = 2;

        // iterate over chunk space
        uint8_t blockId;
        for (int32_t cy = MAX_BLOCK_HEIGHT_127; cy >= 0; cy--) {
            for (int32_t cx = 0; cx < 16; cx++) {
                for (int32_t cz = 0; cz < 16; cz++) {
                    blockId = getBlockId_LevelDB_v2(cdata, cx, cz, cy);

                    // todobig - handle block variant?
                    if (fastBlockToGeoJSON[blockId]) {
                        double ix, iy;
                        char tmpstring[512];
                        worldPointToGeoJSONPoint(dimensionId, chunkX * 16 + cx, chunkZ * 16 + cz, ix, iy);
                        sprintf(tmpstring, ""
                            "\"Name\": \"%s\", "
                            "\"Block\": true, "
                            "\"Dimension\": \"%d\", "
                            "\"Pos\": [%d, %d, %d]"
                            "} }", Block::queryName(blockId).c_str(), dimensionId,
                            chunkX * 16 + cx, cy, chunkZ * 16 + cz
                        );
                        std::string json = ""
                            + makeGeojsonHeader(ix, iy)
                            + tmpstring;
                        listGeoJSON.push_back(json);
                    }


                    // todo - check for isSolid?

                    if (blockId != 0) {  // current block is NOT air
                        if ((blocks[cx][cz] == 0 &&  // top block is not already set
                            !fastBlockHideList[blockId]) ||
                            fastBlockForceTopList[blockId]) {

                            blocks[cx][cz] = blockId;
                            data[cx][cz] = getBlockData_LevelDB_v2(cdata, cx, cz, cy);
                            topBlockY[cx][cz] = cy;

#if 1
                            // todo - we are getting the block light ABOVE this block (correct?)
                            // todo - this will break if we are using force-top stuff
                            int32_t cy2 = cy;
                            auto block = Block::get(blockId);
                            if (block != nullptr && block->solid) {
                            // if (blockInfoList[blockId].isSolid()) {
                                // move to block above this block
                                cy2++;
                                if (cy2 > MAX_BLOCK_HEIGHT_127) { cy2 = MAX_BLOCK_HEIGHT_127; }
                            }
                            else {
                                // if not solid, don't adjust
                            }
                            uint8_t sl = getBlockSkyLight_LevelDB_v2(cdata, cx, cz, cy2);
                            uint8_t bl = getBlockBlockLight_LevelDB_v2(cdata, cx, cz, cy2);
                            // we combine the light nibbles into a byte
                            topLight[cx][cz] = (sl << 4) | bl;
#endif
                        }
                    }
                }
            }
        }

        // get per-column data
        for (int32_t cx = 0; cx < 16; cx++) {
            for (int32_t cz = 0; cz < 16; cz++) {
                heightCol[cx][cz] = getColData_Height_LevelDB_v2(cdata, cx, cz);
                grassAndBiome[cx][cz] = getColData_GrassAndBiome_LevelDB_v2(cdata, cx, cz);
            }
        }
        return 0;
    }

    int32_t ChunkData_LevelDB::_do_chunk_v3(int32_t tchunkX, int32_t tchunkY, int32_t tchunkZ, const char* cdata, size_t cdata_size,
        int32_t dimensionId, const std::string& dimName,
        const bool* fastBlockHideList, const bool* fastBlockForceTopList,
        const bool* fastBlockToGeoJSON)
    {
        chunkX = tchunkX;
        int32_t chunkY = tchunkY;
        chunkZ = tchunkZ;
        chunkFormatVersion = 3;

        // iterate over chunk space
        uint8_t blockId;
        for (int32_t cy = 0; cy < 16; cy++) {
            for (int32_t cx = 0; cx < 16; cx++) {
                for (int32_t cz = 0; cz < 16; cz++) {
                    blockId = getBlockId_LevelDB_v3(cdata, cx, cz, cy);
                    auto block = Block::get(blockId);
                    if (block == nullptr) {
                        continue;
                    }
                    // todobig - handle block variant?
                    if (fastBlockToGeoJSON[blockId]) {
                        double ix, iy;
                        char tmpstring[512];
                        worldPointToGeoJSONPoint(dimensionId, chunkX * 16 + cx, chunkZ * 16 + cz, ix, iy);
                        sprintf(tmpstring, ""
                            "\"Name\": \"%s\", "
                            "\"Block\": true, "
                            "\"Dimension\": \"%d\", "
                            "\"Pos\": [%d, %d, %d]"
                            "} }", block->name.c_str(), dimensionId,
                            chunkX * 16 + cx, chunkY * 16 + cy, chunkZ * 16 + cz
                        );
                        std::string json = ""
                            + makeGeojsonHeader(ix, iy)
                            + tmpstring;
                        listGeoJSON.push_back(json);
                    }

                    // note: we check spawnable later

                    // todo - check for isSolid?

                    int32_t realy = chunkY * 16 + cy;
                    if (blockId != 0) {  // current block is NOT air
                        // todonow - this will break forcetop!
                        if ((realy >= topBlockY[cx][cz] &&
                            !fastBlockForceTopList[blocks[cx][cz]] &&
                            // blocks[cx][cz] == 0 &&  // top block is not already set
                            !fastBlockHideList[blockId]) ||
                            fastBlockForceTopList[blockId]) {

                            blocks[cx][cz] = blockId;
                            data[cx][cz] = getBlockData_LevelDB_v3(cdata, cdata_size, cx, cz, cy);
                            topBlockY[cx][cz] = realy;

                            int32_t cy2 = cy;

                            // todonow todohere todobig todostopper - can't do this until we have the whole chunk
#if 1
                                // todo - we are getting the block light ABOVE this block (correct?)
                                // todo - this will break if we are using force-top stuff
                            if (block->solid) {
                                // move to block above this block
                                cy2++;
                                if (cy2 > MAX_BLOCK_HEIGHT) { cy2 = MAX_BLOCK_HEIGHT; }
                            }
                            else {
                                // if not solid, don't adjust
                            }
#endif
                            uint8_t sl = getBlockSkyLight_LevelDB_v3(cdata, cdata_size, cx, cz, cy2);
                            uint8_t bl = getBlockBlockLight_LevelDB_v3(cdata, cdata_size, cx, cz, cy2);
                            // we combine the light nibbles into a byte
                            topLight[cx][cz] = (sl << 4) | bl;
                        }
                    }
                }
            }
        }
        return 0;
    }

    int32_t ChunkData_LevelDB::_do_chunk_v7(int32_t tchunkX, int32_t tchunkY, int32_t tchunkZ, const char* cdata, size_t cdata_size,
        int32_t dimensionId, const std::string& dimName,
        const bool* fastBlockHideList, const bool* fastBlockForceTopList,
        const bool* fastBlockToGeoJSON)
    {
        chunkX = tchunkX;
        int32_t chunkY = tchunkY;
        chunkZ = tchunkZ;
        chunkFormatVersion = 7;

        // determine location of chunk palette
        int32_t blocksPerWord = -1;
        int32_t bitsPerBlock = -1;
        bool paddingFlag = false;
        int32_t offsetBlockInfoList = -1;
        int32_t extraOffset = -1;

        //logger.msg(kLogWarning,"hey -- cdata %02x %02x %02x\n", cdata[0], cdata[1], cdata[2]);

        if (setupBlockVars_v7(cdata, blocksPerWord, bitsPerBlock, paddingFlag, offsetBlockInfoList, extraOffset) !=
            0) {
            return -1;
        }

        // read chunk palette and associate old-school block id's
        MyNbtTagList tagList;
        int xoff = offsetBlockInfoList + 6 + extraOffset;

        parseNbtQuiet(&cdata[xoff], int32_t(cdata_size - xoff), cdata[offsetBlockInfoList + 3], tagList);
        //parseNbt("chunk-palette",&cdata[xoff], cdata_size-xoff, tagList);

        std::vector<int32_t> chunkBlockPalette_BlockId(tagList.size());
        std::vector<int32_t> chunkBlockPalette_BlockData(tagList.size());

        for (size_t i = 0; i < tagList.size(); i++) {
            // check tagList
            if (tagList[i].second->get_type() == nbt::tag_type::Compound) {
                nbt::tag_compound tc = tagList[i].second->as<nbt::tag_compound>();

                bool processedFlag = false;
                if (tc.has_key("name", nbt::tag_type::String)) {
                    std::string bname = tc["name"].as<nbt::tag_string>().get();
                    int bdata = 0;
                    if (tc.has_key("val", nbt::tag_type::Short)) {
                        bdata = tc["val"].as<nbt::tag_short>().get();
                    }

                    auto block = Block::getByUname(bname);
                    if (block != nullptr) {
                        chunkBlockPalette_BlockId[i] = block->id;
                        // todonow - correct?
                        chunkBlockPalette_BlockData[i] = bdata;
                    }
                    else {
                        record_unknow_uname(bname);
                        // todonow - reasonable?
                        chunkBlockPalette_BlockId[i] = 0;
                        chunkBlockPalette_BlockData[i] = 0;
                    }
                    processedFlag = true;
                }
                if (!processedFlag) {
                    log::warn("Did not find 'name' tag in a chunk palette! (i={}) (len={})", i, tagList.size());
                    //todozooz - dump tc to screen log
                }
            }
            else {
                log::warn("Unexpected NBT format in _do_chunk_v7");
            }
        }

        //todozooz -- new 16-bit block-id's (instead of 8-bit) are a BIG issue - this needs attention here
        // iterate over chunk space
        uint8_t paletteBlockId, blockData;
        int32_t blockId;
        for (int32_t cy = 0; cy < 16; cy++) {
            for (int32_t cx = 0; cx < 16; cx++) {
                for (int32_t cz = 0; cz < 16; cz++) {
                    paletteBlockId = getBlockId_LevelDB_v7(&cdata[2 + extraOffset],
                        blocksPerWord, bitsPerBlock, cx, cz, cy);

                    // look up blockId
                    // TODO error checking
                    if (paletteBlockId < chunkBlockPalette_BlockId.size()) {
                        blockId = chunkBlockPalette_BlockId[paletteBlockId];
                        blockData = chunkBlockPalette_BlockData[paletteBlockId];
                    }
                    else {
                        blockId = 0;
                        blockData = 0;
                        log::warn("Found chunk palette id out of range {} (size={})",
                            paletteBlockId, chunkBlockPalette_BlockId.size());
                    }
                    auto block = Block::get(blockId);
                    if (block == nullptr) {
                        continue;
                    }

                    // todobig - handle block variant?
                    if (fastBlockToGeoJSON[blockId]) {
                        double ix, iy;
                        char tmpstring[512];
                        worldPointToGeoJSONPoint(dimensionId, chunkX * 16 + cx, chunkZ * 16 + cz, ix, iy);
                        sprintf(tmpstring, ""
                            "\"Name\": \"%s\", "
                            "\"Block\": true, "
                            "\"Dimension\": \"%d\", "
                            "\"Pos\": [%d, %d, %d]"
                            "} }", block->name.c_str(), dimensionId,
                            chunkX * 16 + cx, chunkY * 16 + cy, chunkZ * 16 + cz
                        );
                        std::string json = ""
                            + makeGeojsonHeader(ix, iy)
                            + tmpstring;
                        listGeoJSON.push_back(json);
                    }

                    // note: we check spawnable later

                    // todo - check for isSolid?

                    int32_t realy = chunkY * 16 + cy;
                    if (blockId != 0) {  // current block is NOT air
                        // todonow - this will break forcetop!
                        if ((realy >= topBlockY[cx][cz] &&
                            !fastBlockForceTopList[blocks[cx][cz]] &&
                            // blocks[cx][cz] == 0 &&  // top block is not already set
                            !fastBlockHideList[blockId]) ||
                            fastBlockForceTopList[blockId]) {

                            blocks[cx][cz] = blockId;
                            data[cx][cz] = blockData; // getBlockData_LevelDB_v3(cdata, cdata_size, cx,cz,cy);
                            topBlockY[cx][cz] = realy;

                            int32_t cy2 = cy;

                            // todonow todohere todobig todostopper - can't do this until we have the whole chunk
#if 1
                                // todo - we are getting the block light ABOVE this block (correct?)
                                // todo - this will break if we are using force-top stuff
                            if (block->solid) {
                                // move to block above this block
                                cy2++;
                                if (cy2 > MAX_BLOCK_HEIGHT) { cy2 = MAX_BLOCK_HEIGHT; }
                            }
                            else {
                                // if not solid, don't adjust
                            }
#endif
                            // todonow todohere -- no blocklight or skylight in v7 chunks?!
                            uint8_t sl = 0; // getBlockSkyLight_LevelDB_v3(cdata, cdata_size, cx,cz,cy2);
                            uint8_t bl = 0; // getBlockBlockLight_LevelDB_v3(cdata, cdata_size, cx,cz,cy2);
                            // we combine the light nibbles into a byte
                            topLight[cx][cz] = (sl << 4) | bl;
                        }
                    }
                }
            }
        }
        return 0;
    }
    int32_t ChunkData_LevelDB::_do_chunk_biome_v3(int32_t tchunkX, int32_t tchunkZ, const char* cdata, int32_t cdatalen)
    {
        chunkX = tchunkX;
        chunkZ = tchunkZ;

        // get per-column data
        uint8_t biomeId = 0;
        for (int32_t cx = 0; cx < 16; cx++) {
            for (int32_t cz = 0; cz < 16; cz++) {
                heightCol[cx][cz] = getColData_Height_LevelDB_v3(cdata, cx, cz);
                grassAndBiome[cx][cz] = getColData_GrassAndBiome_LevelDB_v3(cdata, cdatalen, cx, cz);
            }
        }
        return 0;
    }
}
