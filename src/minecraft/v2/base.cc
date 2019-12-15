#include "base.h"

#include <utility>

#include "../../define.h"
#include "../../util.h"

namespace mcpe_viz
{

    void BaseObject::color(const ColorType& rgb)
    {
        this->color_ = local_htobe32(rgb);
        this->is_color_set_ = true;
    }

    BaseObject::BaseObject(const BaseObject::IdType& id, std::string  name)
        : color_{kColorDefault}
        , is_color_set_{false}
        , name{std::move(name)}
        , id{id}
    {
    }
}