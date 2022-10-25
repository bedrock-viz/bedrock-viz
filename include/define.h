#pragma once

#include <vector>
#include <string>

namespace mcpe_viz {
    // absolutely block limits. Note that every dimension has unique dynamic
    // limits in control.dimYBottom and control.dimYTop, which should generally
    // be what is used for loops. These are still useful for allocation of
    // worst case scenarios, but should generally not be used if you know what
    // dimension you are working with.
    const int32_t MAX_BLOCK_HEIGHT_127 = 127;
    const int32_t MAX_BLOCK_HEIGHT = 319;
    const int32_t MIN_BLOCK_HEIGHT = -64;

    /**
     * @deprecated - need to calculate these by dimension, and adjust for zero indexing
     * @see generateSlices::worldCubicY for example of how to do it. (the code just above it actually)
     */
    const int32_t MIN_CUBIC_Y = MIN_BLOCK_HEIGHT / 16;
    /**
     * @deprecated
     */
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
    const std::vector<int> kDimJustOverworld{kDimIdOverworld};
    const std::vector<std::string> kDimIdAllStrings {"0","1","2"};
    const std::string kDimIdAllStr = "0 1 2";
    const std::vector<std::string> kDimIdNames {"overworld","nether","the-end"};
    const std::vector<int> kDimIdNone{};

    const int32_t kColorDefault = 0xff00ff;

    const int32_t UNKNOWN_ID = -1;
}
