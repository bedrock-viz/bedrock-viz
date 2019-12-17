#pragma once

#include <map>
#include <vector>

#include "base.h"

namespace mcpe_viz
{
    class Entity: public Named, public WithId{
    public:
        class Variant: public Named, public WithData {
        public:
            Variant(const DataType& data, const std::string& name)
                : Named{name}
                , WithData{data}
            {
            }
        };

        enum class EType {
            NA = 0,
            P = 1, // for passive
            H = 2, // for hostile
        };
    private:
        std::map<Variant::DataType, Variant*> variants_;
    public:
        Entity(const IdType& id, const std::string& name)
            : Named{name}
            , WithId{id}
        {
        }

        Entity(const Entity&) = delete;
        Entity(const Entity&&) = delete;
        Entity& operator=(const Entity&) = delete;
        Entity&& operator=(const Entity&&) = delete;

        ~Entity()
        {
            for (auto& i : this->variants_) delete i.second;
            this->variants_.clear();
        }

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

        [[nodiscard]]
        std::string getVariantName(const Variant::DataType& data) const
        {
            if (this->hasVariants()) {
                for (auto& i : this->variants_) {
                    if (i.first == data) {
                        return i.second->name;
                    }
                }
            }
            // TODO record when necessary
            // record_unknown_block_variant(this->id, this->name, data);
            return this->name;
        }

        [[nodiscard]]
        auto getVariants() const
        {
            return this->variants_;
        }

        static const Entity* get(IdType id);
        static const Entity* getByUname(const std::string& uname);
        static Entity* add(IdType id, const std::string& name);
        static const std::vector<const Entity*>& list();

        static std::string queryName(const IdType& id, const Variant::DataType& data);
    };
}
