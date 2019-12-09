#pragma once

#include <cstdint>
#include <cmath>
#include <vector>
#include <memory>

namespace mcpe_viz {
    // todolib - better name for this
    class CheckSpawn {
    public:
        int32_t x, z, distance;

        CheckSpawn(int32_t nx, int32_t nz, int32_t ndist);

        bool contains(int32_t tx, int32_t tz);
    };

    typedef std::vector<std::unique_ptr<CheckSpawn>> CheckSpawnList;
}
