#include "base.h"

#include <utility>

#include "../../define.h"
#include "../../util.h"

namespace mcpe_viz
{

    void BaseObjectNoId::color(const ColorType& rgb)
    {
        this->color_ = local_htobe32(rgb);
        this->is_color_set_ = true;
    }

    BaseObjectNoId::BaseObjectNoId(std::string name)
        : NamedObject(std::move(name))
        , color_{local_htobe32(kColorDefault)}
        , is_color_set_{false}
    {
    }
}