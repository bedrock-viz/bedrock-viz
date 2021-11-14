#include "world/misc.h"

#include "define.h"
#include "nbt.h"
#include "logger.h"
#include "utils/unknown_recorder.h"
#include "minecraft/v2/block.h"


namespace mcpe_viz {

    // todolib - these funcs should be in a class?

    // calculate an offset into mcpe chunk data for block data
    int32_t _calcOffsetBlock_LevelDB_v2(int32_t x, int32_t z, int32_t y) {
        return (((x * 16) + z) * (MAX_BLOCK_HEIGHT_127 + 1)) + y;
    }

    // calculate an offset into mcpe chunk data for column data
    int32_t _calcOffsetColumn_LevelDB_v2(int32_t x, int32_t z) {
        // NOTE! this is the OPPOSITE of block data (oy)
        return (z * 16) + x;
    }

    uint8_t getBlockId_LevelDB_v2(const char* p, int32_t x, int32_t z, int32_t y) {
        return (p[_calcOffsetBlock_LevelDB_v2(x, z, y)] & 0xff);
    }

    uint8_t getBlockData_LevelDB_v2(const char* p, int32_t x, int32_t z, int32_t y) {
        int32_t off = _calcOffsetBlock_LevelDB_v2(x, z, y);
        int32_t off2 = off / 2;
        int32_t mod2 = off % 2;
        int32_t v = p[32768 + off2];
        if (mod2 == 0) {
            return v & 0x0f;
        }
        else {
            return (v & 0xf0) >> 4;
        }
    }

    // a block opacity value? (e.g. glass is 0xf, water is semi (0xc) and an opaque block is 0x0)
    uint8_t getBlockSkyLight_LevelDB_v2(const char* p, int32_t x, int32_t z, int32_t y) {
        int32_t off = _calcOffsetBlock_LevelDB_v2(x, z, y);
        int32_t off2 = off / 2;
        int32_t mod2 = off % 2;
        int32_t v = p[32768 + 16384 + off2];
        if (mod2 == 0) {
            return v & 0x0f;
        }
        else {
            return (v & 0xf0) >> 4;
        }
    }

    // block light is light value from torches et al -- super cool looking as an image, but it looks like block light is probably stored in air blocks which are above top block
    uint8_t getBlockBlockLight_LevelDB_v2(const char* p, int32_t x, int32_t z, int32_t y) {
        int32_t off = _calcOffsetBlock_LevelDB_v2(x, z, y);
        int32_t off2 = off / 2;
        int32_t mod2 = off % 2;
        int32_t v = p[32768 + 16384 + 16384 + off2];
        if (mod2 == 0) {
            return v & 0x0f;
        }
        else {
            return (v & 0xf0) >> 4;
        }
    }

    // height of top *solid* block? (e.g. a glass block will NOT be the top block here)
    uint8_t getColData_Height_LevelDB_v2(const char* buf, int32_t x, int32_t z) {
        int32_t off = _calcOffsetColumn_LevelDB_v2(x, z);
        int8_t v = buf[32768 + 16384 + 16384 + 16384 + off];
        return v;
    }

    // this is 4-bytes: lsb is biome, the high 3-bytes are RGB grass color
    uint32_t getColData_GrassAndBiome_LevelDB_v2(const char* buf, int32_t x, int32_t z) {
        int32_t off = _calcOffsetColumn_LevelDB_v2(x, z) * 4;
        int32_t v;
        memcpy(&v, &buf[32768 + 16384 + 16384 + 16384 + 256 + off], 4);
        return v;
    }


    // calculate an offset into mcpe chunk data for block data
    int32_t _calcOffsetBlock_LevelDB_v3(int32_t x, int32_t z, int32_t y) {
        return (((x * 16) + z) * 16) + y;
    }

    uint8_t getBlockId_LevelDB_v3(const char* p, int32_t x, int32_t z, int32_t y) {
        return (p[_calcOffsetBlock_LevelDB_v3(x, z, y) + 1] & 0xff);
    }

    uint8_t getBlockData_LevelDB_v3(const char* p, size_t plen, int32_t x, int32_t z, int32_t y) {
        int32_t off = _calcOffsetBlock_LevelDB_v3(x, z, y);
        int32_t off2 = off / 2;
        int32_t mod2 = off % 2;
        // todonow - temp test to find bug
        size_t tmp_offset = (16 * 16 * 16) + 1 + off2;
        if (tmp_offset >= plen) {
            
            log::debug("getBlockData_LevelDB_v3 get data out of bounds! ({} >= {}) ({} {} {})",
                tmp_offset, plen, x, z, y);
            
            return 0;
        }
        int32_t v = p[tmp_offset];
        //int32_t v = p[(16*16*16) + 1 + off2];
        if (mod2 == 0) {
            return v & 0x0f;
        }
        else {
            return (v & 0xf0) >> 4;
        }
    }

