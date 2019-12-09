#pragma once

#include <string>
#include <map>
#include <utility>

namespace mcpe_viz {
    class BlockRecorder {
    public:
        using BlockVariantMap = std::map<std::pair<int32_t, int32_t>, std::string>;
    private:
        BlockVariantMap unknown_block_variant_;
        BlockRecorder();
    public:

        void addUnknownBlockVariant(int32_t blockId, const std::string& blockName, int32_t blockData);

        const BlockVariantMap& getUnknownBlockVariant() const
        {
            return this->unknown_block_variant_;
        }

        static BlockRecorder& instance();
    };
}
