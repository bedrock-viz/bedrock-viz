#include "block.h"

#include <unordered_map>
#include <vector>
#include "config.h"
#include "../../util.h"
#include "../../utils/pointer_array.h"

namespace
{
    using mcpe_viz::PointerArray;
    using Wrapper = PointerArray<mcpe_viz::Block, mcpe_viz::kMaxBlockCount>;

    std::vector<mcpe_viz::Block*> sBlocks;
    std::unordered_map<std::string, mcpe_viz::Block*> unameBlockMap;
}

namespace mcpe_viz
{

    void Block::addUname(const std::string& uname)
    {
        unameBlockMap[uname] = this;
    }

    const Block* Block::get(IdType id)
    {
        auto& instance = Wrapper::value();
        return instance[id];
    }

    const Block* Block::getByUname(const std::string& uname)
    {
        std::string s = uname;
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);

        auto const iter = unameBlockMap.find(s);
        if (iter != unameBlockMap.end()) {
            return iter->second;
        }
        return nullptr;
    }

    Block* Block::add(IdType id, const std::string& name)
    {
        auto& instance = Wrapper::value();
        auto const block = new Block(id, name);
        instance[id] = block;
        sBlocks.emplace_back(block);
        return block;
    }

    const std::vector<Block*>& Block::list()
    {
        return sBlocks;
    }

    Block::Variant* Block::addVariant(Block::Variant::DataType data, const std::string& name)
    {

        if (this->variants_.find(data) != this->variants_.end()) {
            log::error("Variant(data={}, blockId={}, blockName={}) already exists", data, this->id, this->name);
            return nullptr;
        }
        auto const variant = new Variant(data, name);
        this->variants_[data] = variant;
        return variant;
    }

    std::string Block::queryName(const IdType& id, const Variant::DataType& data)
    {
        auto block = Block::get(id);
        if (block != nullptr) {
            return block->getVariantName(data);
        }
        else {
            record_unknow_id(id);
            char buffer[256];
            sprintf(buffer, "(Unknown-block-id-%d-data-%d)", id, data);
            return std::string(buffer);
        }
    }

    std::string Block::queryName(const IdType& id)
    {
        auto block = Block::get(id);
        if (block != nullptr) {
            return block->name;
        }
        else {
            record_unknow_id(id);
            char buffer[256];
            sprintf(buffer, "(unknown-id-0x%02x)", id);
            return std::string(buffer);
        }
    }

}