    // todozooz - this is getting crazy
    uint8_t getBlockData_LevelDB_v3__fake_v7(const int16_t* p, size_t plen, int32_t x, int32_t z, int32_t y) {
        int32_t off = _calcOffsetBlock_LevelDB_v3(x, z, y);
        int32_t off2 = off / 2;
        int32_t mod2 = off % 2;
        // todonow - temp test to find bug
        size_t tmp_offset = (16 * 16 * 16) + 1 + off2;
        if (tmp_offset >= plen) {
            
            log::debug("getBlockData_LevelDB_v3 get data out of bounds! ({} >= {}) ({} {} {})",
                tmp_offset, plen, x, z, y);
            
            return 0;
        }
        int32_t v = p[tmp_offset];
        //int32_t v = p[(16*16*16) + 1 + off2];
        if (mod2 == 0) {
            return v & 0x0f;
        }
        else {
            return (v & 0xf0) >> 4;
        }
    }

    // a block opacity value? (e.g. glass is 0xf, water is semi (0xc) and an opaque block is 0x0)
    uint8_t getBlockSkyLight_LevelDB_v3(const char* p, size_t plen, int32_t x, int32_t z, int32_t y) {
        int32_t off = _calcOffsetBlock_LevelDB_v3(x, z, y);
        int32_t off2 = off / 2;
        int32_t mod2 = off % 2;
        // todonow - temp test to find bug
        size_t tmp_offset = (16 * 16 * 16) + 1 + (16 * 16 * 8) + off2;
        if (tmp_offset >= plen) {
            
            log::debug("getBlockSkyLight_LevelDB_v3 get data out of bounds! ({} >= {}) ({} {} {})",
                tmp_offset, plen, x, z, y);
            
            return 0;
        }
        int32_t v = p[tmp_offset];
        // int32_t v = p[(16*16*16) + 1 + (16*16*8) + off2];
        if (mod2 == 0) {
            return v & 0x0f;
        }
        else {
            return (v & 0xf0) >> 4;
        }
    }

    // block light is light value from torches et al -- super cool looking as an image, but it looks like block light is probably stored in air blocks which are above top block
    uint8_t getBlockBlockLight_LevelDB_v3(const char* p, size_t plen, int32_t x, int32_t z, int32_t y) {
        int32_t off = _calcOffsetBlock_LevelDB_v3(x, z, y);
        int32_t off2 = off / 2;
        int32_t mod2 = off % 2;
        // todonow - temp test to find bug
        size_t tmp_offset = (16 * 16 * 16) + 1 + (16 * 16 * 8) + (16 * 16 * 8) + off2;
        if (tmp_offset >= plen) {
            
            log::debug("getBlockBlockLight_LevelDB_v3 get data out of bounds! ({} >= {}) ({} {} {})",
                tmp_offset, plen, x, z, y);
            
            return 0;
        }
        int32_t v = p[tmp_offset];
        //int32_t v = p[(16*16*16) + 1 + (16*16*8) + (16*16*8) + off2];
        if (mod2 == 0) {
            return v & 0x0f;
        }
        else {
            return (v & 0xf0) >> 4;
        }
    }


    // calculate an offset into mcpe chunk data for column data
    int32_t _calcOffsetColumn_LevelDB_v3(int32_t x, int32_t z) {
        // NOTE! this is the OPPOSITE of block data (oy)
        return (z * 16) + x;
    }

    // height appears to be stored as a 2-byte int
    // height of top *solid* block? (e.g. a glass block will NOT be the top block here)
    uint8_t getColData_Height_LevelDB_v3(const char* buf, int32_t x, int32_t z) {
        int32_t off = _calcOffsetColumn_LevelDB_v3(x, z) * 2;
        int8_t v = buf[off];
        return v;
    }

    // this is 3-bytes: lsb is biome?, the high 2-bytes are RGB grass color?
    uint32_t getColData_GrassAndBiome_LevelDB_v3(const char* buf, int32_t buflen, int32_t x, int32_t z) {
        // format appears to be:
        // 16x16 of 2-byte ints for HEIGHT OF TOP BLOCK
        // 16x16 of 4-byte ints for BIOME and GRASS COLOR
        // todo -- there was a bug in early 0.17 that really messed this data up
        // tood -- grass colors are pretty weird (some are 01 01 01)
        // as of, v0.17.01 we'll just roll with it and adjust as necessary

        //int32_t off = _calcOffsetColumn_LevelDB_v3(x,z) * 4;
        int32_t off = _calcOffsetColumn_LevelDB_v3(x, z);
        int32_t v = 0;

        // HACK! to work around MCPE bug (biome data is not complete in this record as of v0.17.01
        if ((512 + off + 1) <= buflen) {
            // memcpy(&v,&buf[512 + off],1);
            v = buf[512 + off];
        }
        else {
            // nothing - this is deals with the bug in early 0.17
        }

        return v;
    }



