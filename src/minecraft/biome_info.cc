#include "biome_info.h"

#include "../define.h"
#include "../util.h"

namespace mcpe_viz {
    BiomeInfo::BiomeInfo(const char* n)
    {
        setName(n);
        setColor(kColorDefault);
        colorSetFlag = false;
    }

    BiomeInfo& BiomeInfo::setColor(int32_t rgb)
    {
        // note: we convert color storage to big endian so that we can memcpy when creating images
        color = local_htobe32(rgb);
        colorSetFlag = true;
        return *this;
    }

    BiomeInfoList biomeInfoList;

    bool has_key(const BiomeInfoList& m, int32_t k) {
        return m.find(k) != m.end();
    }

    std::string getBiomeName(int32_t idv) 
    {
        if (has_key(biomeInfoList, idv)) {
            return biomeInfoList[idv]->name;
        }
        char s[256];
        sprintf(s, "ERROR: Failed to find biome id (%d)", idv);
        return std::string(s);
    }
}