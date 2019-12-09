#include "dimension_data.h"

#include <random>

#include "../minecraft/block_info.h"
#include "../minecraft/biome_info.h"

#include "../control.h"
#include "../utils/block_recorder.h"
#include "palettes.h"
#include "common.h"
#include "misc.h"
#include "point_conversion.h"
#include "../global.h"
#include "../nbt.h"

namespace mcpe_viz {

    bool DimensionData_LevelDB::checkDoForDim(int32_t v) const
    {
        if (v == kDoOutputAll) {
            return true;
        }
        if (v == dimId) {
            return true;
        }
        return false;
    }

    void DimensionData_LevelDB::doOutputStats()
    {
        logger.msg(kLogInfo1, "\n%s Statistics:\n", name.c_str());
        logger.msg(kLogInfo1, "chunk-count: %d\n", (int)chunks.size());
        logger.msg(kLogInfo1, "Min-dim:  %d %d\n", minChunkX, minChunkZ);
        logger.msg(kLogInfo1, "Max-dim:  %d %d\n", maxChunkX, maxChunkZ);
        int32_t dx = (maxChunkX - minChunkX + 1);
        int32_t dz = (maxChunkZ - minChunkZ + 1);
        logger.msg(kLogInfo1, "diff-dim: %d %d\n", dx, dz);
        logger.msg(kLogInfo1, "pixels:   %d %d\n", dx * 16, dz * 16);

        logger.msg(kLogInfo1, "\nGlobal Chunk Type Histogram:\n");
        for (int32_t i = 0; i < 256; i++) {
            if (histogramChunkType[i] > 0) {
                logger.msg(kLogInfo1, "hg-chunktype: %02x %6d\n", i, histogramChunkType[i]);
            }
        }

        double htotal;
        HistogramVector hvector;

        logger.msg(kLogInfo1, "\nGlobal Block Histogram (block-id count pct name):\n");
        htotal = histogramGlobalBlock.getTotal();
        hvector = histogramGlobalBlock.sort(1);
        for (auto& it : hvector) {
            int32_t k = it.first;
            int32_t v = it.second;
            double pct = ((double)v / htotal) * 100.0;
            logger.msg(kLogInfo1, "hg-globalblock: 0x%02x %10d %7.3lf%% %s\n", k, v, pct,
                blockInfoList[k].name.c_str());
        }

        logger.msg(kLogInfo1, "\nGlobal Biome Histogram (biome-id count pct name):\n");
        htotal = histogramGlobalBiome.getTotal();
        hvector = histogramGlobalBiome.sort(1);
        for (auto& it : hvector) {
            int32_t k = it.first;
            int32_t v = it.second;
            double pct = ((double)v / htotal) * 100.0;
            //      logger.msg(kLogInfo1,"hg-globalbiome: 0x%02x %10d %7.3lf%% %s\n", k, v, pct, biomeInfoList[k]->name.c_str());
            logger.msg(kLogInfo1, "hg-globalbiome: 0x%02x %10d %7.3lf%%\n", k, v, pct);
        }
    }

