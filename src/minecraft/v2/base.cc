#include "minecraft/v2/base.h"
#include "define.h"
#include "util.h"

namespace mcpe_viz
{
    void Colored::color(const ColorType& rgb)
    {
        this->color_ = local_htobe32(rgb);
        this->is_color_set_ = true;
    }

    Colored::Colored()
        : color_{local_htobe32(kColorDefault)}
        , is_color_set_{false}
    {
    }
}
