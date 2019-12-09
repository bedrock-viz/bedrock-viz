#pragma once

#include <string>
#include <cstdint>
#include <map>
#include <memory>

namespace mcpe_viz {
    class BiomeInfo {
    public:
        std::string name;
        int32_t color;
        bool colorSetFlag;

        BiomeInfo(const char* n);

        BiomeInfo(const char* n, int32_t rgb) {
            setName(n);
            setColor(rgb);
        }

        BiomeInfo& setName(const std::string& s) {
            name = std::string(s);
            return *this;
        }

        BiomeInfo& setColor(int32_t rgb);
    };

    typedef std::map<int, std::unique_ptr<BiomeInfo> > BiomeInfoList;

    extern BiomeInfoList biomeInfoList;

    bool has_key(const BiomeInfoList& m, int32_t k);

    std::string getBiomeName(int32_t idv);
}