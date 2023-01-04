#include "minecraft/v2/item.h"
#include "logger.h"
#include "util.h"

namespace mcpe_viz
{
    int load_item(const pugi::xml_node& node)
    {
        for (auto& i: node.children("item")) {
            std::string name{ i.attribute("name").as_string() };

            auto id = i.attribute("id").as_int(-1);

            if (id == -1 || name.empty()) {
                log::error("add item failed(name={}, id={}({:x})",
                    name, id, id);
                return -1;
            }

            auto item = Item::add(id, name);
            if (item == nullptr) {
                log::error("add item failed(name={}, id={}({:x})",
                    name, id, id);
                return -1;
            }

            auto unames = i.attribute("uname").as_string();
            auto uname_list = mysplit(unames, ';');
            for (auto& uname : uname_list) {
                item->addUname(uname);
            }

            for(auto& j : i.children("itemvariant")) {
                std::string var_name{ j.attribute("name").as_string() };
                auto extradata = j.attribute("extradata").as_int(-1);
                if (var_name.empty() || extradata == -1) {
                    log::error("add item variant failed(name={}, data={})",
                        var_name, extradata);
                    return -1;
                }
                auto variant = item->addVariant(extradata, var_name);
                if (variant == nullptr) {
                    log::error("add item variant failed(name={}, data={})",
                        var_name, extradata);
                    return -1;
                }
                auto var_unames = j.attribute("uname").as_string();
                auto var_uname_list = mysplit(var_unames, ';');
                for (auto& var_uname : var_uname_list) {
                    /* there are a LOT of item variants that we don't know the proper
                     * uname for yet. They just have place holders in the xml waiting to
                     * be filled in. Skip those placeholders so we don't get conflicts.
                     */
                    if (var_uname != "minecraft:") {
                        item->addUname(var_uname);
                    }
                }
            }
        }
        return 0;
    }
}
