#pragma once

#include <vector>

namespace mcpe_viz {
    // maximum build height -- as of MCPE 0.13 it is 127
    const int32_t MAX_BLOCK_HEIGHT_127 = 127;
    const int32_t MAX_BLOCK_HEIGHT = 255;

    const int32_t MAX_CUBIC_Y = (MAX_BLOCK_HEIGHT + 1) / 16;

    const int32_t NUM_BYTES_CHUNK_V3 = 10241;

    enum OutputType : int32_t {
        kDoOutputNone = -2,
        kDoOutputAll = -1
    };

    enum HeightMode : int32_t {
        kHeightModeTop = 0
    };

    // output image types
    enum ImageModeType : int32_t {
        kImageModeTerrain = 0,
        kImageModeBiome = 1,
        kImageModeGrass = 2,
        kImageModeHeightCol = 3,
        kImageModeHeightColGrayscale = 4,
        kImageModeBlockLight = 5,
        kImageModeSkyLight = 6,
        kImageModeSlimeChunksMCPC = 7,
        kImageModeHeightColAlpha = 8,
        kImageModeShadedRelief = 9,
        kImageModeSlimeChunksMCPE = 10
    };

    // dimensions
    enum DimensionType : int32_t {
        kDimIdOverworld = 0,
        kDimIdNether = 1,
        kDimIdTheEnd = 2,
        // todobig - this is very brittle - consider that add-ons may one day add custome dimensions
        kDimIdCount = 3
    };

	const std::vector<int> kDimIdAll{kDimIdOverworld, kDimIdNether, kDimIdTheEnd};

    const int32_t kColorDefault = 0xff00ff;
}
