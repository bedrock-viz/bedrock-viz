#include "loader.h"
#include "../logger.h"
#include "load_biome.h"

namespace mcpe_viz
{
    int load_xml(const std::string& filepath)
    {
        pugi::xml_document doc;
        auto result = doc.load_file(filepath.c_str());

        if (!result) {
            log::error("xml file({}) not parsed successfully: {}",
                filepath, result.description());
            return -1;
        }

        if (load_biome(doc.child("xml").child("biomelist")) != 0) {
            log::error("biomelist parse failed");
            return -1;
        }

        return 0;
    }
}
