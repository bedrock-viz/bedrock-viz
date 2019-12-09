/*
  Minecraft Pocket Edition (MCPE) World File Visualization & Reporting Tool
  (c) Plethora777, 2015.9.26

  GPL'ed code - see LICENSE

  Main classes for mcpe_viz
*/
#pragma once

#include <map>

namespace mcpe_viz {

  //todozooz - MAX_BLOCK_ID MAX_ITEM_ID etc

  // todo ugly globals
  extern std::string dirExec;
  extern double playerPositionImageX, playerPositionImageY;
  extern int32_t playerPositionDimensionId;

  extern int32_t globalIconImageId;

  void worldPointToImagePoint(int32_t dimId, double wx, double wz, double &ix, double &iy, bool geoJsonFlag);
  void worldPointToGeoJSONPoint(int32_t dimId, double wx, double wz, double &ix, double &iy);

} // namespace mcpe_viz
