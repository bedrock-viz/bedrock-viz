#ifndef __BEDROCK_VIZ_ASSET_H__
#define __BEDROCK_VIZ_ASSET_H__

#include <filesystem>
#include <string>

namespace mcpe_viz {
    std::filesystem::path xml_path();

    std::filesystem::path static_path(const std::string& filename);

    void set_argv_0(char* s);
}

#endif //__BEDROCK_VIZ_ASSET_H__
