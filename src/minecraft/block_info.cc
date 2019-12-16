#include "block_info.h"

#include "../define.h"
#include "../util.h"
#include "../utils/unknown_recorder.h"

namespace mcpe_viz {
    BlockInfo::BlockInfo()
    {
        id = -1;
        name = "(unknown)";
        unameList.clear();
        setColor(kColorDefault); // purple
        solidFlag = true;
        opaqueFlag = true;
        liquidFlag = false;
        spawnableFlag = true;
        colorSetFlag = false;
        colorSetNeedCount = 0;
        variantList.clear();
        valid = false;
    }

    BlockInfo& BlockInfo::setUname(const std::string& s) {
        unameList = mysplit(s, ';');
        // todonow - update a dictionary for unames?
        return *this;
    }

    BlockInfo& BlockInfo::setColor(int32_t rgb) {
        // note: we convert color storage to big endian so that we can memcpy when creating images
        color = local_htobe32(rgb);
        colorSetFlag = true;
        return *this;
    }

    BlockInfo blockInfoList[512];
}