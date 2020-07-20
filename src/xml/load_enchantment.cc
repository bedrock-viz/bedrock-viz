#include "xml/load_biome.h"
#include "logger.h"
#include "minecraft/v2/enchantment.h"

namespace mcpe_viz
{
    int load_enchantment(const pugi::xml_node& node)
    {
        for(auto& i : node.children("enchantment")) {
            std::string name{i.attribute("name").as_string()};
            std::string offName{i.attribute("officialName").as_string()};
            auto id = i.attribute("id").as_int(-1);
            if (name.empty() || id == -1 || offName.empty()) {
                log::error("add enchantment failed(name={}, id={})",
                           name, id);
                return -1;
            }
            auto enchant = Enchantment::add(id, name, offName);
            if (enchant == nullptr) {
                log::error("add enchantment failed(name={}, id={})",
                           name, id);
                return -1;
            }
        }
        return 0;
    }
}
