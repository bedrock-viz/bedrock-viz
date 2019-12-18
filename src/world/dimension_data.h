#pragma once

#include <string>
#include <cstdint>
#include <utility>
#include <map>
#include <memory>

#include "chunk_data.h"
#include "../minecraft/schematic.h"
#include "../define.h"

// define this to use memcpy instead of manual copy of individual pixel values
 // memcpy appears to be approx 1.3% faster for another1 --html-all
#define PIXEL_COPY_MEMCPY

namespace mcpe_viz {
    class DimensionData_LevelDB {
    private:
        std::string name;
        int32_t dimId;

        typedef std::pair<uint32_t, uint32_t> ChunkKey;
        typedef std::map<ChunkKey, std::unique_ptr<ChunkData_LevelDB> > ChunkData_LevelDB_Map;
        ChunkData_LevelDB_Map chunks;

        int32_t minChunkX, maxChunkX;
        int32_t minChunkZ, maxChunkZ;
        bool chunkBoundsValid;

        bool fastBlockForceTopList[512];
        bool fastBlockHideList[512];
        bool fastBlockToGeoJSONList[512];

        // convenience vars from world object
        std::string worldName;
        int32_t worldSpawnX, worldSpawnZ;
        int64_t worldSeed;

        bool chunks_has_key(const ChunkData_LevelDB_Map& m, const ChunkKey& k) {
            return m.find(k) != m.end();
        }

    public:
        // todobig - move these to private?
        std::vector<int32_t> blockForceTopList;
        std::vector<int32_t> blockHideList;
        std::vector<int32_t> blockToGeoJSONList;

        SchematicList listSchematic;

        DimensionData_LevelDB() {
            name = "(UNKNOWN)";
            dimId = -1;
            chunkBoundsValid = false;
            minChunkX = 0;
            maxChunkX = 0;
            minChunkZ = 0;
            maxChunkZ = 0;
            worldName = "(UNKNOWN)";
            worldSpawnX = worldSpawnZ = 0;
            worldSeed = 0;
        }

        void setWorldInfo(const std::string& wName, int32_t wSpawnX, int32_t wSpawnZ, int64_t wSeed) {
            worldName = wName;
            worldSpawnX = wSpawnX;
            worldSpawnZ = wSpawnZ;
            worldSeed = wSeed;
        }

        void updateFastLists();

        void setName(const std::string& s) {
            name = s;
        }

        const std::string& getName() const {
            return name;
        }

        void setDimId(int32_t id) { dimId = id; }

        void unsetChunkBoundsValid() {
            minChunkX = minChunkZ = maxChunkX = maxChunkZ = 0;
            chunkBoundsValid = false;
        }

        bool getChunkBoundsValid() {
            return chunkBoundsValid;
        }

        void setChunkBoundsValid() {
            chunkBoundsValid = true;
        }

        void reportChunkBounds() {
            const int32_t chunkW = (maxChunkX - minChunkX + 1);
            const int32_t chunkH = (maxChunkZ - minChunkZ + 1);
            const int32_t imageW = chunkW * 16;
            const int32_t imageH = chunkH * 16;
            log::info("  Bounds (chunk): DimId={} X=({} {}) Z=({} {})", 
                dimId, minChunkX, maxChunkX, minChunkZ, maxChunkZ);
            log::info("  Bounds (pixel): DimId={} X=({} {}) Z=({} {}) Image=({} {})", 
                dimId, minChunkX * 16, maxChunkX * 16, minChunkZ * 16, maxChunkZ * 16, imageW, imageH);
        }

        void addToChunkBounds(int32_t chunkX, int32_t chunkZ) {
            minChunkX = std::min(minChunkX, chunkX);
            maxChunkX = std::max(maxChunkX, chunkX);
            minChunkZ = std::min(minChunkZ, chunkZ);
            maxChunkZ = std::max(maxChunkZ, chunkZ);
        }

        int32_t getMinChunkX() { return minChunkX; }

        int32_t getMaxChunkX() { return maxChunkX; }

        int32_t getMinChunkZ() { return minChunkZ; }

        int32_t getMaxChunkZ() { return maxChunkZ; }

