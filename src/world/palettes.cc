#include "palettes.h"

#include "../define.h"
#include "../util.h"

namespace mcpe_viz {
    // palettes
    int32_t palRedBlackGreen[256];

    void makePalettes()
    {
        // create red-green ramp; red to black and then black to green
        makeHslRamp(palRedBlackGreen, 0, 61, 0.0, 0.0, 0.9, 0.9, 0.8, 0.1);
        makeHslRamp(palRedBlackGreen, 63, MAX_BLOCK_HEIGHT, 0.4, 0.4, 0.9, 0.9, 0.1, 0.8);
        // force 62 (sea level) to gray
        palRedBlackGreen[62] = 0x303030;

        // fill 128..255 with purple (we should never see this color)
        for (int32_t i = (MAX_BLOCK_HEIGHT + 1); i < 256; i++) {
            palRedBlackGreen[i] = kColorDefault;
        }

        // convert palette
        for (int32_t i = 0; i < 256; i++) {
            palRedBlackGreen[i] = htobe32(palRedBlackGreen[i]);
        }
    }
}