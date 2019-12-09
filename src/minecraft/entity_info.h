#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace mcpe_viz {
    class EntityInfo {
    public:
        std::string name;
        std::vector<std::string> unameList;
        std::string etype;

        EntityInfo(const std::string &n, const std::string &un, const std::string &e)
        {
            setName(n);
            setUname(un);
            setEtype(e);
        }

        EntityInfo &setName(const std::string &s)
        {
            name = std::string(s);
            return *this;
        }

        EntityInfo &setUname(const std::string &s);

        EntityInfo &setEtype(const std::string &e)
        {
            etype = std::string(e);
            return *this;
        }
    };

    typedef std::map<int, std::unique_ptr<EntityInfo>> EntityInfoList;
    
    extern EntityInfoList entityInfoList;
    
    bool has_key(const EntityInfoList &m, int32_t k);

    int32_t findEntityByUname(const EntityInfoList &m, std::string &un);
}