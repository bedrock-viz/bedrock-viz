#include "load_block.h"
#include "../minecraft/v2/block.h"
#include "../logger.h"
#include "../util.h"

namespace mcpe_viz
{
    int load_block(const pugi::xml_node& node)
    {
        for (auto& i: node.children("block")) {
            std::string name{ i.attribute("name").as_string()};
            auto id = i.attribute("id").as_int(-1);
            

            if (id == -1 || name.empty()) {
                log::error("add block failed(name={}, id={}",
                    name, id);
                return -1;
            }

            auto block = Block::add(id, name);

            if (block == nullptr) {
                log::error("add block failed(name={}, id={}",
                           name, id);
                return -1;
            }
            auto color = i.attribute("color").as_int(-1);
            if (color != -1) {
                block->color(color);
            }

            auto solid = i.attribute("solid").as_bool(true);
            auto opaque = i.attribute("opaque").as_bool(true);
            auto liquid = i.attribute("liquid").as_bool(false);
            auto spawnable = i.attribute("spawnable").as_bool(true);

            auto unames = i.attribute("uname").as_string();
            auto uname_list = mysplit(unames, ';');
            for(auto& uname: uname_list) {
                block->addUname(uname);
            }

            block->solid = solid;
            
            block->opaque = opaque;

            block->liquid = liquid;
            block->spawnable = spawnable;

            for(auto& j : i.children("blockvariant")) {
                std::string var_name{ j.attribute("name").as_string() };
                auto var_data = j.attribute("blockdata").as_int();
                // TODO variant may have uname
                // auto var_unames = j.attribute("uname").as_string();
                auto var_color = j.attribute("color").as_int(-1);
                auto var_dcolor = j.attribute("dcolor").as_int(-1);
                auto spawn_attrib_empty = j.attribute("spawnable").empty();
                auto var_spawnable = j.attribute("spawnable").as_bool(true);
                
                if (var_name.empty()) {
                    log::error("add block variant failed(name={}, data={}",
                        var_name, var_data);
                    return -1;
                }

                auto variant = block->addVariant(var_data, var_name);
                if (var_color != -1) {
                    if (var_dcolor != -1) {
                        var_color += var_dcolor;
                    }
                    variant->color(var_color);
                }
                else {
                    auto new_color = local_be32toh(block->color());
                    new_color += var_data;
                    variant->color(new_color);
                }

                if (!spawn_attrib_empty) {
                    variant->spawnable = var_spawnable;
                }
                else {
                    variant->spawnable = block->spawnable;
                }
                
            }
        }
        return 0;
    }
}
