#include "world/dimension_data.h"
#include "control.h"
#include "utils/unknown_recorder.h"
#include "world/common.h"
#include "world/misc.h"
#include "world/point_conversion.h"
#include "global.h"
#include "nbt.h"
#include "utils/fs.h"
#include "minecraft/v2/biome.h"
#include "minecraft/v2/block.h"

#include <random>
#include <algorithm>

namespace
{
    bool vectorContains(const std::vector<int>& v, int32_t i)
    {
        for (const auto& iter : v) {
            if (iter == i) {
                return true;
            }
        }
        return false;
    }
}

namespace
{
    using mcpe_viz::MAX_BLOCK_HEIGHT;
    using mcpe_viz::MIN_BLOCK_HEIGHT;
    using mcpe_viz::kColorDefault;
    using mcpe_viz::local_htobe32;


    // note: super super old hsl2rgb code; origin unknown
    double _hue_to_rgb(double m1, double m2, double h) {
        while (h < 1.0) { h += 1.0; }
        while (h > 1.0) { h -= 1.0; }
        if ((h * 6.0) < 1.0) {
            return m1 + (m2 - m1) * h * 6.0;
        }
        if ((h * 2.0) < 1.0) {
            return m2;
        }
        if ((h * 3.0) < 2.0) {
            return m1 + (m2 - m1) * (2.0 / 3.0 - h) * 6.0;
        }
        return m1;
    }


    int32_t _clamp(int32_t v, int32_t minv, int32_t maxv) {
        if (v < minv) return minv;
        if (v > maxv) return maxv;
        return v;
    }


    int32_t hsl2rgb(double h, double s, double l, int32_t& r, int32_t& g, int32_t& b) {
        double m2;
        if (l <= 0.5) {
            m2 = l * (s + 1.0);
        }
        else {
            m2 = l + s - l * s;
        }
        double m1 = l * 2.0 - m2;
        double tr = _hue_to_rgb(m1, m2, h + 1.0 / 3.0);
        double tg = _hue_to_rgb(m1, m2, h);
        double tb = _hue_to_rgb(m1, m2, h - 1.0 / 3.0);
        r = _clamp((int)(tr * 255.0), 0, 255);
        g = _clamp((int)(tg * 255.0), 0, 255);
        b = _clamp((int)(tb * 255.0), 0, 255);
        return 0;
    }


    int32_t makeHslRamp(int32_t* pal, int32_t start, int32_t stop, double h1, double h2, double s1, double s2, double l1, double l2) {
        double steps = stop - start + 1;
        double dh = (h2 - h1) / steps;
        double ds = (s2 - s1) / steps;
        double dl = (l2 - l1) / steps;
        double h = h1, s = s1, l = l1;
        int32_t r, g, b;
        for (int32_t i = start; i <= stop; i++) {
            hsl2rgb(h, s, l, r, g, b);
            int32_t c = ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
            pal[i] = c;
            h += dh;
            s += ds;
            l += dl;
        }
        return 0;
    }


    struct Palette {
        Palette()
        {
            memset(this->value, 0, sizeof(this->value));
            // create red-green ramp; red to black and then black to green
            makeHslRamp(this->value, 0 - MIN_BLOCK_HEIGHT, 61 - MIN_BLOCK_HEIGHT, 0.0, 0.0, 0.9, 0.9, 0.8, 0.1);
            makeHslRamp(this->value, 63 - MIN_BLOCK_HEIGHT, MAX_BLOCK_HEIGHT - MIN_BLOCK_HEIGHT, 0.4, 0.4, 0.9, 0.9, 0.1, 0.8);
            // force 62 (sea level) to gray
            this->value[62 - MIN_BLOCK_HEIGHT] = 0x303030;

            // fill 128..255 with purple (we should never see this color)
            for (int32_t i = (MAX_BLOCK_HEIGHT - MIN_BLOCK_HEIGHT + 1); i < 384; i++) {
                this->value[i] = kColorDefault;
            }

            // convert palette
            for (int32_t i = 0; i < 384; i++) {
                this->value[i] = local_htobe32(this->value[i]);
            }
        }
        int32_t value[384];
    };

    Palette& get_palette()
    {
        static Palette instance;
        return instance;
    }
}

