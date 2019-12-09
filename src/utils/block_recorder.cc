#include "block_recorder.h"

namespace mcpe_viz {
    BlockRecorder::BlockRecorder()
    {

    }

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
}