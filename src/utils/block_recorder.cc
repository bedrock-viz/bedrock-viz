#include "block_recorder.h"
#include "../logger.h"

#include <utility>
#include <map>

namespace mcpe_viz {

    class BlockRecorder {
    public:
        using BlockVariantMap = std::map<std::pair<int32_t, int32_t>, std::string>;
    private:
        BlockVariantMap unknown_block_variant_;
        BlockRecorder() = default;
    public:

        void addUnknownBlockVariant(int32_t blockId, const std::string& blockName, int32_t blockData);

        const BlockVariantMap& getUnknownBlockVariant() const
        {
            return this->unknown_block_variant_;
        }

        static BlockRecorder& instance();
    };

    void BlockRecorder::addUnknownBlockVariant(int32_t blockId, const std::string& blockName, int32_t blockData)
    {
        using std::make_pair;

        if (this->unknown_block_variant_.find(make_pair(blockId, blockData)) == this->unknown_block_variant_.end()) {
            this->unknown_block_variant_.insert(make_pair(make_pair(blockId, blockData), blockName));
        }
    }

    BlockRecorder& BlockRecorder::instance()
    {
        static BlockRecorder instance_;
        return instance_;
    }

    void record_unknown_block_variant(int32_t blockId, const std::string& blockName, int32_t blockData)
    {
        auto& recorder = BlockRecorder::instance();
        recorder.addUnknownBlockVariant(blockId, blockName, blockData);
    }

    void print_unknown_block_warnings()
    {
        const auto& recorder = BlockRecorder::instance();
        for (auto& i : recorder.getUnknownBlockVariant()) {
            const auto& blockId = i.first.first;
            const auto& blockData = i.first.second;
            const auto& blockName = i.second;

            slogger.msg(kLogInfo1,
                "WARNING: Did not find block variant for block (id=%d (0x%x) '%s') with blockdata=%d (0x%x) MSG3\n",
                blockId, blockId,
                blockName.c_str(), blockData,
                blockData
            );
        }
    }
}
