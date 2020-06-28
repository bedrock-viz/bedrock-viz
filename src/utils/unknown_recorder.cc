#include "unknown_recorder.h"
#include "../logger.h"

#include <utility>
#include <map>
#include <set>

namespace
{
    using VariantMap = std::map<std::pair<int32_t, int32_t>, std::string>;

    VariantMap sUnknownBlockVariants;
    VariantMap sUnknownItemVariants;
    VariantMap sUnknownEntityVariants;

    std::set<std::string> unknown_uname;

    std::set<int32_t> sUnknownBlockId;
    std::set<int32_t> sUnknownBiomeId;
    std::set<int32_t> sUnknownItemId;
    std::set<int32_t> sUnknownEntityId;
}

namespace mcpe_viz {

    void record_unknown_block_variant(int32_t blockId, const std::string& blockName, int32_t blockData)
    {
        using std::make_pair;

        if (sUnknownBlockVariants.find(make_pair(blockId, blockData)) == sUnknownBlockVariants.end()) {
            sUnknownBlockVariants.insert(make_pair(make_pair(blockId, blockData), blockName));
        }
    }

    void record_unknow_uname(const std::string& uname)
    {
        unknown_uname.insert(uname);
    }

    void record_unknown_block_id(int32_t id)
    {
        sUnknownBlockId.insert(id);
    }

    void record_unknown_biome_id(int32_t id)
    {
        sUnknownBiomeId.insert(id);
    }

    void record_unknown_item_id(int32_t itemId)
    {
        sUnknownItemId.insert(itemId);
    }

    void record_unknown_entity_id(int32_t entityId)
    {
        sUnknownEntityId.insert(entityId);
    }

    void record_unknown_item_variant(int32_t itemId, const std::string& itemName, int32_t blockData)
    {
        using std::make_pair;

        if (sUnknownItemVariants.find(make_pair(itemId, blockData)) == sUnknownItemVariants.end()) {
            sUnknownItemVariants.insert(make_pair(make_pair(itemId, blockData), itemName));
        }
    }

    void record_unknown_entity_variant(int32_t entityId, const std::string& entityName, int32_t extraData)
    {
        using std::make_pair;
        if (sUnknownEntityVariants.find(make_pair(entityId, extraData)) == sUnknownEntityVariants.end()) {
            sUnknownEntityVariants.insert(make_pair(make_pair(entityId, extraData), entityName));
        }
    }


    void print_unknown_warnings()
    {
        for (auto& i : unknown_uname) {
            log::warn("Unknown uname: {}", i);
        }
        for (auto& i : sUnknownBlockVariants) {
            const auto& blockId = i.first.first;
            const auto& blockData = i.first.second;
            const auto& blockName = i.second;

            log::warn("Unknown variant for block (id={} (0x{:x}) '{}') with blockdata={} (0x{:x})",
                      blockId, blockId, blockName, blockData, blockData);
        }
        for (auto& i: sUnknownItemVariants) {
            const auto& id = i.first.first;
            const auto& data = i.first.second;
            const auto& name = i.second;
            log::warn("Unknown item variant for item(id={} (0x{:x}) '{}') with extradata={} (0x{:x})",
                id, id, name, data, data);
        }
        for (auto& i : sUnknownEntityVariants) {
            const auto& id = i.first.first;
            const auto& data = i.first.second;
            const auto& name = i.second;
            log::warn("Unknown entity variant for item(id={} (0x{:x}) '{}') with extradata={} (0x{:x})",
                id, id, name, data, data);
        }
        for (auto& i : sUnknownBlockId) {
            log::warn("Unknown block id: {} (0x{:x})", i, i);
        }

        for (auto& i : sUnknownBiomeId) {
            log::warn("Unknown biome id: {} (0x{:x})", i, i);
        }

        for (auto& i: sUnknownItemId) {
            log::warn("Unknown item id: {} (0x{:x})", i, i);
        }

        for (auto& i: sUnknownEntityId) {
            log::warn("Unknown entity id: {} (0x{:x})", i, i);
        }
    }
}
