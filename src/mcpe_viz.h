/*
  Minecraft Pocket Edition (MCPE) World File Visualization & Reporting Tool
  (c) Plethora777, 2015.9.26

  GPL'ed code - see LICENSE

  Main classes for mcpe_viz
*/

#ifndef __MCPE_VIZ_H__
#define __MCPE_VIZ_H__

#include <map>

#include "define.h"
#include "logger.h"

namespace mcpe_viz {

  //todozooz - MAX_BLOCK_ID MAX_ITEM_ID etc

  // todo ugly globals
  extern std::string dirExec;
  //extern Logger logger;
  extern double playerPositionImageX, playerPositionImageY;
  extern int32_t playerPositionDimensionId;

  extern int32_t globalIconImageId;

  void worldPointToImagePoint(int32_t dimId, double wx, double wz, double &ix, double &iy, bool geoJsonFlag);
  void worldPointToGeoJSONPoint(int32_t dimId, double wx, double wz, double &ix, double &iy);






  class EnchantmentInfo {
  public:
    std::string name;
    std::string officialName;

    EnchantmentInfo(const char* n) {
      setName(n);
      officialName="";
    }

    EnchantmentInfo& setName (const std::string& s) {
      name = std::string(s);
      return *this;
    }

    EnchantmentInfo& setOfficialName (const std::string& s) {
      officialName = std::string(s);
      return *this;
    }
  };

  typedef std::map<int, std::unique_ptr<EnchantmentInfo> > EnchantmentInfoList;
  extern EnchantmentInfoList enchantmentInfoList;
  bool has_key(const EnchantmentInfoList &m, int32_t k);

  typedef std::map<int32_t, int32_t> IntIntMap;

  extern IntIntMap mcpcToMcpeBlock;
  extern IntIntMap mcpeToMcpcBlock;
  extern IntIntMap mcpcToMcpeItem;
  extern IntIntMap mcpeToMcpcItem;

  bool has_key(const IntIntMap &m, int32_t k);

  typedef std::map<std::string, int32_t> StringIntMap;

  extern StringIntMap imageFileMap;

  bool has_key(const StringIntMap &m, const std::string& k);
  
} // namespace mcpe_viz

#endif // __MCPE_VIZ_H__
