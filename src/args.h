#ifndef __BEDROCK_VIZ_ARGS_H__
#define __BEDROCK_VIZ_ARGS_H__

#include <cstdint>
#include <string>
#include <cstdlib>
#include <vector>
#include <utility>

#include "define.h"
#include "logger.h"
#include "minecraft.h"

namespace mcpe_viz {

    struct DimIdBlockId {
        int32_t dimId;
        int32_t blockId;
    };

    class Args {
    public:
        std::string dirLeveldb;
        std::string fnOutputBase;
        std::string fnCfg;
        std::string fnXml;
        std::string fnLog;
        std::string fnGeoJSON;
        std::string fnHtml;
        std::string fnJs;

        // per-dimension filenames
        std::string fnLayerTop[kDimIdCount];
        std::string fnLayerBiome[kDimIdCount];
        std::string fnLayerHeight[kDimIdCount];
        std::string fnLayerHeightGrayscale[kDimIdCount];
        std::string fnLayerHeightAlpha[kDimIdCount];
        std::string fnLayerBlockLight[kDimIdCount];
        std::string fnLayerSkyLight[kDimIdCount];
        std::string fnLayerSlimeChunks[kDimIdCount];
        std::string fnLayerGrass[kDimIdCount];
        std::string fnLayerShadedRelief[kDimIdCount];
        std::string fnLayerRaw[kDimIdCount][MAX_BLOCK_HEIGHT + 1];

        bool doDetailParseFlag = false;
        int32_t doMovie = kDoOutputNone;
        int32_t doSlices = kDoOutputNone;
        int32_t doGrid = kDoOutputNone;
        int32_t doHtml = 0;
        int32_t doTiles = 0;
        int32_t doImageBiome = kDoOutputNone;
        int32_t doImageGrass = kDoOutputNone;
        int32_t doImageHeightCol = kDoOutputNone;
        int32_t doImageHeightColGrayscale = kDoOutputNone;
        int32_t doImageHeightColAlpha = kDoOutputNone;
        int32_t doImageLightBlock = kDoOutputNone;
        int32_t doImageLightSky = kDoOutputNone;
        int32_t doImageSlimeChunks = kDoOutputNone;
        int32_t doImageShadedRelief = kDoOutputNone;
        bool autoTileFlag = false;
        bool noForceGeoJSONFlag = false;
        bool shortRunFlag = false;
        bool colorTestFlag = false;
        bool verboseFlag = false;
        bool quietFlag = false;
        bool helpFlag = false;
        int32_t movieX = 0, movieY = 0, movieW = 0, movieH = 0;

        bool doFindImages = false;
        std::string dirFindImagesIn;
        std::string dirFindImagesOut;

        int32_t heightMode = kHeightModeTop;

        int32_t tileWidth = 1024;
        int32_t tileHeight = 1024;

        // this is the BloomFilterPolicy bits, set to 0 to disable filter
        int32_t leveldbFilter = 10;
        // this is the block_size used by leveldb
        int32_t leveldbBlockSize = 4096;

        std::vector<DimIdBlockId> hide_top;
        std::vector<DimIdBlockId> force_top;
        std::vector<DimIdBlockId> geojson_block;
        std::vector<std::pair<int32_t, Schematic>> schematics;

        bool valid = true;
    private:
        Args(int argc, char* argv[]);

        static Args* instance_;
    public:
        static void initInstance(int argc, char* argv[]);
        static void destroyInstance();

        static Args* instance();
    };

    void print_usage();
}

#endif //__BEDROCK_VIZ_ARGS_H__
