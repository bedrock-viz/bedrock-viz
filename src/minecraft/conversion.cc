#include "conversion.h"

namespace mcpe_viz {
    IntIntMap mcpcToMcpeBlock;
    IntIntMap mcpeToMcpcBlock;
    IntIntMap mcpcToMcpeItem;
    IntIntMap mcpeToMcpcItem;

    StringIntMap imageFileMap;

    bool has_key(const IntIntMap &m, int32_t k) {
        return m.find(k) != m.end();
    }

    bool has_key(const StringIntMap &m, const std::string &k) {
        return m.find(k) != m.end();
    }
}