#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

namespace mcpe_viz {
    class BlockInfo {
    public:
        int32_t id;
        std::string name;
        std::vector<std::string> unameList;
        int32_t color;
        bool colorSetFlag;
        bool solidFlag;
        bool opaqueFlag;
        bool liquidFlag;
        bool spawnableFlag;
        int32_t colorSetNeedCount;
        int32_t blockdata;
        std::vector< std::unique_ptr<BlockInfo> > variantList;
        bool valid;

        int32_t userVar1;
        std::string userString1;

        BlockInfo();

        BlockInfo& setId(int32_t i) {
            id = i;
            return *this;
        }

        BlockInfo& setName(const std::string& s) {
            name = std::string(s);
            valid = true;
            return *this;
        }

        BlockInfo& setUname(const std::string& s);

        bool isValid() { return valid; }

        void setUserVar1(int32_t v) { userVar1 = v; }
        void deltaUserVar1(int32_t d) { userVar1 += d; }
        int32_t getUserVar1() { return userVar1; }

        void setUserString1(const std::string& s) { userString1 = s; }
        std::string& getUserString1() { return userString1; }

        BlockInfo& setColor(int32_t rgb);

        BlockInfo& setSolidFlag(bool f) {
            solidFlag = f;
            return *this;
        }
        bool isSolid() { return solidFlag; }

        BlockInfo& setOpaqueFlag(bool f) {
            opaqueFlag = f;
            return *this;
        }
        bool isOpaque() { return opaqueFlag; }

        BlockInfo& setLiquidFlag(bool f) {
            liquidFlag = f;
            return *this;
        }
        bool isLiquid() { return liquidFlag; }

        BlockInfo& setSpawnableFlag(bool f) {
            spawnableFlag = f;
            return *this;
        }
        bool isSpawnable(int32_t bd) {
            if (hasVariants()) {
                for (const auto& itbv : variantList) {
                    if (itbv->blockdata == bd) {
                        return itbv->spawnableFlag;
                    }
                }
                fprintf(stderr, "WARNING: did not find bd=%d (0x%x) for block='%s'\n", bd, bd, name.c_str());
            }
            return spawnableFlag;
        }

        bool hasVariants() {
            return (variantList.size() > 0);
        }

        void setBlockData(int32_t bd) {
            blockdata = bd;
        }

        BlockInfo& addVariant(int32_t bd, const std::string& n) {
            std::unique_ptr<BlockInfo> bv(new BlockInfo());
            bv->setName(n);
            bv->setBlockData(bd);
            variantList.push_back(std::move(bv));
            return *(variantList.back());
        }

        std::string toString() {
            char tmpstring[1024];
            sprintf(tmpstring, "Block: name=%s color=0x%06x solid=%d opaque=%d liquid=%d spawnable=%d"
                , name.c_str()
                , color
                , (int)solidFlag
                , (int)opaqueFlag
                , (int)liquidFlag
                , (int)spawnableFlag
            );
            // todo variants?
            return std::string(tmpstring);
        }
    };

    extern BlockInfo blockInfoList[512];

    int32_t findIdByBlockName(std::string& uname);

    std::string getBlockName(int32_t id, int32_t blockdata);

    int32_t getBlockByUname(const std::string& uname, int32_t& blockId, int32_t& blockData);
}