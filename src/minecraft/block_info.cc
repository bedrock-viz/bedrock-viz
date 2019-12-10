#include "block_info.h"

#include "../define.h"
#include "../util.h"

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
        userVar1 = 0;
        userString1 = "";
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

    int32_t findIdByBlockName(std::string& un) {
        std::string uname = un;
        std::transform(uname.begin(), uname.end(), uname.begin(), ::tolower);
        for (const auto& it : blockInfoList) {
            for (const auto& u : it.unameList) {
                if (u == uname) {
                    return it.id;
                }
            }
        }
        return -1;
    }

    std::string getBlockName(int32_t id, int32_t blockdata) {
        if (blockInfoList[id].isValid()) {
            if (blockInfoList[id].hasVariants()) {
                for (const auto& iter : blockInfoList[id].variantList) {
                    if (iter->blockdata == blockdata) {
                        // found variant
                        return iter->name;
                    }
                }
                // warn about missing variant, but return parent's name
                slogger.msg(kLogWarning, "getBlockName failed to find variant id=%d (0x%x) blockdata=%d (0x%x)\n", id,
                    id, blockdata, blockdata);
                return blockInfoList[id].name;
            }
            else {
                return blockInfoList[id].name;
            }
        }

        slogger.msg(kLogWarning, "getBlockName failed to find id=%d (0x%x) blockdata=%d (0x%x)\n", id, id, blockdata,
            blockdata);
        char tmpstring[256];
        sprintf(tmpstring, "(Unknown-block-id-%d-data-%d)", id, blockdata);
        return std::string(tmpstring);
    }


    int32_t getBlockByUname(const std::string& un, int32_t& blockId, int32_t& blockData) {
        // convert search key to lower case
        std::string uname = un;
        std::transform(uname.begin(), uname.end(), uname.begin(), ::tolower);
        for (const auto& it : blockInfoList) {
            for (const auto& u : it.unameList) {
                if (u == uname) {
                    blockId = it.id;
                    blockData = 0;
                    return 0;
                }
            }

            for (const auto& itbv : it.variantList) {
                for (const auto& u : itbv->unameList) {
                    if (u == uname) {
                        blockId = it.id;
                        blockData = itbv->blockdata;
                        return 0;
                    }
                }
            }
        }

        // force to "air"
        blockId = 0;
        blockData = 0;
        slogger.msg(kLogWarning, "getBlockByUname failed to find uname=%s\n", uname.c_str());
        return -1;
    }
}