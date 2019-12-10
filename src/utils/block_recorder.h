#pragma once

#include <string>
#include <cstdint>

namespace mcpe_viz {

    void record_unknown_block_variant(int32_t blockId, const std::string& blockName, int32_t blockData);

    void print_unknown_block_warnings();
}
