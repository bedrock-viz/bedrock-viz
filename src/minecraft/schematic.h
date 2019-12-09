#pragma once

#include <string>

namespace mcpe_viz {

    // TODO better name for this
    class Schematic {
    public:
        int32_t x1, y1, z1;
        int32_t x2, y2, z2;
        std::string fn;

        Schematic(int32_t tx1, int32_t ty1, int32_t tz1,
                  int32_t tx2, int32_t ty2, int32_t tz2,
                  const char *fnSchematic);

        std::string toString();
    };
}
