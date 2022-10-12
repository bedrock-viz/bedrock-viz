#include "minecraft/v2/block.h"
#include "config.h"
#include "util.h"
#include "define.h"

#include <unordered_map>
#include <vector>


namespace
{
    std::map<const mcpe_viz::WithId::IdType, const mcpe_viz::Block*> allBlocksById;
    std::unordered_map<std::string, mcpe_viz::Block*> unameBlockMap;
}

namespace mcpe_viz
{
    WithId::IdType Block::lastAssignedBlockId = mcpe_viz::kMaxLegacyBlockId + 1;

    void Block::addUname(const std::string& uname)
    {
        if (unameBlockMap.find(uname) != unameBlockMap.end()) {
            auto block = unameBlockMap[uname];
            log::error("uname({}) already exists (id=0x{:x} name={})",
                uname, block->id, block->name);
        }
        else {
            unameBlockMap[uname] = this;
        }
    }

    const Block* Block::get(const IdType& id)
    {
        try {
            auto blockPtr = allBlocksById.at(id);
            return blockPtr;
        } catch (std::out_of_range& outOfRange) {
            return nullptr;
        }
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
        if (id == UNKNOWN_ID) {
            id = lastAssignedBlockId++;
        }

        auto const block = new Block(id, name);

        if (allBlocksById[id] != nullptr) {
            log::error("Block id={}(0x{:x}) name={} already exists", id, id, name);
            return nullptr;
        }

        allBlocksById[id] = block;

        return block;
    }

    std::map<const WithId::IdType, const Block*> Block::all()
    {
        return allBlocksById;
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
            record_unknown_block_id(id);
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
            record_unknown_block_id(id);
            char buffer[256];
            sprintf(buffer, "(unknown-id-0x%02x)", id);
            return std::string(buffer);
        }
    }

}
