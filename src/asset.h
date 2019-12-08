#ifndef __BEDROCK_VIZ_ASSET_H__
#define __BEDROCK_VIZ_ASSET_H__

#include <filesystem>

namespace bedrock_viz {
    std::filesystem::path get_asset_prefix();

    std::filesystem::path xml_path();
}

#endif //__BEDROCK_VIZ_ASSET_H__