        int32_t addChunk(int32_t tchunkFormatVersion, int32_t chunkX, int32_t chunkY, int32_t chunkZ, const char* cdata,
            size_t cdata_size) {
            ChunkKey chunkKey(chunkX, chunkZ);
            switch (tchunkFormatVersion) {
            case 2:
                // pre-0.17
                chunks[chunkKey] = std::unique_ptr<ChunkData_LevelDB>(new ChunkData_LevelDB());
                return chunks[chunkKey]->_do_chunk_v2(chunkX, chunkZ, cdata, dimId, name,
                    fastBlockHideList, fastBlockForceTopList,
                    fastBlockToGeoJSONList);
            case 3:
                // 0.17 and later?
                // we need to process all sub-chunks, not just blindy add them

                if (!chunks_has_key(chunks, chunkKey)) {
                    chunks[chunkKey] = std::unique_ptr<ChunkData_LevelDB>(new ChunkData_LevelDB());
                }

                return chunks[chunkKey]->_do_chunk_v3(chunkX, chunkY, chunkZ, cdata, cdata_size, dimId, name,
                    fastBlockHideList, fastBlockForceTopList,
                    fastBlockToGeoJSONList);
            case 7:
                // 1.2.x betas?
                // we need to process all sub-chunks, not just blindy add them

                if (!chunks_has_key(chunks, chunkKey)) {
                    chunks[chunkKey] = std::unique_ptr<ChunkData_LevelDB>(new ChunkData_LevelDB());
                }

                return chunks[chunkKey]->_do_chunk_v7(chunkX, chunkY, chunkZ, cdata, cdata_size, dimId, name,
                    fastBlockHideList, fastBlockForceTopList,
                    fastBlockToGeoJSONList);
            }
            log::error("Unknown chunk format ({})", tchunkFormatVersion);
            return -1;
        }

        int32_t addChunkColumnData(int32_t tchunkFormatVersion, int32_t chunkX, int32_t chunkZ, const char* cdata,
            int32_t cdatalen) {
            switch (tchunkFormatVersion) {
            case 2:
                // pre-0.17
                // column data is in the main record
                break;
            case 3:
                // 0.17 and later?
                // we need to process all sub-chunks, not just blindy add them

                ChunkKey chunkKey(chunkX, chunkZ);
                if (!chunks_has_key(chunks, chunkKey)) {
                    chunks[chunkKey] = std::unique_ptr<ChunkData_LevelDB>(new ChunkData_LevelDB());
                }

                return chunks[chunkKey]->_do_chunk_biome_v3(chunkX, chunkZ, cdata, cdatalen);
            }
            log::error("Unknown chunk format ({})", tchunkFormatVersion);
            return -1;
        }

        //todolib - move this out?
        bool checkDoForDim(int32_t v) const;

        // todolib - move this out?
        int32_t addSchematic(int32_t x1, int32_t y1, int32_t z1,
            int32_t x2, int32_t y2, int32_t z2,
            const char* fnSchematic) {
            listSchematic.push_back(std::unique_ptr<Schematic>(new Schematic(x1, y1, z1, x2, y2, z2, fnSchematic)));
            return 0;
        }

        int32_t addSchematic(const Schematic& sc) {
            listSchematic.push_back(std::unique_ptr<Schematic>(new Schematic(sc)));
            return 0;
        }

        void worldPointToImagePoint(double wx, double wz, double& ix, double& iy, bool geoJsonFlag) {
            const int32_t chunkOffsetX = -minChunkX;
            const int32_t chunkOffsetZ = -minChunkZ;

            if (geoJsonFlag) {
                const int32_t chunkH = (maxChunkZ - minChunkZ + 1);
                const int32_t imageH = chunkH * 16;

                ix = wx + (chunkOffsetX * 16);
                // todobig - correct calc here?
                iy = (imageH - 1) - (wz + (chunkOffsetZ * 16));


                // todobig -- for geojson, image == world (with y coordinate negated)
                if (false) {
                    ix = wx;
                    iy = -wz;
                }

            }
            else {
                ix = wx + (chunkOffsetX * 16);
                iy = wz + (chunkOffsetZ * 16);
            }

            // adjust for nether
            /*
        if ( dimId == kDimIdNether ) {
        ix /= 8;
        iy /= 8;
        }
      */
        }

