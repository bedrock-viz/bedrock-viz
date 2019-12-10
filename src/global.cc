#include "global.h"
#include "define.h"

namespace mcpe_viz {
    // list of geojson items
    std::vector<std::string> listGeoJSON;

    int32_t globalIconImageId = 1;

    // todobig -- would be nice for these to be in world class
    double playerPositionImageX = 0.0, playerPositionImageY = 0.0;
    int32_t playerPositionDimensionId = kDimIdOverworld;
}