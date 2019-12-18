#pragma once

#include "base.h"

namespace mcpe_viz
{
    class Enchantment: public WithId, public Named {
    public:
        std::string official_name;

        Enchantment(const IdType& id, const std::string& name, std::string officialName)
            : WithId{id}
            , Named{name}
            , official_name{std::move(officialName)}
        {
        }

        static const std::vector<Enchantment*>& list();
        static Enchantment* add(const IdType& id, const std::string& name, const std::string& offName);
        static const Enchantment* get(const IdType& id);
    };
}
