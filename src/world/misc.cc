#include "misc.h"

#include "../define.h"
#include "../nbt.h"
#include "../logger.h"
#include "../util.h"
#include "../minecraft/block_info.h"

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


    uint8_t
        getBlockId_LevelDB_v7(const char* p, int blocksPerWord, int bitsPerBlock, int32_t x, int32_t z, int32_t y) {
        //int bitstart = ( (((x*16) + z) * 16) + y ) * bitsPerBlock;
        // int bitstart = ( (((y*16) + x) * 16) + z ) * bitsPerBlock;

        int blockPos = (((x * 16) + z) * 16) + y;
        // we find which 4-byte word we want
        int wordStart = blockPos / blocksPerWord;
        // we find the bit offset within that 4-byte word
        int bitOffset = (blockPos % blocksPerWord) * bitsPerBlock;
        int bitStart = wordStart * 4 * 8 + bitOffset;
        return getBitsFromBytes(p, bitStart, bitsPerBlock);
    }


    // todomajor -- see tomcc gist re multiple storages in ONE cubick chunk in version == 8

    int32_t
        setupBlockVars_v7(const char* cdata, int32_t& blocksPerWord, int32_t& bitsPerBlock, bool& paddingFlag,
            int32_t& offsetBlockInfoList, int32_t& extraOffset) {

        int32_t v = -1;

        if (cdata[0] == 0x01) {
            v = cdata[1];
            extraOffset = 0;
        }
        else {
            // this is version 8+, cdata[1] contains the number of storage groups in this cubic chunk (can be more than 1)
            v = cdata[2];
            extraOffset = 1;
        }

        switch (v) {
        case 0x02:
            blocksPerWord = 32;
            bitsPerBlock = 1;
            offsetBlockInfoList = 512;
            break;
        case 0x04:
            blocksPerWord = 16;
            bitsPerBlock = 2;
            offsetBlockInfoList = 1024;
            break;
        case 0x06:
            blocksPerWord = 10;
            bitsPerBlock = 3;
            paddingFlag = true;
            offsetBlockInfoList = 1640;
            break;
        case 0x08:
            blocksPerWord = 8;
            bitsPerBlock = 4;
            offsetBlockInfoList = 2048;
            break;
        case 0x0a:
            blocksPerWord = 6;
            bitsPerBlock = 5;
            paddingFlag = true;
            offsetBlockInfoList = 2732;
            break;
        case 0x0c:
            blocksPerWord = 5;
            bitsPerBlock = 6;
            paddingFlag = true;
            offsetBlockInfoList = 3280;
            break;
        case 0x10:
            blocksPerWord = 4;
            bitsPerBlock = 8;
            offsetBlockInfoList = (4096 / blocksPerWord) * 4;
            break;
        case 0x20:
            blocksPerWord = 2;
            bitsPerBlock = 16;
            offsetBlockInfoList = (4096 / blocksPerWord) * 4;
            break;
        default:
            log::error("Unknown chunk cdata[1] value = {}", v);
            return -1;
        }

        //    logger.msg(kLogInfo, "setupBlockVars_v7 v=%d bpw=%d bpb=%d pf=%d ob=%d\n", v, blocksPerWord, bitsPerBlock, (int)paddingFlag, offsetBlockInfoList);
        return 0;
    }

    int32_t convertChunkV7toV3(const char* cdata, size_t cdata_size, int16_t* emuchunk) {
        // we have a v7 chunk and we want to unpack it into a v3-like chunk
        // determine location of chunk palette

        // some details here: https://gist.github.com/Tomcc/a96af509e275b1af483b25c543cfbf37

        int32_t blocksPerWord = -1;
        int32_t bitsPerBlock = -1;
        bool paddingFlag = false;
        int32_t offsetBlockInfoList = -1;
        int32_t extraOffset = -1;

        memset(emuchunk, 0, NUM_BYTES_CHUNK_V3 * sizeof(int16_t));

        if (setupBlockVars_v7(cdata, blocksPerWord, bitsPerBlock, paddingFlag, offsetBlockInfoList, extraOffset) != 0) {
            return -1;
        }

        // read chunk palette and associate old-school block id's
        MyNbtTagList tagList;
        int xoff = offsetBlockInfoList + 6 + extraOffset;
        parseNbtQuiet(&cdata[xoff], int32_t(cdata_size) - xoff, cdata[offsetBlockInfoList + 3], tagList);

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
                    int32_t blockId, blockData;
                    if (getBlockByUname(bname, blockId, blockData) == 0) {
                        chunkBlockPalette_BlockId[i] = blockId;
                        // todonow - correct?
                        chunkBlockPalette_BlockData[i] = bdata;
                    }
                    else {
                        logger.msg(kLogWarning, "Did not find block uname '%s' in XML file\n", bname.c_str());
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
                logger.msg(kLogWarning, "Unexpected NBT format in _do_chunk_v7\n");
            }
        }

        //todozooz -- new 16-bit block-id's (instead of 8-bit) are a BIG issue - this needs attention here
        // iterate over chunk space
        uint8_t paletteBlockId, blockData;
        int32_t blockId;
        for (int32_t cy = 0; cy < 16; cy++) {
            for (int32_t cx = 0; cx < 16; cx++) {
                for (int32_t cz = 0; cz < 16; cz++) {
                    paletteBlockId = getBlockId_LevelDB_v7(&cdata[2 + extraOffset], blocksPerWord, bitsPerBlock, cx, cz,
                        cy);

                    // look up blockId
                    //todonow error checking
                    if (paletteBlockId < chunkBlockPalette_BlockId.size()) {
                        blockId = chunkBlockPalette_BlockId[paletteBlockId];
                        blockData = chunkBlockPalette_BlockData[paletteBlockId];
                    }
                    else {
                        blockId = 0;
                        blockData = 0;
                        logger.msg(kLogWarning, "Found chunk palette id out of range %d (size=%d)\n", paletteBlockId,
                            (int)chunkBlockPalette_BlockId.size());
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