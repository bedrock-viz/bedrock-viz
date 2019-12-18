#include "enchantment.h"

#include <map>

#include "config.h"
#include "../../logger.h"
#include "../../utils/pointer_array.h"

namespace
{
    std::vector<mcpe_viz::Enchantment*> sEnchants;
}

namespace mcpe_viz
{
    using Wrapper = PointerArray<Enchantment, kMaxEnchantmentCount>;

    const std::vector<Enchantment*>& Enchantment::list()
    {
        return sEnchants;
    }

    Enchantment* Enchantment::add(const WithId::IdType& id, const std::string& name, const std::string& offName)
    {
        auto& inst = Wrapper::value();
        if (inst[id] != nullptr) {
            log::error("Enchantment {} (name={}) already exists", id, inst[id]->name);
            return nullptr;
        }
        auto enchant = new Enchantment(id, name, offName);
        inst[id] = enchant;
        sEnchants.emplace_back(enchant);
        return enchant;
    }

    const Enchantment* Enchantment::get(const WithId::IdType& id)
    {
        return Wrapper::value()[id];
    }
}
