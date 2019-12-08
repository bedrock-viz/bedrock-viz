#include "config.h"
#include "asset.h"

namespace bedrock_viz {
    using std::filesystem::path;
    path get_asset_prefix() {
#ifdef BEDROCK_VIZ_DEBUG
        return path(".");
#else
    #ifdef _MSC_VER
        return path(".");
    #else
        return path(cmake_install_prefix) / "bedrock-viz";
    #endif
#endif
    }

    std::filesystem::path xml_path() {
        const auto prefix = get_asset_prefix();
        return prefix / "data" / "mcpe_viz.xml";
    }
}
