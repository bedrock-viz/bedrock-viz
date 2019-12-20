#include "args.h"
#include <cstdio>


namespace {

    const char * const usage = R"(Usage:

    bedrock-viz [required parameters] [options]

Required Parameters:
    --db dir                 Directory which holds world files (level.dat is in this dir)

Options:
    --outdir dir             Directory for output file(s). Defaults to "./output/"
    
    --html                   Create html and javascript files to use as a fancy viewer
    --html-most              Create html, javascript, and most image files to use as a fancy viewer
    --html-all               Create html, javascript, and *all* image files to use as a fancy viewer
    --auto-tile              Automatically tile the images if they are very large
    --tiles[=tilew,tileh]    Create tiles in subdirectory tiles/ (useful for LARGE worlds)

    --hide-top=did,bid       Hide a block from top block (did=dimension id, bid=block id)
    --force-top=did,bid      Force a block to top block (did=dimension id, bid=block id)
    --geojson-block=did,bid  Add block to GeoJSON file for use in web app (did=dimension id, bid=block id)

    --schematic-get did,x1,y1,z1,x2,y2,z2,fnpart
                             Create a schematic file (fnpart) from (x1,y1,z1) to (x2,y2,z2) in dimension (did)
        (note: [=did] is optional dimension-id - if not specified, do all dimensions; 0=Overworld; 1=Nether)\n"
    --grid[=did]             Display chunk grid on top of images

    --all-image[=did]        Create all image types
    --biome[=did]            Create a biome map image
    --grass[=did]            Create a grass color map image
    --height-col[=did]       Create a height column map image (red is below sea; gray is sea; green is above sea)
    --height-col-gs[=did]    Create a height column map image (grayscale)
    --height-col-alpha[=did] Create a height column map image (alpha)
    --shaded-relief[=did]    Create a shaded relief image
    --blocklight[=did]       Create a block light map image
    --skylight[=did]         Create a sky light map image
    --slime-chunk[=did]      Create a slime chunk map image

    --slices[=did]           Create slices (one image for each layer)
    --movie[=did]            Create movie of layers
    --movie-dim x,y,w,h      Integers describing the bounds of the movie (UL X, UL Y, WIDTH, HEIGHT)

    --xml fn                 XML file containing data definitions

    --no-force-geojson       Don't load geojson in html because we are going to use a web server (or Firefox)

    --detail                 Log extensive details about the world to the log file
    --verbose                verbose output
    --quiet                  supress normal output, continue to output warning and error messages
    --help                   show this info
)";
}

namespace mcpe_viz{
    void print_usage()
    {
        printf("%s", usage);
    }
}
