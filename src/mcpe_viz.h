/*
  Minecraft Pocket Edition (MCPE) World File Visualization & Reporting Tool
  (c) Plethora777, 2015.9.26

  GPL'ed code - see LICENSE

  Main classes for mcpe_viz
*/
#pragma once

#include <string>

namespace mcpe_viz {
    void worldPointToImagePoint(int32_t dimId, double wx, double wz, double& ix, double& iy, bool geoJsonFlag);
    void worldPointToGeoJSONPoint(int32_t dimId, double wx, double wz, double& ix, double& iy);
} // namespace mcpe_viz
