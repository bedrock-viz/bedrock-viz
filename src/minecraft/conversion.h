#pragma once

#include <map>
#include <cstdint>
#include <string>

namespace mcpe_viz {
    using IntIntMap = std::map<int32_t, int32_t>;

    extern IntIntMap mcpcToMcpeBlock;
    extern IntIntMap mcpeToMcpcBlock;
    extern IntIntMap mcpcToMcpeItem;
    extern IntIntMap mcpeToMcpcItem;

    bool has_key(const IntIntMap &m, int32_t k);

    typedef std::map<std::string, int32_t> StringIntMap;

    extern StringIntMap imageFileMap;

    bool has_key(const StringIntMap &m, const std::string& k);
}
