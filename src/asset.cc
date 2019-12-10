#include "config.h"
#include "asset.h"

namespace bedrock_viz {
    using std::filesystem::path;
// TODO Important: path(".") should be executable path instead of working directory.
#ifdef BEDROCK_VIZ_DEBUG
    const auto& prefix = path(".");
#else
#ifdef _MSC_VER
    const auto& prefix = path(".");
#else
    const auto& prefix = path(cmake_install_prefix) / "bedrock-viz";
#endif
#endif

    path xml_path() {
        return prefix / "data" / "mcpe_viz.xml";
    }

    path static_path(const std::string& filename)
    {
        return prefix / "static" / filename;
    }
}
