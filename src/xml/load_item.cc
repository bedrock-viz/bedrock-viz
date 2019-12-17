#include "load_block.h"
#include "../minecraft/v2/item.h"
#include "../logger.h"
#include "../util.h"

namespace mcpe_viz
{
    int load_item(const pugi::xml_node& node)
    {
        for (auto& i: node.children("item")) {
            for(auto& j : i.children("itemvariant")) {
                
            }
        }
        return 0;
    }
}
