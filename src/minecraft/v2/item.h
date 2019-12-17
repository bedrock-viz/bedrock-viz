#pragma once

#include "base.h"

namespace mcpe_viz
{
    class Item: NamedObject {
    public:
        explicit Item(std::string name)
            : NamedObject(std::move(name))
        {
        }
    };


}