#include "minecraft/v2/item.h"
#include "config.h"
#include "util.h"
#include "utils/unknown_recorder.h"
#include "define.h"

#include <iostream>
#include <unordered_map>

namespace
{
    std::map<const mcpe_viz::WithId::IdType, const mcpe_viz::Item*> allItemsById;
    std::unordered_map<std::string, mcpe_viz::Item*> unameItemMap;
}

namespace mcpe_viz
{
    WithId::IdType Item::lastAssignedItemId = mcpe_viz::kMaxLegacyItemId + 1;

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
        try {
            auto itemPtr = allItemsById.at(id);
            return itemPtr;
        } catch (std::out_of_range& outOfRange) {
            return nullptr;
        }
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

    Item* Item::add(IdType id, const std::string& name)
    {
        if (id == UNKNOWN_ID) {
            id = lastAssignedItemId++;
        }

        auto const item = new Item(id, name);

        if (allItemsById[id] != nullptr) {
            log::error("Item id={} name={} already exists", id, name);
            return nullptr;
        }

        allItemsById[id] = item;

        return item;
    }

    void Item::clearList() {
        unameItemMap.clear();
        for(auto& item : allItemsById) {
            delete item.second;
            item.second = nullptr;
        }

        allItemsById.clear();
    }

    std::map<const WithId::IdType, const Item*> Item::all()
    {
        return allItemsById;
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
