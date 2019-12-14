#pragma once

#include <string>
#include <cstdint>

namespace mcpe_viz {

    void record_unknown_block_variant(int32_t blockId, const std::string& blockName, int32_t blockData);

    void record_unknow_uname(const std::string& uname);

    void print_unknown_uname();
    
    void print_unknown_block_warnings();
}
