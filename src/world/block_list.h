#pragma once

#include <cstdint>
#include <set>
#include <vector>

#include "../define.h"

namespace mcpe_viz
{
    class BlockList {

        using DataType = bool[1024];
        using QuerySetType = std::set<int32_t>;

        std::vector<bool*> dataList;
        std::vector<QuerySetType*> querySetList;
    public:
        BlockList()
        {
            for(int i = 0; i < kDimIdCount; ++i) {
                this->dataList.push_back(new bool[1024]());
                this->querySetList.push_back(new QuerySetType());
            }
        }
        BlockList(BlockList&) = delete;
        BlockList& operator=(const BlockList&) = delete;

        ~BlockList()
        {
            for(auto& i : this->dataList) {
                delete[] i;
                i = nullptr;
            }
            for(auto& i: this->querySetList) {
                delete i;
                i = nullptr;
            }
        }


        void add(int32_t dimId, int32_t blockId)
        {
            while (dimId >= this->dataList.size()) {
                this->dataList.push_back(new bool[1024]());
                this->querySetList.push_back(new QuerySetType());
            }
            this->dataList[dimId][blockId] = true;
            this->querySetList[dimId]->insert(blockId);
        }

        [[nodiscard]]
        const std::set<int32_t>& set(int32_t dimId) const
        {
            return (*this->querySetList[dimId]);
        }

        [[nodiscard]]
        const bool* list(int32_t dimId) const
        {
            return this->dataList[dimId];
        }

        [[nodiscard]]
        bool has(int32_t dimId, int32_t blockId) const
        {
            return this->dataList[dimId][blockId];
        }

        static BlockList Hide, ForceTop, ToGeoJSON;
    };
}
