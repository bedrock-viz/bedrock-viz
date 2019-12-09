#include "enchantment_info.h"

namespace mcpe_viz {
    EnchantmentInfoList enchantmentInfoList;

    bool has_key(const EnchantmentInfoList& m, int32_t k) {
        return m.find(k) != m.end();
    }
}