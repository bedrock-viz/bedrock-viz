#pragma once

#include <map>
#include <cstdint>
#include <string>

namespace mcpe_viz {

    typedef std::map<std::string, int32_t> StringIntMap;

    extern StringIntMap imageFileMap;

    bool has_key(const StringIntMap &m, const std::string& k);
}
