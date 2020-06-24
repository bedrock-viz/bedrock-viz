#pragma once

#include <vector>
#include <string>

namespace mcpe_viz {
    enum HelpFlags : char {
        Basic        = 0x00,
        Extended     = 0x01,
        Experimental = 0x02
    };


    //todozooz - MAX_BLOCK_ID MAX_ITEM_ID etc
    // todo ugly globals
    extern std::vector<std::string> listGeoJSON;

    extern int32_t globalIconImageId;

    extern double playerPositionImageX, playerPositionImageY;
    extern int32_t playerPositionDimensionId;
}
