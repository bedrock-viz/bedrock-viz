#include "unknown_recorder.h"
#include "../logger.h"

#include <utility>
#include <map>
#include <set>

namespace
{
    using BlockVariantMap = std::map<std::pair<int32_t, int32_t>, std::string>;

    BlockVariantMap unknown_block_variant;

    std::set<std::string> unknown_uname;

    std::set<int32_t> unknown_id;
}

namespace mcpe_viz {

    void record_unknown_block_variant(int32_t blockId, const std::string& blockName, int32_t blockData)
    {
        using std::make_pair;

        if (unknown_block_variant.find(make_pair(blockId, blockData)) == unknown_block_variant.end()) {
            unknown_block_variant.insert(make_pair(make_pair(blockId, blockData), blockName));
        }
    }

    void record_unknow_uname(const std::string& uname)
    {
        unknown_uname.insert(uname);
    }

    void record_unknow_id(int32_t id)
    {
        unknown_id.insert(id);
    }

    void print_unknown_warnings()
    {
        for (auto& i : unknown_uname) {
            log::warn("Unknown uname: {}", i);
        }
        for (auto& i : unknown_block_variant) {
            const auto& blockId = i.first.first;
            const auto& blockData = i.first.second;
            const auto& blockName = i.second;

            log::warn("Unknown block variant for block (id={} (0x{:x}) '{}') with blockdata={} (0x{:x})",
                      blockId, blockId, blockName, blockData, blockData);
        }
        for (auto& i : unknown_id) {
            log::warn("Unknown id: {} (0x{:x})", i, i);
        }
    }
}