    // todobig - this (XXX_v3_fullchunk) needs to be cleaner/simpler

    // calculate an offset into mcpe chunk data for block data
    int32_t _calcOffsetBlock_LevelDB_v3_fullchunk(int32_t x, int32_t z, int32_t y) {
        return (((x * 16) + z) * MAX_BLOCK_HEIGHT) + y;
    }

    uint8_t getData_LevelDB_v3_fullchunk(const char* p, int32_t x, int32_t z, int32_t y) {
        return p[_calcOffsetBlock_LevelDB_v3_fullchunk(x, z, y)];
    }





    // todomajor -- see tomcc gist re multiple storages in ONE cubick chunk in version == 8

    int32_t
        setupBlockVars_v7(const char* cdata, int32_t& blocksPerWord, int32_t& bitsPerBlock, int32_t& blockOffset,
            int32_t& paletteOffset) {

        int32_t v = -1;
        int32_t wordCount = -1;

        // Check sub-chunk version
        switch (cdata[0]) {
        case 0x01:
            // v1 - [version:byte][block storage]
            v = cdata[1];
            blockOffset = 1;
            break;
        case 0x08:
            // v8 - [version:byte][num_storages:byte][block storage1]...[blockStorageN]
            v = cdata[2];
            blockOffset = 3;
            break;
        case 0x09:
            // v9 - [version:byte][num_storages:byte][sub_chunk_index:byte][block storage1]...[blockStorageN]
            v = cdata[3];
            blockOffset = 4;
            break;
        default:
            log::error("Invalid SubChunk version found ({})",
                cdata[0]);
            return -1;
        }
        
        switch (v) {
        case 0x00:
            // occasional 0 size bits per block
            bitsPerBlock = 0;
            blocksPerWord = 0;
            paletteOffset = 0;
            break;
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x08:
        case 0x0a:
        case 0x0c:
        case 0x10:
        case 0x20:
            bitsPerBlock = v >> 1;
            blocksPerWord = floor(32.0 / bitsPerBlock);
            wordCount = ceil(4096.0 / blocksPerWord);
            paletteOffset = wordCount * 4 + blockOffset;
            break;
        default:
            log::error("Unknown SubChunk palette value (value = {})", v);
            return -1;
        }
        return 0;
    }

    int32_t convertChunkV7toV3(const char* cdata, size_t cdata_size, int16_t* emuchunk) {
        // we have a v7 chunk and we want to unpack it into a v3-like chunk
        // determine location of chunk palette

        // some details here: https://gist.github.com/Tomcc/a96af509e275b1af483b25c543cfbf37

        // determine location of chunk palette
        int32_t blocksPerWord = -1;
        int32_t bitsPerBlock = -1;
        int32_t blockOffset = -1;
        int32_t paletteOffset = -1;

        memset(emuchunk, 0, NUM_BYTES_CHUNK_V3 * sizeof(int16_t));

        if (setupBlockVars_v7(cdata, blocksPerWord, bitsPerBlock, blockOffset, paletteOffset) !=
            0) {
            return -1;
        }

        // read chunk palette and associate old-school block id's
        MyNbtTagList tagList;
        int xoff = paletteOffset + 4;

        parseNbtQuiet(&cdata[xoff], int32_t(cdata_size - xoff), cdata[paletteOffset], tagList);

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
                    log::warn("(Safe) Did not find 'name' tag in a chunk palette! (i={}) (len={})",
                        i, tagList.size());
                    
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
                    if (blocksPerWord != 0 && bitsPerBlock != 0) {
                        paletteBlockId = getBlockId_LevelDB_v7(&cdata[blockOffset],
                            blocksPerWord, bitsPerBlock, cx, cz, cy);
                    }
                    else {
                        paletteBlockId = 0;
                    }
                    // look up blockId
                    //todonow error checking
                    if (paletteBlockId <= chunkBlockPalette_BlockId.size()) {
                        blockId = chunkBlockPalette_BlockId[paletteBlockId];
                        blockData = chunkBlockPalette_BlockData[paletteBlockId];
                    }
                    else {
                        blockId = 0;
                        blockData = 0;
                        log::warn("Found chunk palette id out of range {} (size={})", 
                            paletteBlockId, chunkBlockPalette_BlockId.size());
                    }

                    int32_t bdoff = _calcOffsetBlock_LevelDB_v3(cx, cz, cy);

                    emuchunk[bdoff + 1] = blockId;

                    // put block data
                    int32_t bdoff2 = bdoff / 2;
                    int32_t bdmod2 = bdoff % 2;
                    // todonow - temp test to find bug
                    size_t tmp_offset = (16 * 16 * 16) + 1 + bdoff2;
                    if (bdmod2 == 0) {
                        emuchunk[tmp_offset] |= (blockData & 0x0f);
                    }
                    else {
                        emuchunk[tmp_offset] |= (blockData & 0x0f) << 4;
                    }
                }
            }
        }
        return 0;
    }
}
