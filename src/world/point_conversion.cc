#include "point_conversion.h"

#include "world.h"

namespace mcpe_viz {
    void worldPointToImagePoint(int32_t dimId, double wx, double wz, double& ix, double& iy, bool geoJsonFlag) {
        return world->worldPointToImagePoint(dimId, wx, wz, ix, iy, geoJsonFlag);
    }

    void worldPointToGeoJSONPoint(int32_t dimId, double wx, double wz, double& ix, double& iy) {
        worldPointToImagePoint(dimId, wx, wz, ix, iy, true);
    }
}