namespace mcpe_viz {
    void DimensionData_LevelDB::updateFastLists()
    {
        for (int32_t bid = 0; bid < 1024; bid++) {
            fastBlockHideList[bid] = vectorContains(blockHideList, bid);
            fastBlockForceTopList[bid] = vectorContains(blockForceTopList, bid);
            fastBlockToGeoJSONList[bid] = vectorContains(blockToGeoJSONList, bid);
        }
    }

    bool DimensionData_LevelDB::checkDoForDim(const std::vector<int>& v) const
    {
        if (std::find(v.begin(), v.end(), dimId) != v.end()){
            return true;
        }
        return false;
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
        uint8_t lut[384];

        if (imageMode == kImageModeHeightColAlpha) {
            bpp = 4;
            rgbaFlag = true;
            // todobig - experiment with other ways to do this lut for height alpha
            double vmax = (double)MAX_BLOCK_HEIGHT * (double)MAX_BLOCK_HEIGHT;
            for (int32_t i = 0; i <= (MAX_BLOCK_HEIGHT - MIN_BLOCK_HEIGHT); i++) {
                // todobig make the offset (32) a cmdline param
                double ti = ((MAX_BLOCK_HEIGHT + 1) + 32) - i;
                double v = ((double)(ti * ti) / vmax) * 255.0;
                if (v > 235.0) { v = 235.0; }
                if (v < 0.0) { v = 0.0; }
                lut[i] = uint8_t(v);
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

                            auto biome = Biome::get(biomeId);
                            if (biome != nullptr) {
                                color = biome->color();
                            }
                            else {
                                log::trace("Unknown biome {} 0x{:x}", biomeId, biomeId);
                                record_unknown_biome_id(biomeId);
                                color = local_htobe32(0xff2020);
                            }
                        }
                        else if (imageMode == kImageModeGrass) {
                            // get grass color
                            int32_t grassColor = it->grassAndBiome[cx][cz] >> 8;
                            color = local_htobe32(grassColor);
                        }
                        else if (imageMode == kImageModeHeightCol) {
                            // get height value and use red-black-green palette
                            if (control.heightMode == kHeightModeTop) {
                                int16_t c = it->topBlockY[cx][cz];
                                //color = palRedBlackGreen[c];
                                color = get_palette().value[c - MIN_BLOCK_HEIGHT];
                            }
                            else {
                                int16_t c = it->heightCol[cx][cz];
                                color = get_palette().value[c - MIN_BLOCK_HEIGHT];
                            }
                        }
                        else if (imageMode == kImageModeHeightColGrayscale) {
                            // get height value and make it grayscale
                            if (control.heightMode == kHeightModeTop) {
                                uint8_t c = ((it->topBlockY[cx][cz] - MIN_BLOCK_HEIGHT) * 2) / 3;
                                color = (c << 24) | (c << 16) | (c << 8);
                            }
                            else {
                                uint8_t c = ((it->heightCol[cx][cz] - MIN_BLOCK_HEIGHT) * 2) / 3;
                                color = (c << 24) | (c << 16) | (c << 8);
                            }
                        }
                        else if (imageMode == kImageModeHeightColAlpha) {
                            // get height value and make it alpha
                            int16_t c;
                            if (control.heightMode == kHeightModeTop) {
                                c = it->topBlockY[cx][cz];
                            }
                            else {
                                c = it->heightCol[cx][cz];
                            }
                            // c = (90 - (int32_t)it->heightCol[cx][cz]) * 2;
                            c = lut[c - MIN_BLOCK_HEIGHT];
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
                            auto block = Block::get(blockid);
                            if (block != nullptr) {

                                if (block->hasVariants()) {
                                    // we need to get blockdata
                                    int32_t blockdata = it->data[cx][cz];
                                    auto variant = block->getVariantByBlockData(blockdata);
                                    if (variant != nullptr) {
                                        color = variant->color();
                                    }
                                    else {
                                        record_unknown_block_variant(
                                            blockid,
                                            block->name,
                                            blockdata);
                                        // since we did not find the variant, use the parent block's color
                                        color = block->color();
                                    }
                                }
                                else {
                                    color = block->color();
                                    if (!block->is_color_set()) {
                                        block->color_set_need_count += 1;
                                    }
                                }
                            }
                            else {
                                record_unknown_block_id(blockid);
                                color = kColorDefault;
                            }
                        }

                        // do grid lines
                        if (checkDoForDim(control.doGrid) && (cx == 0 || cz == 0)) {
                            if ((it->chunkX == 0) && (it->chunkZ == 0) && (cx == 0) && (cz == 0)) {
                                color = local_htobe32(0xeb3333);
                            }
                            else {
                                color = local_htobe32(0xc1ffc4);
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
                                log::info("    Info: World (0, 0) is at image ({}, {})", tix, tiz);
                            }
                            // todobig - just report this somwhere instead of having to pass the spawn params
                            if (twx == worldSpawnX && twz == worldSpawnZ) {
                                log::info("    Info: World Spawn ({}, {}) is at image ({}, {})",
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
            for(auto& i: Block::all()) {
                if (i.first != -1 && i.second->color_set_need_count != 0) {
                    log::info("    Need pixel color for: 0x{:x} '{}' (count={})",
                        i.second->id, i.second->name, i.second->color_set_need_count);
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
        random.seed(unsigned(seed));

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

        log::info("    Writing all images in one pass");

        std::string svalue;

        int32_t color;
        const char* pcolor = (const char*)&color;

        int16_t* emuchunk = new int16_t[NUM_BYTES_CHUNK_V3];

        // create png helpers
        int dimensionBottomY = control.dimYBottom[dimId];
        int dimensionTopY = control.dimYTop[dimId];
        PngWriter* png = new PngWriter[(dimensionTopY - dimensionBottomY + 1)];
        for (int32_t cy = dimensionBottomY; cy <= dimensionTopY; cy++) {
            std::string fnameTmp = fnBase + ".slice.full.";
            fnameTmp += name;
            fnameTmp += ".";
            sprintf(keybuf, "%03d", cy);
            fnameTmp += keybuf;
            fnameTmp += ".png";

            control.fnLayerRaw[dimId][cy - dimensionBottomY] = fnameTmp;

            if (png[cy - dimensionBottomY].init(fnameTmp, makeImageDescription(-1, cy), imageW, imageH, 16, false, true) != 0) {
                delete[] emuchunk;
                return -1;
            }
        }

        // create row buffers
        uint8_t** rbuf = new uint8_t*[(dimensionTopY - dimensionBottomY) + 1];
        for (int32_t cy = dimensionBottomY; cy <= dimensionTopY; cy++) {
            rbuf[cy - dimensionBottomY] = new uint8_t[(imageW * 3) * 16];
            // setup row pointers
            for (int32_t cz = 0; cz < 16; cz++) {
                png[cy - dimensionBottomY].row_pointers[cz] = &rbuf[cy - dimensionBottomY][(cz * imageW) * 3];
            }
        }

        // create a helper buffer which contains topBlockY for the entire image
        int16_t currTopBlockY = dimensionTopY;
        size_t bsize = (size_t)imageW * (size_t)imageH;
        int16_t* tbuf = new int16_t[bsize];
        std::fill(tbuf, tbuf+bsize, dimensionTopY);
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
                log::info("    Row {} of {}", imageZ, imageH);
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
                                    memcpy(&rbuf[cy - dimensionBottomY][((cz * imageW) + imageX + cx) * 3],
                                        &rbuf[currTopBlockY - dimensionBottomY][((cz * imageW) + imageX + cx) * 3],
                                        3);

                                }
                                else {
                                    auto block = Block::get(blockid);
                                    if (block != nullptr) {
                                        if (block->hasVariants()) {
                                            blockdata = getBlockData_LevelDB_v2(ochunk, cx, cz, cy);
                                            auto variant = block->getVariantByBlockData(blockdata);
                                            if (variant != nullptr) {
                                                color = variant->color();
                                            }
                                            else {
                                                record_unknown_block_variant(
                                                    block->id,
                                                    block->name,
                                                    blockdata);
                                                // since we did not find the variant, use the parent block's color
                                                color = block->color();
                                            }
                                        }
                                        else {
                                            color = block->color();
                                        }
                                    }
                                    else {
                                        record_unknown_block_id(blockid);
                                        color = kColorDefault;
                                    }

#ifdef PIXEL_COPY_MEMCPY
                                    memcpy(&rbuf[cy - dimensionBottomY][((cz * imageW) + imageX + cx) * 3], &pcolor[1], 3);
#else
                                    // todo - any use in optimizing the offset calc?
                                    rbuf[cy][((cz * imageW) + imageX + cx) * 3] = pcolor[1];
                                    rbuf[cy][((cz * imageW) + imageX + cx) * 3 + 1] = pcolor[2];
                                    rbuf[cy][((cz * imageW) + imageX + cx) * 3 + 2] = pcolor[3];
#endif
                                }
                            }

                            // to support 256h worlds, for v2 chunks, we need to make 128..255 the same as 127
                            // todo - could optimize this, is it needed with the top values?
                            for (int cy = 128; cy <= dimensionTopY; cy++) {
                                memcpy(&rbuf[cy - dimensionBottomY][((cz * imageW) + imageX + cx) * 3],
                                    &rbuf[127 - dimensionBottomY][((cz * imageW) + imageX + cx) * 3], 3);
                            }
                            // and I guess below zero too?
                            for (int cy = dimensionBottomY; cy < 0; cy++) {
                                memcpy(&rbuf[cy - dimensionBottomY][((cz * imageW) + imageX + cx) * 3],
                                       &rbuf[0 - dimensionBottomY][((cz * imageW) + imageX + cx) * 3], 3);
                            }

                        }
                    }
                }
                else {

                    // we did NOT find a pre-0.17 chunk...

                    // SECOND -- we try post 0.17 chunks

                    // we need to iterate over all possible y cubic chunks here...
                    int32_t cubicFoundCount = 0;
                    int32_t dimMinCubicY = dimensionBottomY / 16;
                    int32_t dimMaxCubicY = dimensionTopY / 16;
                    for (int32_t cubicY = dimMinCubicY; cubicY <= dimMaxCubicY; cubicY++) {
                        // todobug - this fails around level 112? on another1 -- weird -- run a valgrind to see where we're messing up
                        //check valgrind output

                        // construct key to get the chunk
                        if (dimId == kDimIdOverworld) {
                            //overworld
                            memcpy(&keybuf[0], &chunkX, sizeof(int32_t));
                            memcpy(&keybuf[4], &chunkZ, sizeof(int32_t));
                            memcpy(&keybuf[8], &kt_v3, sizeof(uint8_t));
                            memcpy(&keybuf[9], &cubicY, sizeof(uint8_t));
                            keybuflen = 10;
                        }
                        else {
                            // nether (and probably any others that are added)
                            memcpy(&keybuf[0], &chunkX, sizeof(int32_t));
                            memcpy(&keybuf[4], &chunkZ, sizeof(int32_t));
                            memcpy(&keybuf[8], &kw, sizeof(int32_t));
                            memcpy(&keybuf[12], &kt_v3, sizeof(uint8_t));
                            memcpy(&keybuf[13], &cubicY, sizeof(uint8_t));
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
                                        int32_t cy = cubicY * 16 + ccy;

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
                                            memcpy(&rbuf[cy - dimensionBottomY][((cz * imageW) + imageX + cx) * 3],
                                                &rbuf[currTopBlockY - dimensionBottomY][((cz * imageW) + imageX + cx) * 3],
                                                3);

                                        }
                                        else {
                                            // TODO not safe 
                                            if (blockid >= 0 && blockid < 1024) {
                                                auto block = Block::get(blockid);
                                                if (block != nullptr) {
                                                    if (block->hasVariants()) {
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
                                                        auto variant = block->getVariantByBlockData(blockdata);
                                                        if (variant != nullptr) {
                                                            color = variant->color();
                                                        }
                                                        else {
                                                            record_unknown_block_variant(
                                                                block->id,
                                                                block->name,
                                                                blockdata);
                                                            color = block->color();
                                                        }
                                                    }
                                                    else {
                                                        color = block->color();
                                                    }
                                                }
                                                else {
                                                    record_unknown_block_id(blockid);
                                                    color = kColorDefault;
                                                }
                                            }
                                            else {
                                                // bad blockid
                                                //todozooz todostopper - we get a lot of these w/ negative blockid around row 4800 of world 'another1'
                                                log::trace("Invalid blockid={} (image {} {}) (cc {} {} {})",
                                                    blockid, imageX, imageZ, cx, cz, cy);
                                                record_unknown_block_id(blockid);
                                                // set an unused color
                                                color = local_htobe32(0xf010d0);
                                            }

#ifdef PIXEL_COPY_MEMCPY
                                            memcpy(&rbuf[cy - dimensionBottomY][((cz * imageW) + imageX + cx) * 3], &pcolor[1], 3);
#else
                                            // todo - any use in optimizing the offset calc?
                                            rbuf[cy - MIN_BLOCK_HEIGHT][((cz * imageW) + imageX + cx) * 3] = pcolor[1];
                                            rbuf[cy - MIN_BLOCK_HEIGHT][((cz * imageW) + imageX + cx) * 3 + 1] = pcolor[2];
                                            rbuf[cy - MIN_BLOCK_HEIGHT][((cz * imageW) + imageX + cx) * 3 + 2] = pcolor[3];
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
                                        int32_t cy = cubicY * 16 + ccy;
                                        if ((cy > currTopBlockY) && (dimId != kDimIdNether)) {
                                            // special handling for air -- keep existing value if we are above top block
                                            // the idea is to show air underground, but hide it above so that the map is not all black pixels @ y=MAX_BLOCK_HEIGHT
                                            // however, we do NOT do this for the nether. because: the nether

                                            // we need to copy this pixel from another layer
                                            memcpy(&rbuf[cy - dimensionBottomY][((cz * imageW) + imageX + cx) * 3],
                                                &rbuf[currTopBlockY - dimensionBottomY][((cz * imageW) + imageX + cx) * 3],
                                                3);
                                        }
                                        else {
                                            if (cy <= dimensionTopY) {
                                                memset(&(rbuf[cy - dimensionBottomY])[((cz * imageW) + imageX + cx) * 3], 0, 3);
                                            } else {

                                            }
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
                        for (int32_t cy = dimensionBottomY; cy <= dimensionTopY; cy++) {
                            for (int32_t cz = 0; cz < 16; cz++) {
                                memset(&rbuf[cy - dimensionBottomY][((cz * imageW) + imageX) * 3], 0, 16 * 3);
                            }
                        }
                        // todonow - need this?
                        //continue;
                    }
                }

            }

            // put the png rows
            // todo - png lib is SLOW - worth it to alloc a larger window (16-row increments) and write in batches?
            // y here is already shifted by dimensionBottomY so we don't have to subtract it every loop
            for (int32_t y = 0; y <= dimensionTopY - dimensionBottomY; y++) {
                png_write_rows(png[y].png, png[y].row_pointers, 16);
            }
        }

        // y here is already shifted by dimensionBottomY so we don't have to subtract it every loop
        for (int32_t y = 0; y <= dimensionTopY - dimensionBottomY; y++) {
            delete[] rbuf[y];
            png[y].close();
        }

        delete[] tbuf;
        delete[] rbuf;
        delete[] png;
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
        for (int32_t cy = control.dimYBottom[dimId]; cy <= control.dimYTop[dimId]; cy++) {
            // todo - make this part a func so that user can ask for specific slices from the cmdline?
            log::info("  Layer {}", cy);
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
                                    log::warn("LevelDB operation returned status={}", dstatus.ToString());
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
                                auto block = Block::get(blockid);

                                if (block != nullptr) {
                                    if (block->hasVariants()) {
                                        int32_t blockdata = it.second->data[cx][cz];
                                        auto variant = block->getVariantByBlockData(blockdata);
                                        if (variant != nullptr) {
                                            color = variant->color();
                                        }
                                        else {
                                            color = block->color();
                                            record_unknown_block_variant(
                                                block->id,
                                                block->name,
                                                blockdata);
                                        }
                                    }
                                    else {
                                        color = block->color();
                                    }
                                }
                                else {
                                    record_unknown_block_id(blockid);
                                    color = kColorDefault;
                                }

                                // do grid lines
                                if (checkDoForDim(control.doGrid) && (cx == 0 || cz == 0)) {
                                    if ((it.second->chunkX == 0) && (it.second->chunkZ == 0) && (cx == 0) &&
                                        (cz == 0)) {
                                        // highlight (0,0)
                                        color = local_htobe32(0xeb3333);
                                    }
                                    else {
                                        color = local_htobe32(0xc1ffc4);
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

            control.fnLayerRaw[dimId][cy - control.dimYBottom[dimId]] = fnameTmp;

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
                log::error("Failed to create movie ret=({}) cmd=({})", ret, cmdline);
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

            log::info("  Processing Schematic: {}", schematic->toString());

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
                                log::warn("Did not find chunk in leveldb x={} z={} status={}",
                                    chunkX, chunkZ, dstatus.ToString());
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

            std::string fnOut = (control.outputDir / ("bedrock_viz.schematic." + schematic->fn + ".nbt")).generic_string();

            writeSchematicFile(fnOut, sizex, sizey, sizez, blockArray, blockDataArray);

            //slogger.msg(kLogInfo1,"    Chunk Info: Found = %d / Not Found (our list) = %d / Not Found (leveldb) = %d\n", foundCt, notFoundCt1, notFoundCt2);
        }
        return 0;
    }

    int32_t DimensionData_LevelDB::doOutput(leveldb::DB* db)
    {
        log::info("Do Output: {}", name);

        doOutput_GeoJSON();

        // we put images in subdir
        std::string fnBase = "bedrock_viz";
        std::string dirOut = (control.outputDir / "images").generic_string();
        local_mkdir(dirOut);

        log::info("  Generate Image");
        control.fnLayerTop[dimId] = std::string(dirOut + "/" + fnBase + "." + name + ".map.png");
        generateImage(control.fnLayerTop[dimId], kImageModeTerrain);

        if (checkDoForDim(control.doImageBiome)) {
            log::info("  Generate Biome Image");
            control.fnLayerBiome[dimId] = std::string(dirOut + "/" + fnBase + "." + name + ".biome.png");
            generateImage(control.fnLayerBiome[dimId], kImageModeBiome);
        }
        if (checkDoForDim(control.doImageHeightCol)) {
            log::info("  Generate Height Column Image");
            control.fnLayerHeight[dimId] = std::string(dirOut + "/" + fnBase + "." + name + ".height_col.png");
            generateImage(control.fnLayerHeight[dimId], kImageModeHeightCol);
        }
        if (checkDoForDim(control.doImageHeightColGrayscale)) {
            log::info("  Generate Height Column (grayscale) Image");
            control.fnLayerHeightGrayscale[dimId] = std::string(
                dirOut + "/" + fnBase + "." + name + ".height_col_grayscale.png");
            generateImage(control.fnLayerHeightGrayscale[dimId], kImageModeHeightColGrayscale);
        }
        if (checkDoForDim(control.doImageHeightColAlpha)) {
            log::info("  Generate Height Column (alpha) Image");
            control.fnLayerHeightAlpha[dimId] = std::string(
                dirOut + "/" + fnBase + "." + name + ".height_col_alpha.png");
            generateImage(control.fnLayerHeightAlpha[dimId], kImageModeHeightColAlpha);
        }
        if (checkDoForDim(control.doImageSlimeChunks)) {
            log::info("  Generate Slime Chunks Image");
            control.fnLayerSlimeChunks[dimId] = std::string(
                dirOut + "/" + fnBase + "." + name + ".slime_chunks.png");
            generateImageSpecial(control.fnLayerSlimeChunks[dimId], kImageModeSlimeChunksMCPE);
        }

        if (checkDoForDim(control.doImageShadedRelief)) {
            log::info("  Generate Shaded Relief Image");
            control.fnLayerShadedRelief[dimId] = std::string(
                dirOut + "/" + fnBase + "." + name + ".shaded_relief.png");
#if 0

            // todobig - idea is to oversample the src image and then get higher resolution shaded relief - but, openlayers does not cooperate with this idea :) -- could fiddle with it later
            // todo - param for oversample
            std::string fnTemp = std::string(dirOut + "/" + fnBase + "." + name + ".shaded_relief.temp.png");
            if (oversampleImage(control.fnLayerHeightGrayscale[dimId], fnTemp, 2) == 0) {
                generateShadedRelief(fnTemp, control.fnLayerShadedRelief[dimId]);
                // remove temporary file
                deleteFile(fnTemp);
            }

#else
            generateShadedRelief(control.fnLayerHeightGrayscale[dimId], control.fnLayerShadedRelief[dimId]);
#endif
        }

        if (checkDoForDim(control.doMovie)) {
            log::info("  Generate movie");
            const std::string movieName = (control.outputDir / ("bedrock_viz." + name + ".mp4")).generic_string();
            generateMovie(db, dirOut + "/" + fnBase, movieName, true, true);
        }

        if (checkDoForDim(control.doSlices)) {
            log::info("  Generate full-size slices");
            generateSlices(db, dirOut + "/" + fnBase);
        }

        doOutput_Schematic(db);

        // reset
        for(auto& i: Block::all()) {
            if (i.second != nullptr) {
                i.second->color_set_need_count = 0;
            }
        }

        return 0;
    }


}
