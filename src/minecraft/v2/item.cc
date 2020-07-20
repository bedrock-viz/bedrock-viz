#include "minecraft/v2/item.h"
#include "config.h"
#include "util.h"
#include "utils/pointer_array.h"
#include "utils/unknown_recorder.h"

#include <iostream>
#include <unordered_map>

namespace
{
    using mcpe_viz::PointerArray;
    using Wrapper = PointerArray<mcpe_viz::Item, mcpe_viz::kMaxItemCount>;

    std::vector<const mcpe_viz::Item*> sItems;
    std::unordered_map<std::string, mcpe_viz::Item*> unameItemMap;
}

namespace mcpe_viz
{
    void Item::addUname(const std::string& uname)
    {
        if (unameItemMap.find(uname) != unameItemMap.end()) {

            auto item = unameItemMap[uname];

            log::error("uname({}) already exists (id=0x{:x} name={})",
                uname, item->id, item->name);
        }
        else {
            unameItemMap[uname] = this;
        }
    }

    Item::Variant* Item::addVariant(Variant::DataType data, const std::string& name)
    {
        if (this->variants_.find(data) != this->variants_.end()) {
            log::error("Variant(data={}, itemId={}, itemName={}) already exists", data, this->id, this->name);
            return nullptr;
        }
        auto const variant = new Variant(data, name);
        this->variants_[data] = variant;
        return variant;
    }

    const Item* Item::get(const IdType& id)
    {
        auto& instance = Wrapper::value();
        return instance[id];
    }

    const Item* Item::getByUname(const std::string& uname)
    {
        std::string s = uname;
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        auto const iter = unameItemMap.find(s);
        if (iter != unameItemMap.end()) {
            return iter->second;
        }
        return nullptr;
    }

    Item* Item::add(const IdType& id, const std::string& name)
    {
        auto& instance = Wrapper::value();
        if (instance[id] != nullptr) {
            log::error("Item id={} name={} already exists", id, name);
            return nullptr;
        }
        auto const item = new Item(id, name);
        instance[id] = item;
        sItems.emplace_back(item);
        return item;
    }

    void Item::clearList() {
        unameItemMap.clear();
        sItems.clear();
        for(size_t i = 0; i < Wrapper::value().size(); ++i) {
            // TODO: Fix nullptr issues due to data structures
            Item* item = Wrapper::value()[i];
            Wrapper::value()[i] = nullptr;
            if(item != nullptr){
                delete item;
            }
        }
    }

    const std::vector<const Item*>& Item::list()
    {
        return sItems;
    }

    std::string Item::queryName(const IdType& id, const Variant::DataType& data)
    {
        auto item = Item::get(id);
        if (item != nullptr) {
            auto variant = item->getVariantByBlockData(data);
            if (variant != nullptr) {
                return variant->name;
            }
            // TODO record unknown if necessary
            // record_unknown_item_variant(id, item->name, data);
            return item->name;
        }
        record_unknown_item_id(id);
        char buffer[256];
        sprintf(buffer, "(Unknown-item-id-%d-data-%d)", id, data);
        return std::string(buffer);
    }
}