    int32_t DimensionData_LevelDB::generateImage(const std::string& fname, const ImageModeType imageMode)
    {
        const int32_t chunkOffsetX = -minChunkX;
        const int32_t chunkOffsetZ = -minChunkZ;

        const int32_t chunkW = (maxChunkX - minChunkX + 1);
        const int32_t chunkH = (maxChunkZ - minChunkZ + 1);
        const int32_t imageW = chunkW * 16;
        const int32_t imageH = chunkH * 16;

        int32_t bpp = 3;
        bool rgbaFlag = false;
        uint8_t lut[256];

        if (imageMode == kImageModeHeightColAlpha) {
            bpp = 4;
            rgbaFlag = true;
            // todobig - experiment with other ways to do this lut for height alpha
            double vmax = (double)MAX_BLOCK_HEIGHT * (double)MAX_BLOCK_HEIGHT;
            for (int32_t i = 0; i <= MAX_BLOCK_HEIGHT; i++) {
                // todobig make the offset (32) a cmdline param
                double ti = ((MAX_BLOCK_HEIGHT + 1) + 32) - i;
                double v = ((double)(ti * ti) / vmax) * 255.0;
                if (v > 235.0) { v = 235.0; }
                if (v < 0.0) { v = 0.0; }
                lut[i] = v;
            }
        }

        // todohere -- reddit user silvergoat77 has a 1gb (!) world and it is approx 33k x 26k -- alloc chokes on this.
        // the solution is to write a chunk of rows at a time instead of the whole image...
        // but -- the code below is optimized to just iterate through the list and do it's thing instead of searching for each chunk
        // so --- we need to test before / after changing this to step thru in Z/X order

        // note RGB pixels
        uint8_t* buf = new uint8_t[imageW * 16 * bpp];

        uint8_t* rows[16];
        for (int i = 0; i < 16; i++) {
            rows[i] = &buf[i * imageW * bpp];
        }

        int32_t color;
        const char* pcolor;
        if (bpp == 4) {
            pcolor = (const char*)&color;
        }
        else {
            const char* pcolor_temp = (const char*)&color;
            pcolor = &pcolor_temp[1];
        }

        PngWriter png;
        if (outputPNG_init(png, fname, makeImageDescription(imageMode, 0), imageW, imageH, rgbaFlag) != 0) {
            delete[] buf;
            return -1;
        }

        for (int32_t iz = 0, chunkZ = minChunkZ; iz < imageH; iz += 16, chunkZ++) {

            // clear buffer
            memset(buf, 0, imageW * 16 * bpp);

            for (int32_t ix = 0, chunkX = minChunkX; ix < imageW; ix += 16, chunkX++) {

                ChunkKey chunkKey(chunkX, chunkZ);
                if (!chunks_has_key(chunks, chunkKey)) {
                    continue;
                }

                const auto& it = chunks.at(chunkKey);

                int32_t imageX = (it->chunkX + chunkOffsetX) * 16;
                int32_t imageZ = (it->chunkZ + chunkOffsetZ) * 16;

                int32_t worldX = it->chunkX * 16;
                int32_t worldZ = it->chunkZ * 16;

                for (int32_t cz = 0; cz < 16; cz++) {
                    for (int32_t cx = 0; cx < 16; cx++) {

                        // todobig - we could do EVERYTHING (but initial key scan) in one pass:
                        //   do images here, then iterate over chunkspace again looking for items that populate geojson list

                        // todo - this big conditional inside an inner loop, not so good

                        if (imageMode == kImageModeBiome) {
                            // get biome color
                            int32_t biomeId = it->grassAndBiome[cx][cz] & 0xff;
                            if (has_key(biomeInfoList, biomeId)) {
                                color = biomeInfoList[biomeId]->color;
                            }
                            else {
                                slogger.msg(kLogInfo1, "ERROR: Unknown biome %d 0x%x\n", biomeId, biomeId);
                                color = htobe32(0xff2020);
                            }
                        }
                        else if (imageMode == kImageModeGrass) {
                            // get grass color
                            int32_t grassColor = it->grassAndBiome[cx][cz] >> 8;
                            color = htobe32(grassColor);
                        }
                        else if (imageMode == kImageModeHeightCol) {
                            // get height value and use red-black-green palette
                            if (control.heightMode == kHeightModeTop) {
                                uint8_t c = it->topBlockY[cx][cz];
                                color = palRedBlackGreen[c];
                            }
                            else {
                                uint8_t c = it->heightCol[cx][cz];
                                color = palRedBlackGreen[c];
                            }
                        }
                        else if (imageMode == kImageModeHeightColGrayscale) {
                            // get height value and make it grayscale
                            if (control.heightMode == kHeightModeTop) {
                                uint8_t c = it->topBlockY[cx][cz];
                                color = (c << 24) | (c << 16) | (c << 8);
                            }
                            else {
                                uint8_t c = it->heightCol[cx][cz];
                                color = (c << 24) | (c << 16) | (c << 8);
                            }
                        }
                        else if (imageMode == kImageModeHeightColAlpha) {
                            // get height value and make it alpha
                            uint8_t c;
                            if (control.heightMode == kHeightModeTop) {
                                c = it->topBlockY[cx][cz];
                            }
                            else {
                                c = it->heightCol[cx][cz];
                            }
                            // c = (90 - (int32_t)it->heightCol[cx][cz]) * 2;
                            c = lut[c];
                            color = ((c & 0xff) << 24);
                        }
                        else if (imageMode == kImageModeBlockLight) {
                            // get block light value and expand it (is only 4-bits)
                            uint8_t c = (it->topLight[cx][cz] & 0x0f) << 4;
                            color = (c << 24) | (c << 16) | (c << 8);
                        }
                        else if (imageMode == kImageModeSkyLight) {
                            // get sky light value and expand it (is only 4-bits)
                            uint8_t c = (it->topLight[cx][cz] & 0xf0);
                            color = (c << 24) | (c << 16) | (c << 8);
                        }
                        else {
                            // regular image
                            int32_t blockid = it->blocks[cx][cz];

                            if (blockInfoList[blockid].hasVariants()) {
                                // we need to get blockdata
                                int32_t blockdata = it->data[cx][cz];
                                bool vfound = false;
                                for (const auto& itbv : blockInfoList[blockid].variantList) {
                                    if (itbv->blockdata == blockdata) {
                                        vfound = true;
                                        color = itbv->color;
                                        break;
                                    }
                                }
                                if (!vfound) {
                                    BlockRecorder::instance().addUnknownBlockVariant(
                                        blockid,
                                        blockInfoList[blockid].name,
                                        blockdata
                                    );
                                    // since we did not find the variant, use the parent block's color
                                    color = blockInfoList[blockid].color;
                                }
                            }
                            else {
                                color = blockInfoList[blockid].color;
                                if (!blockInfoList[blockid].colorSetFlag) {
                                    blockInfoList[blockid].colorSetNeedCount++;
                                }
                            }
                        }

                        // do grid lines
                        if (checkDoForDim(control.doGrid) && (cx == 0 || cz == 0)) {
                            if ((it->chunkX == 0) && (it->chunkZ == 0) && (cx == 0) && (cz == 0)) {
                                color = htobe32(0xeb3333);
                            }
                            else {
                                color = htobe32(0xc1ffc4);
                            }
                        }

#ifdef PIXEL_COPY_MEMCPY
                        memcpy(&buf[((cz)*imageW + (imageX + cx)) * bpp], pcolor, bpp);
#else
                        // todobig - support for bpp here
// todo - any use in optimizing the offset calc?
                        buf[((cz)*imageW + (imageX + cx)) * 3] = pcolor[1];
                        buf[((cz)*imageW + (imageX + cx)) * 3 + 1] = pcolor[2];
                        buf[((cz)*imageW + (imageX + cx)) * 3 + 2] = pcolor[3];
#endif

                        // report interesting coordinates
                        if (dimId == kDimIdOverworld && imageMode == kImageModeTerrain) {
                            int32_t tix = (imageX + cx);
                            int32_t tiz = (imageZ + cz);
                            int32_t twx = (worldX + cx);
                            int32_t twz = (worldZ + cz);
                            if ((twx == 0) && (twz == 0)) {
                                slogger.msg(kLogInfo1, "    Info: World (0, 0) is at image (%d, %d)\n", tix, tiz);
                            }
                            // todobig - just report this somwhere instead of having to pass the spawn params
                            if ((twx == worldSpawnX) && (twz == worldSpawnZ)) {
                                slogger.msg(kLogInfo1, "    Info: World Spawn (%d, %d) is at image (%d, %d)\n",
                                    worldSpawnX, worldSpawnZ, tix, tiz);
                            }
                        }
                    }
                }
            }
            // write rows
            outputPNG_writeRows(png, rows, 16);
        }

        // output the image
        outputPNG_close(png);

        delete[] buf;

        // report items that need to have their color set properly (in the XML file)
        if (imageMode == kImageModeTerrain) {
            for (int32_t i = 0; i < 512; i++) {
                if (blockInfoList[i].colorSetNeedCount) {
                    slogger.msg(kLogInfo1, "    Need pixel color for: 0x%x '%s' (%d)\n", i,
                        blockInfoList[i].name.c_str(), blockInfoList[i].colorSetNeedCount);
                }
            }
        }
        return 0;
    }

