#include "xml/load_entity.h"
#include "minecraft/v2/entity.h"
#include "logger.h"
#include "util.h"

namespace mcpe_viz
{
    int load_entity(const pugi::xml_node& node)
    {
        for(auto& i: node.children("entity")) {
            std::string name{ i.attribute("name").as_string()};
            auto id = i.attribute("id").as_int(-1);

            if (id == -1 || name.empty()) {
                log::error("add entity failed(name={}, id={}",
                           name, id);
                return -1;
            }
            auto entity = Entity::add(id, name);
            if (entity == nullptr) {
                log::error("add entity failed(name={} id={:x}",
                    name, id);
                return -1;
            }
            std::string etype{ i.attribute("etype").as_string()};
            if (etype == "H") {
                entity->etype = Entity::EType::H;
            }
            else if (etype == "P") {
                entity->etype = Entity::EType::P;
            }

            auto unames = i.attribute("uname").as_string();
            auto uname_list = mysplit(unames, ';');
            for(auto& uname: uname_list) {
                entity->addUname(uname);
            }

            for (auto& j: i.children("entityvariant")) {
                std::string var_name{ j.attribute("name").as_string() };
                auto var_data = j.attribute("extradata").as_int();

                if (var_name.empty()) {
                    log::error("add entity variant failed(name={}, data={}",
                               var_name, var_data);
                    return -1;
                }
                auto variant = entity->addVariant(var_data, var_name);
                if (variant == nullptr) {
                    log::error("add entity variant failed(name={}, data={}",
                               var_name, var_data);
                    return -1;
                }
            }
        }
        return 0;
    }
}
