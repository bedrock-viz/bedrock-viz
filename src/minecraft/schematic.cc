#include "minecraft/schematic.h"

#include <cstdio>
#include <algorithm>

namespace mcpe_viz {
    Schematic::Schematic(int32_t tx1, int32_t ty1, int32_t tz1, int32_t tx2, int32_t ty2, int32_t tz2,
                         const char *fnSchematic)
    {
        // we order the coordinates here so it's easier later
        x1 = std::min(tx1, tx2);
        x2 = std::max(tx1, tx2);
        y1 = std::min(ty1, ty2);
        y2 = std::max(ty1, ty2);
        z1 = std::min(tz1, tz2);
        z2 = std::max(tz1, tz2);
        fn = fnSchematic;
    }

    std::string Schematic::toString()
    {
        char buffer[2048];
        sprintf(buffer, "p1=%d,%d,%d p2=%d,%d,%d fn=%s", x1, y1, z1, x2, y2, z2, fn.c_str()
        );
        return std::string(buffer);
    }
}
