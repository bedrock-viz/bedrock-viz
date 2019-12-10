#ifndef __BEDROCK_VIZ_ASSET_H__
#define __BEDROCK_VIZ_ASSET_H__

#include <filesystem>
#include <string>

namespace bedrock_viz {
    std::filesystem::path xml_path();

    std::filesystem::path static_path(const std::string& filename);
}

#endif //__BEDROCK_VIZ_ASSET_H__
