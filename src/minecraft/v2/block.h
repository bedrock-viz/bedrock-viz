#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <map>
#include <cassert>

#include "base.h"

namespace mcpe_viz {

    const int kBlockMaxSize = 512;
    
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

        Block(IdType id, const std::string& name);
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
        

        IdType id;

        int color_set_need_count;

        bool solid;
        bool opaque;
        bool liquid;
        bool spawnable;


        void addUname(const std::string& uname);

        int addVariant(Variant::DataType data, const std::string& name)
        {
            if (this->variants_.find(data) != this->variants_.end()) {
                log::error("Variant(data={}, blockId={}, blockName={}) already exists",
                    data, this->id, this->name);
                return -1;
            }
            assert(this->variants_.find(data) == this->variants_.end());
            auto const variant = new Variant(data, name);
            this->variants_[data] = variant;
        }

        bool hasVariants() const { return !this->variants_.empty(); }

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

        static const std::array<Block*, kBlockMaxSize>& list();
    };

}