#pragma once

#include <vector>
#include <string>
#include <map>
#include <cstdint>
#include <memory>

namespace mcpe_viz {
    class ItemInfo {
    public:
        std::string name;
        std::vector<std::string> unameList;
        int32_t extraData;
        std::vector< std::unique_ptr<ItemInfo> > variantList;
        int32_t userVar1;
        std::string userString1;

        ItemInfo(const char* n, const char* un) {
            setName(n);
            setUname(un);
            extraData = 0;
            variantList.clear();
            userVar1 = 0;
            userString1 = "";
        }

        ItemInfo& setName(const std::string& s) {
            name = std::string(s);
            return *this;
        }

        ItemInfo& setUname(const std::string& un);

        void setUserVar1(int32_t v) { userVar1 = v; }
        void deltaUserVar1(int32_t d) { userVar1 += d; }
        int32_t getUserVar1() { return userVar1; }

        void setUserString1(const std::string& s) { userString1 = s; }
        std::string& getUserString1() { return userString1; }

        bool hasVariants() {
            return (variantList.size() > 0);
        }

        void setExtraData(int32_t ed) {
            extraData = ed;
        }

        ItemInfo& addVariant(int32_t ed, const std::string& n, const std::string& un) {
            std::unique_ptr<ItemInfo> iv(new ItemInfo(n.c_str(), un.c_str()));
            iv->setExtraData(ed);
            variantList.push_back(std::move(iv));
            return *(variantList.back());
        }
    };

    typedef std::map<int, std::unique_ptr<ItemInfo> > ItemInfoList;

    extern ItemInfoList itemInfoList;

    bool has_key(const ItemInfoList& m, int32_t k);

    std::string getItemName(int32_t id, int32_t extraData, bool nameBasedFlag);

    int32_t findIdByItemName(std::string& un);
}