        std::string makeImageDescription(int32_t imageMode, int32_t layerNumber) {
            std::string ret = "MCPE Viz Image -- World=(" + worldName + ")";
            ret += " Dimension=(" + name + ")";
            ret += " Image=(";
            switch (imageMode) {
            case kImageModeTerrain:
                ret += "Overview Map";
                break;
            case kImageModeBiome:
                ret += "Biome Map";
                break;
            case kImageModeGrass:
                ret += "Grass Color Map";
                break;
            case kImageModeHeightCol:
                ret += "Top Block Height Map";
                break;
            case kImageModeHeightColGrayscale:
                ret += "Top Block Height Map (grayscale)";
                break;
            case kImageModeHeightColAlpha:
                ret += "Top Block Height Map (alpha)";
                break;
            case kImageModeShadedRelief:
                ret += "Shaded Relief";
                break;
            case kImageModeBlockLight:
                ret += "Top Block Light Map";
                break;
            case kImageModeSkyLight:
                ret += "Top Block Sky Light Map";
                break;
            case kImageModeSlimeChunksMCPC:
                ret += "Slime Chunks Map (MCPC)";
                break;
            case kImageModeSlimeChunksMCPE:
                ret += "Slime Chunks Map (MCPE)";
                break;
            case -1:
                // special marker for raw layers
            {
                char tmpstring[256];
                sprintf(tmpstring, "Raw Layer %d", layerNumber);
                ret += tmpstring;
            }
            break;
            default: {
                char tmpstring[256];
                sprintf(tmpstring, "UNKNOWN: ImageMode=%d", imageMode);
                ret += tmpstring;
            }
                   break;
            }
            ret += ")";
            return ret;
        }

        int32_t outputPNG(const std::string& fname, const std::string& imageDescription, uint8_t* buf, int32_t width,
            int32_t height, bool rgbaFlag) {
            PngWriter png;
            if (png.init(fname, imageDescription, width, height, height, rgbaFlag, true) != 0) {
                return -1;
            }
            int32_t bpp = 3;
            if (rgbaFlag) {
                bpp = 4;
            }
            for (int32_t y = 0; y < height; y++) {
                png.row_pointers[y] = &buf[y * width * bpp];
            }
            png_write_image(png.png, png.row_pointers);
            png.close();
            return 0;
        }


        // todohere - rename - we are in "row(s) at a time" mode here
        int32_t
            outputPNG_init(PngWriter& png, const std::string& fname, const std::string& imageDescription, int32_t width,
                int32_t height, bool rgbaFlag) {
            if (png.init(fname, imageDescription, width, height, height, rgbaFlag, false) != 0) {
                return -1;
            }
            return 0;
        }

        int32_t outputPNG_writeRow(PngWriter& png, uint8_t* buf) {
            png_write_row(png.png, buf);
            // todo - check err?
            return 0;
        }

        int32_t outputPNG_writeRows(PngWriter& png, uint8_t** rows, uint32_t nrows) {
            png_write_rows(png.png, rows, nrows);
            // todo - check err?
            return 0;
        }

        int32_t outputPNG_close(PngWriter& png) {
            png.close();
            // todo - check err?
            return 0;
        }


        int32_t generateImage(const std::string& fname, const ImageModeType imageMode);


        // adapted from: https://gist.github.com/protolambda/00b85bf34a75fd8176342b1ad28bfccc
        bool isSlimeChunk_MCPE(int32_t cX, int32_t cZ);


