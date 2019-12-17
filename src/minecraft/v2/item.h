#pragma once

#include "base.h"

namespace mcpe_viz
{
    class Item: public Named, public WithId {
    public:
        explicit Item(const IdType& id, std::string name)
            : Named(std::move(name))
            , WithId{id}
        {
        }

        bool hasVariants() const { return true; }
    };
}