#include "check_spawn.h"

namespace mcpe_viz {

    CheckSpawn::CheckSpawn(int32_t nx, int32_t nz, int32_t ndist) {
        x = nx;
        z = nz;
        distance = ndist;
    }

    bool CheckSpawn::contains(int32_t tx, int32_t tz)
    {
        // todo - faster distance calc? do simple distance tests before sqrt part?
        double dx = x - tx;
        double dz = z - tz;
        double d = sqrt(dx * dx + dz * dz);
        if (d <= distance) {
            return true;
        }
        return false;
    }
}