        int32_t generateImageSpecial(const std::string& fname, const ImageModeType imageMode) {
            const int32_t chunkW = (maxChunkX - minChunkX + 1);
            const int32_t chunkH = (maxChunkZ - minChunkZ + 1);
            const int32_t imageW = chunkW * 16;
            const int32_t imageH = chunkH * 16;

            JavaRandom rnd;
            int64_t rndseed;
            bool slimeChunkFlag = false;

            int32_t bpp = 3;
            bool rgbaFlag = false;
            if (imageMode == kImageModeSlimeChunksMCPC || imageMode == kImageModeSlimeChunksMCPE) {
                bpp = 4;
                rgbaFlag = true;
            }

            // note RGB pixels
            uint8_t* buf = new uint8_t[imageW * 16 * bpp];

            uint8_t* rows[16];
            for (int i = 0; i < 16; i++) {
                rows[i] = &buf[i * imageW * bpp];
            }

            PngWriter png;
            if (outputPNG_init(png, fname, makeImageDescription(imageMode, 0), imageW, imageH, rgbaFlag) != 0) {
                delete[] buf;
                return -1;
            }

            int32_t color;
            for (int32_t iz = 0, chunkZ = minChunkZ; iz < imageH; iz += 16, chunkZ++) {
                memset(buf, 0, imageW * 16 * bpp);
                for (int32_t ix = 0, chunkX = minChunkX; ix < imageW; ix += 16, chunkX++) {

                    if (0) {
                    }
                    else if (imageMode == kImageModeSlimeChunksMCPC) {
                        /*
              from: http://minecraft.gamepedia.com/Slime_chunk#Low_layers
              Random rnd = new Random(seed +
              (long) (xPosition * xPosition * 0x4c1906) +
              (long) (xPosition * 0x5ac0db) +
              (long) (zPosition * zPosition) * 0x4307a7L +
              (long) (zPosition * 0x5f24f) ^ 0x3ad8025f);
              return rnd.nextInt(10) == 0;
            */
                        rndseed =
                            (worldSeed +
                            (int64_t)(chunkX * chunkX * (int64_t)0x4c1906) +
                                (int64_t)(chunkX * (int64_t)0x5ac0db) +
                                (int64_t)(chunkZ * chunkZ * (int64_t)0x4307a7) +
                                (int64_t)(chunkZ * (int64_t)0x5f24f)
                                )
                            ^ 0x3ad8025f;
                        rnd.setSeed(rndseed);
                        slimeChunkFlag = (rnd.nextInt(10) == 0);

                        if (slimeChunkFlag) {
                            color = (0xff << 24) | (0xff << 8);
                        }
                        else {
                            color = 0;
                        }

                        for (int32_t sz = 0; sz < 16; sz++) {
                            for (int32_t sx = 0; sx < 16; sx++) {
                                memcpy(&buf[((sz)*imageW + (ix + sx)) * bpp], &color, bpp);
                            }
                        }
                    }
                    else if (imageMode == kImageModeSlimeChunksMCPE) {
                        slimeChunkFlag = isSlimeChunk_MCPE(chunkX, chunkZ);

                        if (slimeChunkFlag) {
                            color = (0xff << 24) | (0xff << 8);
                        }
                        else {
                            color = 0;
                        }

                        for (int32_t sz = 0; sz < 16; sz++) {
                            for (int32_t sx = 0; sx < 16; sx++) {
                                memcpy(&buf[((sz)*imageW + (ix + sx)) * bpp], &color, bpp);
                            }
                        }
                    }
                }
                outputPNG_writeRows(png, rows, 16);
            }

            // output the image
            outputPNG_close(png);

            delete[] buf;

            return 0;
        }

