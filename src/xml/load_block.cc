#include "load_block.h"
#include "../minecraft/v2/block.h"
#include "../logger.h"
namespace mcpe_viz
{
    int load_block(const pugi::xml_node& node)
    {
        for (auto& i: node.children("block")) {
            std::string name{ i.attribute("name").as_string()};
            auto id = i.attribute("id").as_int();
            auto color = i.attribute("color").as_int();

            auto block = Block::add(id, name);

            if (block == nullptr) {
                log::error("add block failed(name={}, id={}",
                           name, id);
                return -1;
            }

            block->color(color);

            bool solid = i.attribute("solid").as_bool(true);
            bool opaque = i.attribute("opaque").as_bool(false);
            bool liquid = i.attribute("liquid").as_bool(false);
            bool spawnable = i.attribute("spawnable").as_bool(true);

            // TODO uname

            block->solid = solid;
            block->opaque = opaque;
            block->liquid = liquid;
            block->spawnable = spawnable;

            // TODO add variant
        }
        return 0;
    }
}
