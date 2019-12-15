#pragma once

#include <array>

#include "base.h"

namespace mcpe_viz
{
    const int kBiomeMaxSize = 256;

    class Biome: public BaseObject {
    public:

        Biome(const IdType& id, const std::string& name)
            : BaseObject{id, name}
        {
        }

        // TODO use iterator instead
        // for(auto& i: list()) { //... }
        static const std::array<Biome*, kBiomeMaxSize>& list();

        static Biome* add(const IdType& id, const std::string& name);
        static const Biome* get(const IdType& id);
        static const Biome* get(const std::string& name);

    };

}