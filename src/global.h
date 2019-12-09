#pragma once

#include <vector>
#include <string>

namespace mcpe_viz {

    //todozooz - MAX_BLOCK_ID MAX_ITEM_ID etc
    // todo ugly globals
    extern std::string dirExec;

    extern std::vector<std::string> listGeoJSON;

    extern int32_t globalIconImageId;

    extern double playerPositionImageX, playerPositionImageY;
    extern int32_t playerPositionDimensionId;
}
