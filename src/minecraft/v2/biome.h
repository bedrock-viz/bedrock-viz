#pragma once
#include <vector>
#include "base.h"

namespace mcpe_viz
{
    class Biome: public BaseObject {
    public:
        Biome(const IdType& id, const std::string& name)
            : BaseObject{id, name}
        {
        }

        static const std::vector<Biome*>& list();
        static Biome* add(const IdType& id, const std::string& name);
        static const Biome* get(const IdType& id);
        static const Biome* get(const std::string& name);
    };
}