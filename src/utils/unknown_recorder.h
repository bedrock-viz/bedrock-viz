#pragma once

#include <string>
#include <cstdint>

namespace mcpe_viz {

    

    void record_unknow_uname(const std::string& uname);

    void record_unknown_block_id(int32_t blockId);

    void record_unknown_biome_id(int32_t blockId);

    void record_unknown_item_id(int32_t itemId);

    void record_unknown_entity_id(int32_t entityId);

    void record_unknown_block_variant(int32_t blockId, const std::string& blockName, int32_t blockData);

    void record_unknown_item_variant(int32_t itemId, const std::string& itemName, int32_t extraData);

    void record_unknown_entity_variant(int32_t entityId, const std::string& entityName, int32_t extraData);

    void print_unknown_warnings();
}