    bool DimensionData_LevelDB::isSlimeChunk_MCPE(int32_t cX, int32_t cZ)
    {
        //
            // MCPE slime-chunk checker
            // From Minecraft: Pocket Edition 0.15.0 (0.15.0.50_V870150050)
            // Reverse engineered by @protolambda and @jocopa3
            //
            // NOTE:
            // - The world-seed doesn't seem to be incorporated into the randomness, which is very odd.
            //   This means that every world has its slime-chunks in the exact same chunks!
            //   This is not officially confirmed yet.
            // - Reverse engineering this code cost a lot of time,
            //   please add CREDITS when you are copying this.
            //   Copy the following into your program source:
            //     MCPE slime-chunk checker; reverse engineered by @protolambda and @jocopa3
            //

            // chunkX/Z are the chunk-coordinates, used in the DB keys etc.
            // Unsigned int32, using 64 bit longs to work-around the sign issue.
        int64_t chunkX_uint = cX & 0xffffffffL;
        int64_t chunkZ_uint = cZ & 0xffffffffL;

        // Combine X and Z into a 32 bit int (again, long to work around sign issue)
        int64_t seed = (chunkX_uint * 0x1f1f1f1fL) ^ chunkZ_uint;

        // The random function MCPE uses, not the same as MCPC!
        // This is a Mersenne Twister; MT19937 by Takuji Nishimura and Makoto Matsumoto.
        // Java adaption source: http://dybfin.wustl.edu/teaching/compufinj/MTwister.java
        //MTwister random = new MTwister();
        std::mt19937 random;
        //random.init_genrand(seed);
        random.seed(seed);

        // The output of the random function, first operand of the asm umull instruction
        //int64_tn = random.genrand_int32();
        int64_t n = random();

        // The other operand, magic bit number that keeps characteristics
        // In binary: 1100 1100 1100 1100 1100 1100 1100 1101
        int64_t m = 0xcccccccdL;

        // umull (unsigned long multiplication)
        // Stores the result of multiplying two int32 integers in two registers (lo and hi).
        // Java workaround: store the result in a 64 bit long, instead of two 32 bit registers.
        int64_t product = n * m;

        // The umull instruction puts the result in a lo and a hi register, the lo one is not used.
        int64_t hi = (product >> 32) & 0xffffffffL;

        // Make room for 3 bits, preparation for decrease of randomness by a factor 10.
        int64_t hi_shift3 = (hi >> 0x3) & 0xffffffffL;

        // Multiply with 10 (3 bits)
        // ---> effect: the 3 bit randomness decrease expresses a 1 in a 10 chance.
        int64_t res = (((hi_shift3 + (hi_shift3 * 0x4)) & 0xffffffffL) * 0x2) & 0xffffffffL;

        // Final check: is the input equal to 10 times less random, but comparable, output.
        // Every chunk has a 1 in 10 chance to be a slime-chunk.
        return n == res;
    }

