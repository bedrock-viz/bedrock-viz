#include "item.h"

#include <unordered_map>
#include <vector>
#include "config.h"
#include "../../util.h"
#include "../../utils/pointer_array.h"

namespace
{
    using mcpe_viz::PointerArray;
    using Wrapper = PointerArray<mcpe_viz::Item, mcpe_viz::kMaxItemCount>;

    std::vector<mcpe_viz::Item*> sItems;
    std::unordered_map<std::string, mcpe_viz::Item*> unameBlockMap;
}

namespace mcpe_viz
{
    
}