#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <map>
#include <cassert>

#include "base.h"

namespace mcpe_viz {
    class Block final: public BaseObject {
    public:
        using IdType = unsigned short;

        class Variant: public BaseObjectNoId {
        public:
            using DataType = unsigned short;
            DataType data;
            Variant(DataType data, const std::string& name)
                : BaseObjectNoId{name}, data{data}
            {
            }
        };

    private:
        std::map<Variant::DataType, Variant*> variants_;
    public:

        Block(IdType id, const std::string& name)
            : BaseObject{id, name}
            , color_set_need_count{0}
            , solid{false}
            , opaque{false}
            , liquid{false}
            , spawnable{false}
        {
        }
        Block(const Block&) = delete;
        Block(const Block&&) = delete;
        Block& operator=(const Block&) = delete;
        Block& operator=(const Block&&) = delete;

        virtual ~Block()
        {
            for(auto& i : this->variants_) {
                delete i.second;
            }
            this->variants_.clear();
        }

        int color_set_need_count;

        bool solid;
        bool opaque;
        bool liquid;
        bool spawnable;

        void addUname(const std::string& uname);
        Variant* addVariant(Variant::DataType data, const std::string& name);

        [[nodiscard]]
        bool hasVariants() const { return !this->variants_.empty(); }

        [[nodiscard]]
        const Variant* getVariantByBlockData(Variant::DataType data) const
        {
            const auto iter = this->variants_.find(data);
            if (iter == this->variants_.end()) {
                return nullptr;
            }
            return iter->second;
        }

        static const Block* getById(IdType id);
        static const Block* getByName(const std::string& name);
        static const Block* getByUname(const std::string& uname);
        static Block* add(IdType id, const std::string& name);
        static const std::vector<Block*>& list();
    };

}