    int32_t DimensionData_LevelDB::generateSlices(leveldb::DB* db, const std::string& fnBase)
    {
        const int32_t chunkOffsetX = -minChunkX;
        const int32_t chunkOffsetZ = -minChunkZ;

        const int32_t chunkW = (maxChunkX - minChunkX + 1);
        const int32_t chunkH = (maxChunkZ - minChunkZ + 1);
        const int32_t imageW = chunkW * 16;
        const int32_t imageH = chunkH * 16;

        char keybuf[128];
        int32_t keybuflen;
        int32_t kw = dimId;
        uint8_t kt = 0x30;
        uint8_t kt_v3 = 0x2f;
        leveldb::Status dstatus;

        slogger.msg(kLogInfo1, "    Writing all images in one pass\n");

        std::string svalue;

        int32_t color;
        const char* pcolor = (const char*)&color;

        int16_t* emuchunk = new int16_t[NUM_BYTES_CHUNK_V3];

        // create png helpers
        PngWriter png[MAX_BLOCK_HEIGHT + 1];
        for (int32_t cy = 0; cy <= MAX_BLOCK_HEIGHT; cy++) {
            std::string fnameTmp = fnBase + ".mcpe_viz_slice.full.";
            fnameTmp += name;
            fnameTmp += ".";
            sprintf(keybuf, "%03d", cy);
            fnameTmp += keybuf;
            fnameTmp += ".png";

            control.fnLayerRaw[dimId][cy] = fnameTmp;

            if (png[cy].init(fnameTmp, makeImageDescription(-1, cy), imageW, imageH, 16, false, true) != 0) {
                delete[] emuchunk;
                return -1;
            }
        }

        // create row buffers
        uint8_t* rbuf[MAX_BLOCK_HEIGHT + 1];
        for (int32_t cy = 0; cy <= MAX_BLOCK_HEIGHT; cy++) {
            rbuf[cy] = new uint8_t[(imageW * 3) * 16];
            // setup row pointers
            for (int32_t cz = 0; cz < 16; cz++) {
                png[cy].row_pointers[cz] = &rbuf[cy][(cz * imageW) * 3];
            }
        }

        // create a helper buffer which contains topBlockY for the entire image
        uint8_t currTopBlockY = MAX_BLOCK_HEIGHT;
        size_t bsize = (size_t)imageW * (size_t)imageH;
        uint8_t* tbuf = new uint8_t[bsize];
        memset(tbuf, MAX_BLOCK_HEIGHT, bsize);
        for (const auto& it : chunks) {
            int32_t ix = (it.second->chunkX + chunkOffsetX) * 16;
            int32_t iz = (it.second->chunkZ + chunkOffsetZ) * 16;
            for (int32_t cz = 0; cz < 16; cz++) {
                for (int32_t cx = 0; cx < 16; cx++) {
                    tbuf[(iz + cz) * imageW + (ix + cx)] = it.second->topBlockY[cx][cz];
                }
            }
        };

        int32_t foundCt = 0, notFoundCt2 = 0;
        //todozooz -- new 16-bit block-id's (instead of 8-bit) are a BIG issue - this needs attention here
        uint8_t blockdata;
        int32_t blockid;

        // we operate on sets of 16 rows (which is one chunk high) of image z
        int32_t runCt = 0;
        for (int32_t imageZ = 0, chunkZ = minChunkZ; imageZ < imageH; imageZ += 16, chunkZ++) {

            if ((runCt++ % 20) == 0) {
                slogger.msg(kLogInfo1, "    Row %d of %d\n", imageZ, imageH);
            }

            for (int32_t imageX = 0, chunkX = minChunkX; imageX < imageW; imageX += 16, chunkX++) {

                // FIRST - we try pre-0.17 chunks

                // construct key to get the chunk
                if (dimId == kDimIdOverworld) {
                    //overworld
                    memcpy(&keybuf[0], &chunkX, sizeof(int32_t));
                    memcpy(&keybuf[4], &chunkZ, sizeof(int32_t));
                    memcpy(&keybuf[8], &kt, sizeof(uint8_t));
                    keybuflen = 9;
                }
                else {
                    // nether (and probably any others that are added)
                    memcpy(&keybuf[0], &chunkX, sizeof(int32_t));
                    memcpy(&keybuf[4], &chunkZ, sizeof(int32_t));
                    memcpy(&keybuf[8], &kw, sizeof(int32_t));
                    memcpy(&keybuf[12], &kt, sizeof(uint8_t));
                    keybuflen = 13;
                }

                dstatus = db->Get(levelDbReadOptions, leveldb::Slice(keybuf, keybuflen), &svalue);
                if (dstatus.ok()) {

                    // we got a pre-0.17 chunk
                    const char* ochunk = nullptr;
                    const char* pchunk = nullptr;

                    pchunk = svalue.data();
                    ochunk = pchunk;
                    // size_t ochunk_size = svalue.size();
                    foundCt++;

                    // we step through the chunk in the natural order to speed things up
                    for (int32_t cx = 0; cx < 16; cx++) {
                        for (int32_t cz = 0; cz < 16; cz++) {
                            currTopBlockY = tbuf[(imageZ + cz) * imageW + imageX + cx];
                            for (int32_t cy = 0; cy <= MAX_BLOCK_HEIGHT_127; cy++) {

                                // todo - if we use this, we get blockdata errors... somethings not right
                                //blockid = *(pchunk++);
                                blockid = getBlockId_LevelDB_v2(ochunk, cx, cz, cy);

                                if (blockid == 0 && (cy > currTopBlockY) && (dimId != kDimIdNether)) {

                                    // special handling for air -- keep existing value if we are above top block
                                    // the idea is to show air underground, but hide it above so that the map is not all black pixels @ y=MAX_BLOCK_HEIGHT
                                    // however, we do NOT do this for the nether. because: the nether

                                    // we need to copy this pixel from another layer
                                    memcpy(&rbuf[cy][((cz * imageW) + imageX + cx) * 3],
                                        &rbuf[currTopBlockY][((cz * imageW) + imageX + cx) * 3],
                                        3);

                                }
                                else {

                                    if (blockInfoList[blockid].hasVariants()) {
                                        // we need to get blockdata

                                        blockdata = getBlockData_LevelDB_v2(ochunk, cx, cz, cy);

                                        bool vfound = false;
                                        for (const auto& itbv : blockInfoList[blockid].variantList) {
                                            if (itbv->blockdata == blockdata) {
                                                vfound = true;
                                                color = itbv->color;
                                                break;
                                            }
                                        }
                                        if (!vfound) {
                                            BlockRecorder::instance().addUnknownBlockVariant(
                                                blockid,
                                                blockInfoList[blockid].name,
                                                blockdata
                                            );
                                            // since we did not find the variant, use the parent block's color
                                            color = blockInfoList[blockid].color;
                                        }
                                    }
                                    else {
                                        color = blockInfoList[blockid].color;
                                    }

#ifdef PIXEL_COPY_MEMCPY
                                    memcpy(&rbuf[cy][((cz * imageW) + imageX + cx) * 3], &pcolor[1], 3);
#else
                                    // todo - any use in optimizing the offset calc?
                                    rbuf[cy][((cz * imageW) + imageX + cx) * 3] = pcolor[1];
                                    rbuf[cy][((cz * imageW) + imageX + cx) * 3 + 1] = pcolor[2];
                                    rbuf[cy][((cz * imageW) + imageX + cx) * 3 + 2] = pcolor[3];
#endif
                                }
                            }

                            // to support 256h worlds, for v2 chunks, we need to make 128..255 the same as 127
                            // todo - could optimize this
                            for (int cy = 128; cy <= MAX_BLOCK_HEIGHT; cy++) {
                                memcpy(&rbuf[cy][((cz * imageW) + imageX + cx) * 3],
                                    &rbuf[127][((cz * imageW) + imageX + cx) * 3], 3);
                            }

                        }
                    }
                }
                else {

                    // we did NOT find a pre-0.17 chunk...

                    // SECOND -- we try post 0.17 chunks

                    // we need to iterate over all possible y cubic chunks here...
                    int32_t cubicFoundCount = 0;
                    for (int8_t cubicy = 0; cubicy < MAX_CUBIC_Y; cubicy++) {

                        // todobug - this fails around level 112? on another1 -- weird -- run a valgrind to see where we're messing up
                        //check valgrind output

                        // construct key to get the chunk
                        if (dimId == kDimIdOverworld) {
                            //overworld
                            memcpy(&keybuf[0], &chunkX, sizeof(int32_t));
                            memcpy(&keybuf[4], &chunkZ, sizeof(int32_t));
                            memcpy(&keybuf[8], &kt_v3, sizeof(uint8_t));
                            memcpy(&keybuf[9], &cubicy, sizeof(uint8_t));
                            keybuflen = 10;
                        }
                        else {
                            // nether (and probably any others that are added)
                            memcpy(&keybuf[0], &chunkX, sizeof(int32_t));
                            memcpy(&keybuf[4], &chunkZ, sizeof(int32_t));
                            memcpy(&keybuf[8], &kw, sizeof(int32_t));
                            memcpy(&keybuf[12], &kt_v3, sizeof(uint8_t));
                            memcpy(&keybuf[13], &cubicy, sizeof(uint8_t));
                            keybuflen = 14;
                        }

                        dstatus = db->Get(levelDbReadOptions, leveldb::Slice(keybuf, keybuflen), &svalue);
                        if (dstatus.ok()) {
                            cubicFoundCount++;

                            // we got a post-0.17 cubic chunk

                            const char* rchunk = svalue.data();
                            const int16_t* pchunk_word = (int16_t*)svalue.data();
                            const char* pchunk_byte = (char*)svalue.data();
                            size_t ochunk_size = svalue.size();
                            const int16_t* ochunk_word = pchunk_word;
                            const char* ochunk_byte = pchunk_byte;
                            bool wordModeFlag = false;
                            foundCt++;

                            // determine if it is a v7 chunk and process accordingly
                            //todozooz - here is where it gets weird
                            if (rchunk[0] != 0x0) {
                                // we have a v7 chunk - emulate v3
                                convertChunkV7toV3(rchunk, ochunk_size, emuchunk);
                                wordModeFlag = true;
                                pchunk_word = emuchunk;
                                ochunk_word = emuchunk;
                                ochunk_size = NUM_BYTES_CHUNK_V3;
                            }
                            else {
                                wordModeFlag = false;
                                // slogger.msg(kLogWarning,"Found a non-v7 chunk\n");
                            }

                            // the first byte is not interesting to us (it is version #?)
                            pchunk_word++;
                            pchunk_byte++;

                            // we step through the chunk in the natural order to speed things up
                            for (int32_t cx = 0; cx < 16; cx++) {
                                for (int32_t cz = 0; cz < 16; cz++) {
                                    currTopBlockY = tbuf[(imageZ + cz) * imageW + imageX + cx];
                                    for (int32_t ccy = 0; ccy < 16; ccy++) {
                                        int32_t cy = cubicy * 16 + ccy;

                                        // todo - if we use this, we get blockdata errors... somethings not right
                                        if (wordModeFlag) {
                                            blockid = *(pchunk_word++);
                                        }
                                        else {
                                            //todozooz - getting blockid manually fixes issue
                                            // blockid = *(pchunk_byte++);
                                            blockid = getBlockId_LevelDB_v3(ochunk_byte, cx, cz, ccy);
                                        }

                                        // blockid = getBlockId_LevelDB_v3(ochunk, cx,cz,ccy);

                                        if (blockid == 0 && (cy > currTopBlockY) && (dimId != kDimIdNether)) {

                                            // special handling for air -- keep existing value if we are above top block
                                            // the idea is to show air underground, but hide it above so that the map is not all black pixels @ y=MAX_BLOCK_HEIGHT
                                            // however, we do NOT do this for the nether. because: the nether

                                            // we need to copy this pixel from another layer
                                            memcpy(&rbuf[cy][((cz * imageW) + imageX + cx) * 3],
                                                &rbuf[currTopBlockY][((cz * imageW) + imageX + cx) * 3],
                                                3);

                                        }
                                        else {

                                            if (blockid >= 0 && blockid < 512) {
                                                if (blockInfoList[blockid].hasVariants()) {
                                                    // we need to get blockdata

                                                    if (wordModeFlag) {
                                                        blockdata = getBlockData_LevelDB_v3__fake_v7(ochunk_word,
                                                            ochunk_size,
                                                            cx, cz, ccy);
                                                    }
                                                    else {
                                                        blockdata = getBlockData_LevelDB_v3(ochunk_byte,
                                                            ochunk_size, cx, cz,
                                                            ccy);
                                                    }

                                                    bool vfound = false;
                                                    for (const auto& itbv : blockInfoList[blockid].variantList) {
                                                        if (itbv->blockdata == blockdata) {
                                                            vfound = true;
                                                            color = itbv->color;
                                                            break;
                                                        }
                                                    }
                                                    if (!vfound) {
                                                        BlockRecorder::instance().addUnknownBlockVariant(
                                                            blockid,
                                                            blockInfoList[blockid].name,
                                                            blockdata
                                                        );
                                                        // since we did not find the variant, use the parent block's color
                                                        color = blockInfoList[blockid].color;
                                                    }
                                                }
                                                else {
                                                    color = blockInfoList[blockid].color;
                                                }
                                            }
                                            else {
                                                // bad blockid
                                                //todozooz todostopper - we get a lot of these w/ negative blockid around row 4800 of world 'another1'
                                                slogger.msg(kLogError,
                                                    "Invalid blockid=%d (image %d %d) (cc %d %d %d)\n",
                                                    blockid, imageX, imageZ, cx, cz, cy
                                                );
                                                // set an unused color
                                                color = htobe32(0xf010d0);
                                            }

#ifdef PIXEL_COPY_MEMCPY
                                            memcpy(&rbuf[cy][((cz * imageW) + imageX + cx) * 3], &pcolor[1], 3);
#else
                                            // todo - any use in optimizing the offset calc?
                                            rbuf[cy][((cz * imageW) + imageX + cx) * 3] = pcolor[1];
                                            rbuf[cy][((cz * imageW) + imageX + cx) * 3 + 1] = pcolor[2];
                                            rbuf[cy][((cz * imageW) + imageX + cx) * 3 + 2] = pcolor[3];
#endif
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            // we did NOT find the cubic chunk, which means that it is 100% air

                            for (int32_t cx = 0; cx < 16; cx++) {
                                for (int32_t cz = 0; cz < 16; cz++) {
                                    currTopBlockY = tbuf[(imageZ + cz) * imageW + imageX + cx];
                                    for (int32_t ccy = 0; ccy < 16; ccy++) {
                                        int32_t cy = cubicy * 16 + ccy;
                                        if ((cy > currTopBlockY) && (dimId != kDimIdNether)) {
                                            // special handling for air -- keep existing value if we are above top block
                                            // the idea is to show air underground, but hide it above so that the map is not all black pixels @ y=MAX_BLOCK_HEIGHT
                                            // however, we do NOT do this for the nether. because: the nether

                                            // we need to copy this pixel from another layer
                                            memcpy(&rbuf[cy][((cz * imageW) + imageX + cx) * 3],
                                                &rbuf[currTopBlockY][((cz * imageW) + imageX + cx) * 3],
                                                3);
                                        }
                                        else {
                                            memset(&rbuf[cy][((cz * imageW) + imageX + cx) * 3], 0, 3);
                                        }
                                    }
                                }
                            }
                        }
                    }

                    if (cubicFoundCount <= 0) {

                        // FINALLY -- we did not find the chunk at all
                        notFoundCt2++;
                        // slogger.msg(kLogInfo1,"WARNING: Did not find chunk in leveldb x=%d z=%d status=%s\n", chunkX, chunkZ, dstatus.ToString().c_str());

                        // we need to clear this area
                        for (int32_t cy = 0; cy <= MAX_BLOCK_HEIGHT; cy++) {
                            for (int32_t cz = 0; cz < 16; cz++) {
                                memset(&rbuf[cy][((cz * imageW) + imageX) * 3], 0, 16 * 3);
                            }
                        }
                        // todonow - need this?
                        //continue;
                    }
                }

            }

            // put the png rows
            // todo - png lib is SLOW - worth it to alloc a larger window (16-row increments) and write in batches?
            for (int32_t cy = 0; cy <= MAX_BLOCK_HEIGHT; cy++) {
                png_write_rows(png[cy].png, png[cy].row_pointers, 16);
            }
        }

        for (int32_t cy = 0; cy <= MAX_BLOCK_HEIGHT; cy++) {
            delete[] rbuf[cy];
            png[cy].close();
        }

        delete[] tbuf;

        // slogger.msg(kLogInfo1,"    Chunk Info: Found = %d / Not Found (our list) = %d / Not Found (leveldb) = %d\n", foundCt, notFoundCt1, notFoundCt2);

        delete[] emuchunk;
        return 0;
    }

    int32_t DimensionData_LevelDB::generateMovie(leveldb::DB* db, const std::string& fnBase, const std::string& fnOut, bool makeMovieFlag, bool useCropFlag)
    {
        const int32_t chunkOffsetX = -minChunkX;
        const int32_t chunkOffsetZ = -minChunkZ;

        const int32_t chunkW = (maxChunkX - minChunkX + 1);
        const int32_t chunkH = (maxChunkZ - minChunkZ + 1);
        const int32_t imageW = chunkW * 16;
        const int32_t imageH = chunkH * 16;

        int32_t divisor = 1;
        if (dimId == kDimIdNether) {
            // if nether, we divide coordinates by 8
            divisor = 8;
        }

        int32_t cropX, cropZ, cropW, cropH;

        if (useCropFlag) {
            cropX = control.movieX / divisor;
            cropZ = control.movieY / divisor;
            cropW = control.movieW / divisor;
            cropH = control.movieH / divisor;
        }
        else {
            cropX = cropZ = 0;
            cropW = imageW;
            cropH = imageH;
        }

        // note RGB pixels
        uint8_t* buf = new uint8_t[cropW * cropH * 3];
        memset(buf, 0, cropW * cropH * 3);

        // todobig - we *could* write image data to flat files during dbParse and then convert
        //   these flat files into png here (but temp disk space requirements are *huge*); could try gzwrite etc

        std::string svalue;
        const char* pchunk = nullptr;
        int32_t pchunkX = 0;
        int32_t pchunkZ = 0;

        int32_t color;
        const char* pcolor = (const char*)&color;
        for (int32_t cy = 0; cy <= MAX_BLOCK_HEIGHT; cy++) {
            // todo - make this part a func so that user can ask for specific slices from the cmdline?
            slogger.msg(kLogInfo1, "  Layer %d\n", cy);
            for (const auto& it : chunks) {
                int32_t imageX = (it.second->chunkX + chunkOffsetX) * 16;
                int32_t imageZ = (it.second->chunkZ + chunkOffsetZ) * 16;

                for (int32_t cz = 0; cz < 16; cz++) {
                    int32_t iz = (imageZ + cz);

                    for (int32_t cx = 0; cx < 16; cx++) {
                        int32_t ix = (imageX + cx);

                        if (!useCropFlag ||
                            ((ix >= cropX) && (ix < (cropX + cropW)) && (iz >= cropZ) && (iz < (cropZ + cropH)))) {

                            if (pchunk == nullptr || (pchunkX != it.second->chunkX) ||
                                (pchunkZ != it.second->chunkZ)) {
                                // get the chunk
                                // construct key
                                char keybuf[20];
                                int32_t keybuflen;
                                int32_t kx = it.second->chunkX, kz = it.second->chunkZ, kw = dimId;
                                //todohere todostopper - needs attention for 256h
                                uint8_t kt = 0x30;
                                switch (dimId) {
                                case kDimIdOverworld:
                                    //overworld
                                    memcpy(&keybuf[0], &kx, sizeof(int32_t));
                                    memcpy(&keybuf[4], &kz, sizeof(int32_t));
                                    memcpy(&keybuf[8], &kt, sizeof(uint8_t));
                                    keybuflen = 9;
                                    break;
                                default:
                                    // nether
                                    memcpy(&keybuf[0], &kx, sizeof(int32_t));
                                    memcpy(&keybuf[4], &kz, sizeof(int32_t));
                                    memcpy(&keybuf[8], &kw, sizeof(int32_t));
                                    memcpy(&keybuf[12], &kt, sizeof(uint8_t));
                                    keybuflen = 13;
                                    break;
                                }
                                leveldb::Slice key(keybuf, keybuflen);
                                leveldb::Status dstatus = db->Get(levelDbReadOptions, key, &svalue);
                                if (!dstatus.ok()) {
                                    slogger.msg(kLogInfo1, "WARNING: LevelDB operation returned status=%s\n",
                                        dstatus.ToString().c_str());
                                }
                                pchunk = svalue.data();
                                pchunkX = it.second->chunkX;
                                pchunkZ = it.second->chunkZ;
                            }

                            uint8_t blockid = getBlockId_LevelDB_v2(pchunk, cx, cz, cy);

                            if (blockid == 0 && (cy > it.second->topBlockY[cx][cz]) && (dimId != kDimIdNether)) {
                                // special handling for air -- keep existing value if we are above top block
                                // the idea is to show air underground, but hide it above so that the map is not all black pixels @ y=MAX_BLOCK_HEIGHT
                                // however, we do NOT do this for the nether. because: the nether
                            }
                            else {

                                if (blockInfoList[blockid].hasVariants()) {
                                    // we need to get blockdata
                                    int32_t blockdata = it.second->data[cx][cz];
                                    bool vfound = false;
                                    for (const auto& itbv : blockInfoList[blockid].variantList) {
                                        if (itbv->blockdata == blockdata) {
                                            vfound = true;
                                            color = itbv->color;
                                            break;
                                        }
                                    }
                                    if (!vfound) {
                                        BlockRecorder::instance().addUnknownBlockVariant(
                                            blockid,
                                            blockInfoList[blockid].name,
                                            blockdata
                                        );
                                        // since we did not find the variant, use the parent block's color
                                        color = blockInfoList[blockid].color;
                                    }
                                }
                                else {
                                    color = blockInfoList[blockid].color;
                                }

                                // do grid lines
                                if (checkDoForDim(control.doGrid) && (cx == 0 || cz == 0)) {
                                    if ((it.second->chunkX == 0) && (it.second->chunkZ == 0) && (cx == 0) &&
                                        (cz == 0)) {
                                        // highlight (0,0)
                                        color = htobe32(0xeb3333);
                                    }
                                    else {
                                        color = htobe32(0xc1ffc4);
                                    }
                                }

#ifdef PIXEL_COPY_MEMCPY
                                memcpy(&buf[(((imageZ + cz) - cropZ) * cropW + ((imageX + cx) - cropX)) * 3],
                                    &pcolor[1], 3);
#else
                                // todo - any use in optimizing the offset calc?
                                buf[(((imageZ + cz) - cropZ) * cropW + ((imageX + cx) - cropX)) * 3] = pcolor[1];
                                buf[(((imageZ + cz) - cropZ) * cropW + ((imageX + cx) - cropX)) * 3 + 1] = pcolor[2];
                                buf[(((imageZ + cz) - cropZ) * cropW + ((imageX + cx) - cropX)) * 3 + 2] = pcolor[3];
#endif
                            }
                        }
                    }
                }
            }

            // output the image
            std::string fnameTmp = fnBase + ".mcpe_viz_slice.";
            if (!makeMovieFlag) {
                fnameTmp += "full.";
            }
            fnameTmp += name;
            fnameTmp += ".";
            char xtmp[100];
            sprintf(xtmp, "%03d", cy);
            fnameTmp += xtmp;
            fnameTmp += ".png";

            control.fnLayerRaw[dimId][cy] = fnameTmp;

            outputPNG(fnameTmp, makeImageDescription(-1, cy), buf, cropW, cropH, false);
        }

        delete[] buf;

        if (makeMovieFlag) {
            // "ffmpeg" method
            std::string fnameTmp = fnBase + ".mcpe_viz_slice.";
            fnameTmp += name;
            fnameTmp += ".%03d.png";

            // todo - ffmpeg on win32? need bin path option?
            // todo - provide other user options for ffmpeg cmd line params?
            std::string cmdline = std::string("ffmpeg -y -framerate 1 -i " + fnameTmp + " -c:v libx264 -r 30 ");
            cmdline += fnOut;
            int32_t ret = system(cmdline.c_str());
            if (ret != 0) {
                slogger.msg(kLogInfo1, "Failed to create movie ret=(%d) cmd=(%s)\n", ret, cmdline.c_str());
            }

            // todo - delete temp slice files? cmdline option to NOT delete
        }

        return 0;
    }


    int32_t DimensionData_LevelDB::doOutput_GeoJSON()
    {
        // put spawnable info
        for (const auto& it : listCheckSpawn) {
            // spwawnable! add it to the list
            double ix, iy;
            char tmpstring[512];
            worldPointToGeoJSONPoint(dimId, it->x, it->z, ix, iy);
            sprintf(tmpstring, ""
                "\"Spawnable\":true,"
                "\"Name\":\"SpawnableBoundingCircle\","
                "\"BoundingCircle\":\"1\","
                "\"Clickable\":\"0\","
                "\"Dimension\":\"%d\","
                "\"Radius\":\"%d\","
                "\"Pos\":[%d,%d,%d]"
                "}}", dimId, it->distance, it->x, 0, it->z
            );
            std::string json = ""
                + makeGeojsonHeader(ix, iy)
                + tmpstring;
            listGeoJSON.push_back(json);
        }

        return 0;
    }

    int32_t DimensionData_LevelDB::doOutput_Schematic(leveldb::DB* db)
    {
        for (const auto& schematic : listSchematic) {
            int32_t sizex = schematic->x2 - schematic->x1 + 1;
            int32_t sizey = schematic->y2 - schematic->y1 + 1;
            int32_t sizez = schematic->z2 - schematic->z1 + 1;

            // std::vector<int8_t>
            nbt::tag_byte_array blockArray;
            nbt::tag_byte_array blockDataArray;

            char keybuf[128];
            int32_t keybuflen;
            int32_t kw = dimId;
            //todohere todostopper - needs attention for 256h
            uint8_t kt = 0x30;
            leveldb::Status dstatus;

            slogger.msg(kLogInfo1, "  Processing Schematic: %s\n", schematic->toString().c_str());

            std::string svalue;
            const char* pchunk = nullptr;

            //int32_t color;
            //const char *pcolor = (const char*)&color;


            int32_t foundCt = 0, notFoundCt2 = 0;
            uint8_t blockid, blockdata;

            int32_t prevChunkX = 0;
            int32_t prevChunkZ = 0;
            bool prevChunkValid = false;

            // todozzz - if schematic area is larger than one chunk (65k byte array limit), then create multiple chunk-sized schematic files and name then .schematic.11.22 (where 11=x_chunk & 22=z_chunk)

            for (int32_t imageY = schematic->y1; imageY <= schematic->y2; imageY++) {

                for (int32_t imageZ = schematic->z1; imageZ <= schematic->z2; imageZ++) {
                    int32_t chunkZ = imageZ / 16;
                    int32_t coz = imageZ % 16;

                    for (int32_t imageX = schematic->x1; imageX <= schematic->x2; imageX++) {
                        int32_t chunkX = imageX / 16;
                        int32_t cox = imageX % 16;

                        if (prevChunkValid && (chunkX == prevChunkX) && (chunkZ == prevChunkZ)) {
                            // we already have the chunk
                        }
                        else {
                            // we need to read the chunk

                            prevChunkValid = false;

                            // construct key to get the chunk
                            if (dimId == kDimIdOverworld) {
                                //overworld
                                memcpy(&keybuf[0], &chunkX, sizeof(int32_t));
                                memcpy(&keybuf[4], &chunkZ, sizeof(int32_t));
                                memcpy(&keybuf[8], &kt, sizeof(uint8_t));
                                keybuflen = 9;
                            }
                            else {
                                // nether (and probably any others that are added)
                                memcpy(&keybuf[0], &chunkX, sizeof(int32_t));
                                memcpy(&keybuf[4], &chunkZ, sizeof(int32_t));
                                memcpy(&keybuf[8], &kw, sizeof(int32_t));
                                memcpy(&keybuf[12], &kt, sizeof(uint8_t));
                                keybuflen = 13;
                            }

                            dstatus = db->Get(levelDbReadOptions, leveldb::Slice(keybuf, keybuflen), &svalue);
                            if (!dstatus.ok()) {
                                notFoundCt2++;
                                slogger.msg(kLogInfo1,
                                    "WARNING: Did not find chunk in leveldb x=%d z=%d status=%s\n", chunkX,
                                    chunkZ, dstatus.ToString().c_str());
                                blockArray.push_back(0);
                                blockDataArray.push_back(0);
                                continue;
                            }

                            pchunk = svalue.data();

                            prevChunkValid = true;
                            prevChunkX = chunkX;
                            prevChunkZ = chunkZ;
                            foundCt++;
                        }

                        blockid = getBlockId_LevelDB_v2(pchunk, cox, coz, imageY);
                        blockdata = getBlockData_LevelDB_v2(pchunk, cox, coz, imageY);

                        blockArray.push_back(blockid);
                        blockDataArray.push_back(blockdata);
                    }
                }
            }

            std::string fnOut = control.fnOutputBase + ".schematic." + schematic->fn + ".nbt";

            writeSchematicFile(fnOut, sizex, sizey, sizez, blockArray, blockDataArray);

            //slogger.msg(kLogInfo1,"    Chunk Info: Found = %d / Not Found (our list) = %d / Not Found (leveldb) = %d\n", foundCt, notFoundCt1, notFoundCt2);
        }
        return 0;
    }

    int32_t DimensionData_LevelDB::doOutput(leveldb::DB* db)
    {
        slogger.msg(kLogInfo1, "Do Output: %s\n", name.c_str());

        doOutputStats();

        doOutput_GeoJSON();

        // we put images in subdir
        std::string fnBase = mybasename(control.fnOutputBase);
        std::string dirOut = mydirname(control.fnOutputBase) + "/images";
        local_mkdir(dirOut.c_str());

        slogger.msg(kLogInfo1, "  Generate Image\n");
        control.fnLayerTop[dimId] = std::string(dirOut + "/" + fnBase + "." + name + ".map.png");
        generateImage(control.fnLayerTop[dimId], kImageModeTerrain);

        if (checkDoForDim(control.doImageBiome)) {
            slogger.msg(kLogInfo1, "  Generate Biome Image\n");
            control.fnLayerBiome[dimId] = std::string(dirOut + "/" + fnBase + "." + name + ".biome.png");
            generateImage(control.fnLayerBiome[dimId], kImageModeBiome);
        }
        if (checkDoForDim(control.doImageGrass)) {
            slogger.msg(kLogInfo1, "  Generate Grass Image\n");
            control.fnLayerGrass[dimId] = std::string(dirOut + "/" + fnBase + "." + name + ".grass.png");
            generateImage(control.fnLayerGrass[dimId], kImageModeGrass);
        }
        if (checkDoForDim(control.doImageHeightCol)) {
            slogger.msg(kLogInfo1, "  Generate Height Column Image\n");
            control.fnLayerHeight[dimId] = std::string(dirOut + "/" + fnBase + "." + name + ".height_col.png");
            generateImage(control.fnLayerHeight[dimId], kImageModeHeightCol);
        }
        if (checkDoForDim(control.doImageHeightColGrayscale)) {
            slogger.msg(kLogInfo1, "  Generate Height Column (grayscale) Image\n");
            control.fnLayerHeightGrayscale[dimId] = std::string(
                dirOut + "/" + fnBase + "." + name + ".height_col_grayscale.png");
            generateImage(control.fnLayerHeightGrayscale[dimId], kImageModeHeightColGrayscale);
        }
        if (checkDoForDim(control.doImageHeightColAlpha)) {
            slogger.msg(kLogInfo1, "  Generate Height Column (alpha) Image\n");
            control.fnLayerHeightAlpha[dimId] = std::string(
                dirOut + "/" + fnBase + "." + name + ".height_col_alpha.png");
            generateImage(control.fnLayerHeightAlpha[dimId], kImageModeHeightColAlpha);
        }
        if (checkDoForDim(control.doImageLightBlock)) {
            slogger.msg(kLogInfo1, "  Generate Block Light Image\n");
            control.fnLayerBlockLight[dimId] = std::string(dirOut + "/" + fnBase + "." + name + ".light_block.png");
            generateImage(control.fnLayerBlockLight[dimId], kImageModeBlockLight);
        }
        if (checkDoForDim(control.doImageLightSky)) {
            slogger.msg(kLogInfo1, "  Generate Sky Light Image\n");
            control.fnLayerSkyLight[dimId] = std::string(dirOut + "/" + fnBase + "." + name + ".light_sky.png");
            generateImage(control.fnLayerSkyLight[dimId], kImageModeSkyLight);
        }
        if (checkDoForDim(control.doImageSlimeChunks)) {
            slogger.msg(kLogInfo1, "  Generate Slime Chunks Image\n");
            control.fnLayerSlimeChunks[dimId] = std::string(
                dirOut + "/" + fnBase + "." + name + ".slime_chunks.png");
            generateImageSpecial(control.fnLayerSlimeChunks[dimId], kImageModeSlimeChunksMCPE);
        }

        if (checkDoForDim(control.doImageShadedRelief)) {
            slogger.msg(kLogInfo1, "  Generate Shaded Relief Image\n");
            control.fnLayerShadedRelief[dimId] = std::string(
                dirOut + "/" + fnBase + "." + name + ".shaded_relief.png");

            if (false) {
                // todobig - idea is to oversample the src image and then get higher resolution shaded relief - but, openlayers does not cooperate with this idea :) -- could fiddle with it later
                // todo - param for oversample
                std::string fnTemp = std::string(dirOut + "/" + fnBase + "." + name + ".shaded_relief.temp.png");
                if (oversampleImage(control.fnLayerHeightGrayscale[dimId], fnTemp, 2) == 0) {
                    generateShadedRelief(fnTemp, control.fnLayerShadedRelief[dimId]);
                    // remove temporary file
                    deleteFile(fnTemp);
                }
            }
            else {
                generateShadedRelief(control.fnLayerHeightGrayscale[dimId], control.fnLayerShadedRelief[dimId]);
            }
        }

        if (checkDoForDim(control.doMovie)) {
            slogger.msg(kLogInfo1, "  Generate movie\n");
            generateMovie(db, dirOut + "/" + fnBase, std::string(control.fnOutputBase + "." + name + ".mp4"), true,
                true);
        }

        if (checkDoForDim(control.doSlices)) {
            slogger.msg(kLogInfo1, "  Generate full-size slices\n");
            generateSlices(db, dirOut + "/" + fnBase);
        }

        doOutput_Schematic(db);

        // reset
        for (int32_t i = 0; i < 512; i++) {
            blockInfoList[i].colorSetNeedCount = 0;
        }

        return 0;
    }


}