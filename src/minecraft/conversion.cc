#include "conversion.h"

namespace mcpe_viz {

    StringIntMap imageFileMap;

    bool has_key(const StringIntMap &m, const std::string &k) {
        return m.find(k) != m.end();
    }
}