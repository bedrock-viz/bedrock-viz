/*
  Minecraft Pocket Edition (MCPE) World File Visualization & Reporting Tool
  (c) Plethora777, 2015.9.26

  GPL'ed code - see LICENSE

  NBT support for mcpe_viz
*/
#pragma once

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4661)
#endif
#include "nbt_tags.h"

#ifdef _MSC_VER
// Your function
#pragma warning( pop ) 
#endif
namespace mcpe_viz {

  // helper types for NBT
  typedef std::pair<std::string, std::unique_ptr<nbt::tag> > MyNbtTag;
  typedef std::vector< MyNbtTag > MyNbtTagList;


  std::string makeGeojsonHeader(double ix, double iy, bool adjustCoordFlag = true);
  
  int32_t parseNbt( const char* hdr, const char* buf, int32_t bufLen, MyNbtTagList& tagList );

  int32_t parseNbtQuiet( const char* buf, int32_t bufLen, int32_t numToRead, MyNbtTagList& tagList );
    
  int32_t parseNbt_entity(int32_t dimensionId, const std::string& dimName, MyNbtTagList &tagList,
                      bool playerLocalFlag, bool playerRemoteFlag, const std::string& playerType, const std::string& playerId);
    
  int32_t parseNbt_tileEntity(int32_t dimensionId, const std::string& dimName, MyNbtTagList &tagList);
    
  int32_t parseNbt_portals(MyNbtTagList &tagList);

  int32_t parseNbt_mVillages(MyNbtTagList &tagList);

  int32_t writeSchematicFile(const std::string& fn, int32_t sizex, int32_t sizey, int32_t sizez,
                             nbt::tag_byte_array& blockArray, nbt::tag_byte_array& blockDataArray);
  
} // namespace mcpe_viz