        // originally from: http://openlayers.org/en/v3.10.0/examples/shaded-relief.html
        // but that code is actually *quite* insane
        // rewritten based on:
        //   http://edndoc.esri.com/arcobjects/9.2/net/shared/geoprocessing/spatial_analyst_tools/how_hillshade_works.htm
        int32_t generateShadedRelief(const std::string& fnSrc, const std::string& fnDest) {

            //todobig - make these params
            double data_vert = 5;

            double data_sunEl = 45.0;
            double data_sunAz = 315;
            double data_resolution = 1;

            PngReader pngSrc;
            if (pngSrc.init(fnSrc) != 0) {
                log::error("Failed to open src png");
                return -1;
            }

            pngSrc.read_info();

            int32_t srcW = pngSrc.getWidth();
            int32_t srcH = pngSrc.getHeight();
            int32_t colorType = pngSrc.getColorType();
            int32_t bppSrc = 3;
            if (colorType == PNG_COLOR_TYPE_RGB_ALPHA) {
                bppSrc = 4;
            }
            int32_t srcStride = srcW * bppSrc;

            uint8_t* sbuf = new uint8_t[srcStride * 3];

            // todobig - pngwriter support for 8-bit images (don't need RGBA for this)
            int32_t bppDest = 4;

            int32_t destW = srcW;
            int32_t destH = srcH;
            uint8_t* buf = new uint8_t[destW * bppDest];

            PngWriter pngOut;
            if (outputPNG_init(pngOut, fnDest, makeImageDescription(kImageModeShadedRelief, 0), destW, destH, true) !=
                0) {
                delete[] buf;
                pngSrc.close();
                delete[] sbuf;
                return -1;
            }

            /*
        uint8_t lut[256];

        double vmax = 128.0 * 128.0;
        for (int32_t i=0; i < 128; i++) {
        // (log( 1.0 + (double)(128 - i)/4.0 ) / logmax) * 255;
        double ti = (128 + 32) - i;
        double v = ((double)(ti * ti) / vmax) * 255.0;
        if ( v > 230.0 ) { v = 230.0; }
        if ( v < 0.0 ) { v = 0.0; }
        lut[i] = v;
        }
      */

      // weird - mingw32 doesn't get M_PI? - copied from math.h
#ifndef M_PI
#define M_PI            3.14159265358979323846
#endif

            int32_t maxX = srcW - 1;
            int32_t maxY = srcH - 1;
            double twoPi = 2.0 * M_PI;
            double halfPi = M_PI / 2.0;

            // (2)  Zenith_deg = 90 - Altitude
            // (3)  Zenith_rad = Zenith_deg * pi / 180.0
            double zenithRad = (90.0 - data_sunEl) * M_PI / 180.0;

            // (4)  Azimuth_math = 360.0 - Azimuth + 90
            double azimuthMath = 360.0 - data_sunAz + 90.0;
            // (5)  if Azimth_math >= 360.0 : Azimuth_math = Azimuth_math - 360.0
            if (azimuthMath >= 360.0) {
                azimuthMath = azimuthMath - 360.0;
            }
            // (6)  Azimuth_rad = Azimuth_math *  pi / 180.0
            double azimuthRad = azimuthMath * M_PI / 180.0;

            double cosZenithRad = cos(zenithRad);
            double sinZenithRad = sin(zenithRad);

            // todo - since we need to multiply x2 to expand 0..127 to 0..255 we instead halve this (would be 8)
            double dp = data_resolution *
                4.0;  // data.resolution * 8; // todo - not totally sure about the use of resolution here

    // notes: negative values simply reverse the sun azimuth; the range of interesting values is fairly narrow - somewhere on (0.001..0.8)
            double zFactor = (data_vert / 10.0) - 0.075;

            int32_t x0, x2,
                //y0, y2,
                offset;
            double z0, z2,
                dzdx, dzdy,
                slopeRad, aspectRad, hillshade, fhillshade;


            // prime the src buffers (first two rows are src row 0, then src row 1)
            png_read_row(pngSrc.png, &sbuf[0], NULL);
            memcpy(&sbuf[srcStride], &sbuf[0], srcStride);
            png_read_row(pngSrc.png, &sbuf[srcStride * 2], NULL);

            uint8_t* srcbuf0 = &sbuf[0];
            uint8_t* srcbuf1 = &sbuf[srcStride];
            uint8_t* srcbuf2 = &sbuf[srcStride * 2];

            for (int32_t y1 = 0; y1 < srcH; y1++) {
                // y0 = (y1 == 0) ? 0 : (y1 - 1);
                // y2 = (y1 == maxY) ? maxY : (y1 + 1);

                if (y1 > 0) {
                    // todo - this is slower than it needs to be, we could be clever about where we put the new row and not have to move stuff
                    // move data up
                    memcpy(&sbuf[0], &sbuf[srcStride], srcStride);
                    memcpy(&sbuf[srcStride], &sbuf[srcStride * 2], srcStride);
                    if (y1 < maxY) {
                        // read new row
                        png_read_row(pngSrc.png, &sbuf[srcStride * 2], NULL);
                    }
                }

                // clear output buffer
                memset(buf, 0, destW * bppDest);

                for (int32_t x1 = 0; x1 < srcW; x1++) {
                    x0 = (x1 == 0) ? 0 : (x1 - 1);
                    x2 = (x1 == maxX) ? maxX : (x1 + 1);

                    // z0 = a + 2d + g
                    z0 =
                        srcbuf0[x0 * bppSrc] +
                        srcbuf1[x0 * bppSrc] * 2.0 +
                        srcbuf2[x0 * bppSrc];

                    // z2 = c + 2f + i
                    z2 =
                        srcbuf0[x2 * bppSrc] +
                        srcbuf1[x2 * bppSrc] * 2.0 +
                        srcbuf2[x2 * bppSrc];

                    // (7)  [dz/dx] = ((c + 2f + i) - (a + 2d + g)) / (8 * cellsize)
                    dzdx = (z2 - z0) / dp;


                    // z0 = a + 2b + c
                    z0 =
                        srcbuf0[x0 * bppSrc] +
                        srcbuf0[x1 * bppSrc] * 2.0 +
                        srcbuf0[x2 * bppSrc];

                    // z2 = g + 2h + i
                    z2 =
                        srcbuf2[x0 * bppSrc] +
                        srcbuf2[x1 * bppSrc] * 2.0 +
                        srcbuf2[x2 * bppSrc];

                    // (8)  [dz/dy] = ((g + 2h + i) - (a + 2b + c))  / (8 * cellsize)
                    dzdy = (z2 - z0) / dp;

                    // (9)  Slope_rad = ATAN (z_factor * sqrt ([dz/dx]2 + [dz/dy]2))
                    slopeRad = atan(zFactor * sqrt(dzdx * dzdx + dzdy * dzdy));

                    if (dzdx != 0.0) {
                        aspectRad = atan2(dzdy, -dzdx);

                        if (aspectRad < 0) {
                            aspectRad += twoPi;
                        }
                    }
                    else {
                        if (dzdy > 0.0) {
                            aspectRad = halfPi;
                        }
                        else if (dzdy < 0.0) {
                            aspectRad = twoPi - halfPi;
                        }
                        else {
                            // aspectRad is fine
                            aspectRad = 0.0; // todo - this is my guess; algo notes are ambiguous
                        }
                    }

                    // (1)  Hillshade = 255.0 * ((cos(Zenith_rad) * cos(Slope_rad)) +
                    //        (sin(Zenith_rad) * sin(Slope_rad) * cos(Azimuth_rad - Aspect_rad)))
                    // Note that if the calculation of Hillshade value is < 0, the cell value will be = 0.

                    // todo - worth doing a sin/cos LUT?
                    fhillshade = 255.0 * ((cosZenithRad * cos(slopeRad)) +
                        (sinZenithRad * sin(slopeRad) * cos(azimuthRad - aspectRad)));

                    if (fhillshade < 0.0) {
                        hillshade = 0;
                    }
                    else {
                        hillshade = round(fhillshade);
                    }

                    offset = (x1)*bppDest;
                    // rgb
                    buf[offset] =
                        buf[offset + 1] =
                        buf[offset + 2] = uint8_t(hillshade);

                    // alpha
                    // note: reduce the opacity for brighter parts; idea is to reduce haziness
                    // todo - adjust this -- we want BLACK to be opaque; WHITE to be transparent (or is it gray that should be transparent)
                    //buf[offset+3] = 255 - (hillshade / 2);
                    //buf[offset+3] = lut[ srcbuf1[x1 * bppSrc] ];
                    buf[offset + 3] = 255;
                }

                // output image data
                outputPNG_writeRow(pngOut, buf);

            }

            outputPNG_close(pngOut);

            delete[] buf;

            pngSrc.close();

            delete[] sbuf;

            return 0;
        }



