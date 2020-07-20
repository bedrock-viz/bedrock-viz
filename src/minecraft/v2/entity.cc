#include "minecraft/v2/entity.h"
#include "config.h"
#include "util.h"
#include "utils/pointer_array.h"

#include <unordered_map>
#include <vector>


namespace
{
    using mcpe_viz::PointerArray;
    using Wrapper = PointerArray<mcpe_viz::Entity, mcpe_viz::kMaxEntityCount>;

    std::vector<const mcpe_viz::Entity*> sEntities;
    std::unordered_map<std::string, mcpe_viz::Entity*> sUnameEntityMap;
}

namespace mcpe_viz
{
    void Entity::addUname(const std::string& uname)
    {
        if (sUnameEntityMap.find(uname) != sUnameEntityMap.end()) {
            auto entity = sUnameEntityMap[uname];
            log::error("uname({}) already exists (id=0x{:x} name={})",
                uname, entity->id, entity->name);
        }
        else {
            sUnameEntityMap[uname] = this;
        }
    }

    Entity::Variant* Entity::addVariant(Variant::DataType data, const std::string& name)
    {
        if (this->variants_.find(data) != this->variants_.end()) {
            log::error("Entity variant(data={}, id={}, name={}) already exists", data, this->id, this->name);
            return nullptr;
        }
        auto const variant = new Variant(data, name);
        this->variants_[data] = variant;
        return variant;
    }

    const Entity* Entity::get(IdType id)
    {
        auto& instance = Wrapper::value();
        if (id < 0 || id >= kMaxEntityCount) {
            log::debug("Entity::Get out of range(id={}, length={})", id, kMaxEntityCount);
            return nullptr;
        }
        return instance[id];
    }

    const Entity* Entity::getByUname(const std::string& uname)
    {
        std::string s = uname;
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);

        auto const iter = sUnameEntityMap.find(s);
        if (iter != sUnameEntityMap.end()) {
            return iter->second;
        }
        return nullptr;
    }

    Entity* Entity::add(IdType id, const std::string& name)
    {
        auto& instance = Wrapper::value();
        if (instance[id] != nullptr) {
            log::error("Entity id={}(0x{:x} name={} already exists", id, id, name);
            return nullptr;
        }
        auto const entity = new Entity(id, name);
        instance[id] = entity;
        sEntities.emplace_back(entity);
        return entity;
    }

    void Entity::clearList() {
        sUnameEntityMap.clear();
        sEntities.clear();
        for(size_t i = 0; i < Wrapper::value().size(); ++i) {
            // TODO: Fix nullptr issues due to data structures
            Entity* ent = Wrapper::value()[i];
            Wrapper::value()[i] = nullptr;
            if(ent != nullptr){
                delete ent;
            }
        }
    }

    const std::vector<const Entity*>& Entity::list()
    {
        return sEntities;
    }

    std::string Entity::queryName(const IdType& id, const Variant::DataType& data)
    {
        auto entity = Entity::get(id);
        if (entity != nullptr) {
            return entity->getVariantName(data);
        }
        else {
            char buffer[256];
            sprintf(buffer, "(Unknown-entity-id-%d-data-%d)", id, data);
            return std::string(buffer);
        }
    }

}
