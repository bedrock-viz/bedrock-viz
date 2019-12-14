#include "item_info.h"

#include "../util.h"

namespace mcpe_viz {
    ItemInfo& ItemInfo::setUname(const std::string& un) {
        unameList = mysplit(un, ';');
        // todonow - update a dictionary for unames?
        return *this;
    }

    bool has_key(const ItemInfoList& m, int32_t k) {
        return m.find(k) != m.end();
    }

    ItemInfoList itemInfoList;

    std::string getItemName(int32_t id, int32_t extraData, bool nameBasedFlag)
    {
        if (has_key(itemInfoList, id)) {
            if (itemInfoList[id]->hasVariants()) {
                for (const auto& iter : itemInfoList[id]->variantList) {
                    if (iter->extraData == extraData) {
                        // found variant
                        return iter->name;
                    }
                }
                // warn about missing variant, but return parent's name
                log::warn("getItemName failed to find variant id={} (0x{:x}) extradata={} (0x{:x}) nbf={}",
                    id, id, extraData, extraData, nameBasedFlag);
                return itemInfoList[id]->name;
            }
            else {
                return itemInfoList[id]->name;
            }
        }
        log::warn("getItemName failed to find id={} (0x{:x}) extradata={} (0x{:x}) nbf={}",
            id, id, extraData, extraData, nameBasedFlag);
        char tmpstring[256];
        sprintf(tmpstring, "(Unknown-item-id-%d-data-%d)", id, extraData);
        return std::string(tmpstring);
    }


    int32_t findIdByItemName(std::string& un) {
        std::string uname = un;
        std::transform(uname.begin(), uname.end(), uname.begin(), ::tolower);
        for (const auto& it : itemInfoList) {
            for (const auto& u : it.second->unameList) {
                if (u == uname) {
                    return it.first;
                }
            }
        }
        return -1;
    }
}