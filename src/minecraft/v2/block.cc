#include "block.h"

#include <unordered_map>

#include "../../util.h"
#include "../../utils/pointer_array.h"

namespace
{
    using mcpe_viz::Block;
    using mcpe_viz::PointerArray;
    using mcpe_viz::kBlockMaxSize;
    using Wrapper = PointerArray<Block, kBlockMaxSize>;
    

    std::unordered_map<std::string, mcpe_viz::Block*> nameBlockMap;

    std::unordered_map<std::string, mcpe_viz::Block*> unameBlockMap;
}

namespace mcpe_viz {

    void Block::addUname(const std::string& uname)
    {
        unameBlockMap[uname] = this;
    }

    const Block* Block::getById(IdType id)
    {
        auto& instance = Wrapper::value();
        return instance[id];
    }

    const Block* Block::getByName(const std::string& name)
    {
        auto const iter = nameBlockMap.find(name);
        if (iter != nameBlockMap.end())
        {
            return iter->second;
        }
        return nullptr;
    }

    const Block* Block::getByUname(const std::string& uname)
    {
        auto const iter = unameBlockMap.find(uname);
        if (iter != unameBlockMap.end())
        {
            return iter->second;
        }
        return nullptr;
    }

    Block* Block::add(IdType id, const std::string& name)
    {
        auto& instance = Wrapper::value();
        auto const block = new Block(id, name);
        instance[id] = block;
        nameBlockMap[name] = block;
        return block;
    }

    const std::array<Block*, kBlockMaxSize>& Block::list()
    {
        auto& instance = Wrapper::value();
        return instance;
    }
    
}
