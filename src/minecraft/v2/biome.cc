#include "biome.h"

#include <map>

#include "../../logger.h"
#include "../../utils/pointer_array.h"

namespace
{
    std::map<std::string, mcpe_viz::Biome*> sNameBiomeMap;
}

namespace mcpe_viz
{
    using Wrapper = PointerArray<Biome, kBiomeMaxSize>;

    const std::array<Biome*, kBiomeMaxSize>& Biome::list()
    {
        return Wrapper::value();
    }

    Biome *Biome::add(const BaseObject::IdType &id, const std::string &name)
    {
        auto& inst = Wrapper::value();
        if (inst[id] != nullptr) {
            log::error("Biome {} (name={}) already exists",
                id, inst[id]->name);
            return nullptr;
        }

        auto biome = new Biome(id, name);
        inst[id] = biome;
        sNameBiomeMap[name] = biome;
        return biome;
    }

    const Biome *Biome::get(const BaseObject::IdType &id)
    {
        return Wrapper::value()[id];
    }

    const Biome *Biome::get(const std::string& name)
    {
        if (sNameBiomeMap.find(name) != sNameBiomeMap.end()) {
            return sNameBiomeMap[name];
        }
        else {
            return nullptr;
        }
    }
}