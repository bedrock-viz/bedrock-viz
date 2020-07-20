#pragma once

#include "base.h"

#include <map>
#include <vector>

namespace mcpe_viz
{
    class Item: public Named, public WithId {
    public:

        class Variant: public Named, public WithData {
        public:
            Variant(const DataType& data, const std::string& name)
                : Named{name}
                , WithData{data}
            {
            }
        };
    private:
        std::map<Variant::DataType, Variant*> variants_;
    public:

        explicit Item(const IdType& id, std::string name)
            : Named(std::move(name))
            , WithId{id}
        {
        }

        Item(const Item&) = delete;
        Item(const Item&&) = delete;
        Item& operator=(const Item&) = delete;
        Item& operator=(const Item&&) = delete;

        ~Item()
        {
            for(auto& i: this->variants_) {
                delete i.second;
            }
            this->variants_.clear();
        }

        [[nodiscard]]
        bool hasVariants() const { return !this->variants_.empty(); }

        void addUname(const std::string& uname);

        Variant* addVariant(Variant::DataType data, const std::string& name);

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
        auto getVariants() const
        {
            return this->variants_;
        }

        static const Item* get(const IdType& id);
        static const Item* getByUname(const std::string& uname);
        static Item* add(const IdType& id, const std::string& name);
        static void clearList();
        static const std::vector<const Item*>& list();

        static std::string queryName(const IdType& id, const Variant::DataType& data);

    };
}