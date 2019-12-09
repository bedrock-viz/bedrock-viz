#include "entity_info.h"

#include <algorithm>

#include "../util.h"

namespace mcpe_viz {

    EntityInfoList entityInfoList;
    
    bool has_key(const EntityInfoList &m, int32_t k) {
        return m.find(k) != m.end();
    }


    int32_t findEntityByUname(const EntityInfoList &m, std::string &un) {
        // convert search key to lower case
        std::string uname = un;
        std::transform(uname.begin(), uname.end(), uname.begin(), ::tolower);
        for (const auto &it : m) {
            for (const auto &u : it.second->unameList) {
                if (u == uname) {
                    return it.first;
                }
            }
        }
        return -1;
    }

    EntityInfo& EntityInfo::setUname(const std::string &s)
    {
        unameList = mysplit(s, ';');
        return *this;
    }

}