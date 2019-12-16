#include "load_biome.h"
#include "../minecraft/v2/biome.h"
#include "../logger.h"
namespace mcpe_viz
{
    int load_biome(const pugi::xml_node& node)
    {
        for(auto& i : node.children("biome")) {
            std::string name{i.attribute("name").as_string()};
            auto color = i.attribute("color").as_int();
            auto id = i.attribute("id").as_int();
            auto biome = Biome::add(id, name);
            if (biome == nullptr) {
                log::error("add biome failed(name={}, id={}",
                           name, id);
                return -1;
            }
            biome->color(color);
        }
        return 0;
    }
}
