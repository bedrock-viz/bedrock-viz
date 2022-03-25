#pragma once

#include <string>
#include <filesystem>

#include "define.h"
#include "global.h"

namespace mcpe_viz {

    // all user options are stored here
    class Control {
    public:
        std::filesystem::path outputDir;
        std::string dirLeveldb;
        std::string fnCfg;
        std::string fnXml;

        std::filesystem::path fnHtml() const { return this->outputDir / "index.html"; }
        // new logger output file
        std::filesystem::path logFile() const { return this->outputDir / "bedrock_viz.log"; }
        std::filesystem::path fnJs() const { return this->outputDir / "output.js"; }
        std::filesystem::path fnGeoJSON() const { return this->outputDir / "output.geojson";  }

        // per-dimension filenames
        std::string fnLayerTop[kDimIdCount];
        std::string fnLayerBiome[kDimIdCount];
        std::string fnLayerHeight[kDimIdCount];
        std::string fnLayerHeightGrayscale[kDimIdCount];
        std::string fnLayerHeightAlpha[kDimIdCount];
        std::string fnLayerSlimeChunks[kDimIdCount];
        std::string fnLayerShadedRelief[kDimIdCount];
        std::string fnLayerRaw[kDimIdCount][(MAX_BLOCK_HEIGHT - MIN_BLOCK_HEIGHT) + 1];

        bool doDetailParseFlag;
		std::vector<int> doMovie;
		std::vector<int> doSlices;
		std::vector<int> doGrid;
		int doHtml;
		bool doTiles;
		std::vector<int> doImageBiome;
		std::vector<int> doImageHeightCol;
		std::vector<int> doImageHeightColGrayscale;
		std::vector<int> doImageHeightColAlpha;
		std::vector<int> doImageSlimeChunks;
		std::vector<int> doImageShadedRelief;
        int dimYBottom[kDimIdCount];
        int dimYTop[kDimIdCount];
        bool autoTileFlag;
        bool noForceGeoJSONFlag;
        bool shortRunFlag;
        bool verboseFlag;
        bool quietFlag;
        char helpFlags;
        bool tryDbRepair;

        std::vector<bool> limitX, limitZ;
        std::vector<int> limitXMin, limitXMax, limitZMin, limitZMax;
        int32_t movieX, movieY, movieW, movieH;

        int32_t heightMode;

        int32_t tileWidth;
        int32_t tileHeight;

        // this is the BloomFilterPolicy bits, set to 0 to disable filter
        int32_t leveldbFilter = 10;
        // this is the block_size used by leveldb
        int32_t leveldbBlockSize = 4096;

        Control() {
            init();
        }

        void init() {
            dirLeveldb = "";
            fnXml = "";
            outputDir = std::filesystem::path("output");
            doDetailParseFlag = false;

            doMovie = kDimIdNone;
            doSlices = kDimIdNone;
            doGrid = kDimIdNone;
            doHtml = 0;
            doTiles = true;
            doImageBiome = kDimIdNone;
            doImageHeightCol = kDimIdNone;
            doImageHeightColGrayscale = kDimIdNone;
            doImageHeightColAlpha = kDimIdNone;
            doImageSlimeChunks = kDimIdNone;
            doImageShadedRelief = kDimIdNone;
            noForceGeoJSONFlag = false;
 
            // todobig - reasonable default? strike a balance between speed/# of files
            tileWidth = 2048;
            tileHeight = 2048;

            shortRunFlag = false;
            verboseFlag = false;
            quietFlag = false;
            helpFlags = HelpFlags::Basic;
            tryDbRepair = false;
            movieX = movieY = movieW = movieH = 0;

            leveldbFilter = 10;
            leveldbBlockSize = 4096;

            // todo - cmdline option for this?
            heightMode = kHeightModeTop;

            // default top/bottom Y levels
            // Overworld
            dimYBottom[kDimIdOverworld] = -64;
            dimYTop[kDimIdOverworld] = 319;
            // Nether
            dimYBottom[kDimIdNether] = 0;
            dimYTop[kDimIdNether] = 127;
            // The End
            dimYBottom[kDimIdTheEnd] = 0;
            dimYTop[kDimIdTheEnd] = 127;

            limitX.resize(kDimIdCount);
            limitZ.resize(kDimIdCount);
            limitXMin.resize(kDimIdCount);
            limitXMax.resize(kDimIdCount);
            limitZMin.resize(kDimIdCount);
            limitZMax.resize(kDimIdCount);

            for (int32_t did = 0; did < kDimIdCount; did++) {
                fnLayerTop[did] = "";
                fnLayerBiome[did] = "";
                fnLayerHeight[did] = "";
                fnLayerHeightGrayscale[did] = "";
                fnLayerHeightAlpha[did] = "";
                fnLayerSlimeChunks[did] = "";
                fnLayerShadedRelief[did] = "";
                for (int32_t i = dimYBottom[did]; i <= dimYTop[did]; i++) {
                    fnLayerRaw[did][i - dimYBottom[did]] = "";
                }
            }
        }
    };

    extern Control control;
}