        // a run on old code (generateMovie):
        // > time ./mcpe_viz --db another1/ --out test-all1/out1 --log log.tall1 --html-all
        // 2672.248u 5.461s 45:47.69 97.4% 0+0k 72+1424304io 0pf+0w
        // approx 45 minutes

        // a run on this new code (generateSlices):
        // 957.146u 4.463s 16:30.58 97.0%  0+0k 2536+1424304io 1pf+0w
        // approx 17 minutes

        // 823.212u 13.623s 14:23.86 96.8% 0+0k 2536+1423088io 1pf+0w
        // 830.614u 13.871s 14:52.42 94.6% 0+0k 2536+1423088io 1pf+0w

        // change memcpy to byte copy:
        // 828.757u 13.842s 14:29.31 96.9% 0+0k 2536+1422656io 1pf+0w

        // with -O3
        // 827.337u 13.430s 14:27.58 96.9% 0+0k 2568+1422656io 1pf+0w

        // with -O3 and png zlib set to 1 (default is 9) and filter set to NONE
        //   with zlib=9 == 405M    test-all2
        //   with zlib=1 == 508M    test-all2-zlib1/
        // 392.597u 13.598s 7:07.25 95.0%  0+0k 2568+1633896io 1pf+0w

        // as above, but disabled setting filter to NONE
        //   with zlib=1 == 572M    test-all2-zlib1/
        // 660.763u 13.799s 11:40.99 96.2% 0+0k 2568+1764624io 1pf+0w

        // 402.214u 13.937s 7:26.63 93.1%  0+0k 2568+1633896io 1pf+0w

        // without -O3, with zlib=1 and filter NONE
        // 404.518u 13.369s 7:25.09 93.8%  0+0k 2536+1633896io 1pf+0w

        // so we're at ~6.5x faster now

        // 2015.10.24:
        // 372.432u 13.435s 6:50.66 93.9%  0+0k 419456+1842944io 210pf+0w

        int32_t generateSlices(leveldb::DB* db, const std::string& fnBase);


        int32_t generateMovie(leveldb::DB* db, const std::string& fnBase, const std::string& fnOut, bool makeMovieFlag,
            bool useCropFlag);

        int32_t doOutput_Schematic(leveldb::DB* db);

        int32_t doOutput(leveldb::DB* db);
    };

}
