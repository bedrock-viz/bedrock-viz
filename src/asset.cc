#include "config.h"
#include "asset.h"


namespace mcpe_viz {
    using std::filesystem::path;



#ifdef BEDROCK_VIZ_DEBUG
    const auto use_argv_0 = true;
#else
#ifdef _MSC_VER
    const auto use_argv_0 = true;
#else
    const auto use_argv_0 = false;
#endif
#endif

    path argv_0{"."};

    const path& get_prefix()
    {
        static auto prefix = path(cmake_install_prefix) / "share" / "bedrock-viz";
        return use_argv_0 ? argv_0 : prefix;
    }

    path data_path(const std::string& filename)
    {
        return get_prefix() / "data" / filename;
    }

    path static_path(const std::string& filename)
    {
        return get_prefix() / "static" / filename;
    }

    void set_argv_0(char* s)
    {
        argv_0 = path(s).parent_path();
    }
}
