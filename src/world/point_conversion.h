#pragma once

#include <string>

namespace mcpe_viz {

    void worldPointToImagePoint(int32_t dimId, double wx, double wz, double& ix, double& iy, bool geoJsonFlag);
    
    void worldPointToGeoJSONPoint(int32_t dimId, double wx, double wz, double& ix, double& iy);
}