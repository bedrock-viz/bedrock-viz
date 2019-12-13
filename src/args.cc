#include "args.h"
#include <cstdio>
#include <cassert>

#ifndef _MSC_VER

#include <getopt.h>

#else
#include "getopt/getopt.h"
#endif

#include "logger.h"

namespace {
    option long_opt_list[] = {
            {"db",                 required_argument, nullptr, 'D'},
            {"out",                required_argument, nullptr, 'O'},

            {"xml",                required_argument, nullptr, 'X'},
            {"log",                required_argument, nullptr, 'L'},

            {"detail",             no_argument,       nullptr, '@'},

            {"hide-top",           required_argument, nullptr, 'H'},
            {"force-top",          required_argument, nullptr, 'F'},
            {"geojson-block",      required_argument, nullptr, '+'},

//            {"check-spawn",        required_argument, nullptr, 'C'},
//            {"check-spawnable",    required_argument, nullptr, 'C'},

            {"schematic",          required_argument, nullptr, 'Z'},
            {"schematic-get",      required_argument, nullptr, 'Z'},

            {"all-image",          optional_argument, nullptr, 'A'},
            {"biome",              optional_argument, nullptr, 'B'},
            {"grass",              optional_argument, nullptr, 'g'},
            {"height-col",         optional_argument, nullptr, 'd'},
            {"height-col-gs",      optional_argument, nullptr, '#'},
            {"height-col-alpha",   optional_argument, nullptr, 'a'},
            {"shaded-relief",      optional_argument, nullptr, 'S'},
            {"blocklight",         optional_argument, nullptr, 'b'},
            {"skylight",           optional_argument, nullptr, 's'},
            {"slime-chunk",        optional_argument, nullptr, '%'},

            {"slices",             optional_argument, nullptr, '('},

            {"movie",              optional_argument, nullptr, 'M'},
            {"movie-dim",          required_argument, nullptr, '*'},

            {"grid",               optional_argument, nullptr, 'G'},

            {"html",               no_argument,       nullptr, ')'},
            {"html-most",          no_argument,       nullptr, '='},
            {"html-all",           no_argument,       nullptr, '_'},
            {"no-force-geojson",   no_argument,       nullptr, ':'},

            {"auto-tile",          no_argument,       nullptr, ']'},
            {"tiles",              optional_argument, nullptr, '['},

            {"shortrun",           no_argument,       nullptr, '$'}, // this is just for testing
            {"colortest",          no_argument,       nullptr, '!'}, // this is just for testing

            {"flush",              no_argument,       nullptr, 'f'},

            {"leveldb-filter",     required_argument, nullptr, '<'},
            {"leveldb-block-size", required_argument, nullptr, '>'},

            {"find-images",        required_argument, nullptr, '"'},

            {"verbose",            no_argument,       nullptr, 'v'},
            {"quiet",              no_argument,       nullptr, 'q'},
            {"help",               no_argument,       nullptr, 'h'},
            {nullptr,              no_argument,       nullptr, 0}
    };

    int32_t parseDimIdOptArg(const char *arg) {
        assert(arg != nullptr);
        using namespace mcpe_viz;
        int32_t did = strtol(arg, nullptr, 10);
        if (did < kDimIdOverworld || did > kDimIdCount) {
            bedrock_viz::logger::warn(
                    "Invalid dimension-id supplied ({}), defaulting to Overworld only",
                    did);
            did = kDimIdOverworld;
        }
        return did;
    }

    const char * const usage = R"(Usage:

    bedrock-viz [required parameters] [options]

Required Parameters:
    --db dir                 Directory which holds world files (level.dat is in this dir)

Options:
    --outdir dir             Directory for output file(s). Defaults to "./output/"
    --detail                 Log extensive details about the world to the log file
    --html                   Create html and javascript files to use as a fancy viewer
    --html-most              Create html, javascript, and most image files to use as a fancy viewer
    --html-all               Create html, javascript, and *all* image files to use as a fancy viewer
    --auto-tile              Automatically tile the images if they are very large
    --tiles[=tilew,tileh]    Create tiles in subdirectory tiles/ (useful for LARGE worlds)

    --hide-top=did,bid       Hide a block from top block (did=dimension id, bid=block id)
    --force-top=did,bid      Force a block to top block (did=dimension id, bid=block id)
    --geojson-block=did,bid  Add block to GeoJSON file for use in web app (did=dimension id, bid=block id)

    --check-spawn did,x,z,dist  Add spawnable blocks to the geojson file (did=dimension id; checks a circle of radius 'dist' centered on x,z)
    --schematic-get did,x1,y1,z1,x2,y2,z2,fnpart   Create a schematic file (fnpart) from (x1,y1,z1) to (x2,y2,z2) in dimension (did)
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

    --verbose                verbose output
    --quiet                  supress normal output, continue to output warning and error messages
    --help                   show this info
)";
}

namespace mcpe_viz {
    Args::Args(int argc, char **argv) {
        using namespace bedrock_viz;
        int option_index = 0;
        int optc = 0;

        while ((optc = getopt_long_only(argc, argv, "", long_opt_list, &option_index)) != -1) {
            switch (optc) {
                case 'O':
                    this->fnOutputBase = optarg;
                    break;
                case 'X':
                    this->fnXml = optarg;
                    break;
                case 'L':
                    this->fnLog = optarg;
                    break;
                case 'D':
                    this->dirLeveldb = optarg;
                    break;
                case '@':
                    this->doDetailParseFlag = true;
                    break;
                case '<':
                    this->leveldbFilter = strtol(optarg, nullptr, 10);
                    if (this->leveldbFilter < 0) {
                        logger::warn("Invalid LevelDB filter({})", this->leveldbFilter);
                        this->leveldbFilter = 0;
                    }
                    break;
                case '>':
                    this->leveldbBlockSize = strtol(optarg, nullptr, 10);
                    if (this->leveldbBlockSize < 0) {
                        logger::warn("Invalid LevelDB block size({})", this->leveldbBlockSize);
                        this->leveldbBlockSize = 4096;
                    }
                    break;

                case '"':
                    this->doFindImages = true;
                    // todobig - specify an in and an out dir
                    this->dirFindImagesIn = optarg;
                    this->dirFindImagesOut = optarg;
                    break;

                case 'H': {
                    bool pass = false;
                    int32_t dimId, blockId;
                    if (sscanf(optarg, "%d,0x%x", &dimId, &blockId) == 2) {
                        pass = true;
                    } else if (sscanf(optarg, "%d,%d", &dimId, &blockId) == 2) {
                        pass = true;
                    }

                    if (pass) {
                        // check dimId
                        if (dimId < kDimIdOverworld || dimId >= kDimIdCount) {
                            pass = false;
                        }
                        if (pass) {
                            this->hide_top.push_back(DimIdBlockId{dimId, blockId});
//                            this->hide_top = std::make_tuple(true, dimid, blockId);
//                            world->dimDataList[dimId]->blockHideList.push_back(blockId);
                        }
                    }

                    if (!pass) {
                        logger::error("Failed to parse --hide-top {}", optarg);
                        this->valid = false;
                    }
                    break;
                }
                case 'F': {
                    bool pass = false;
                    int32_t dimId, blockId;
                    if (sscanf(optarg, "%d,0x%x", &dimId, &blockId) == 2) {
                        pass = true;
                    } else if (sscanf(optarg, "%d,%d", &dimId, &blockId) == 2) {
                        pass = true;
                    }

                    if (pass) {
                        // check dimId
                        if (dimId < kDimIdOverworld || dimId >= kDimIdCount) {
                            pass = false;
                        }
                        if (pass) {
                            this->force_top.push_back(DimIdBlockId{dimId, blockId});
                            // world->dimDataList[dimId]->blockForceTopList.push_back(blockId);
                        }
                    }

                    if (!pass) {
                        logger::error("Failed to parse --force-top {}", optarg);
                        this->valid = false;
                    }
                }
                    break;

                case '+': {
                    bool pass = false;
                    int32_t dimId, blockId;
                    if (sscanf(optarg, "%d,0x%x", &dimId, &blockId) == 2) {
                        pass = true;
                    } else if (sscanf(optarg, "%d,%d", &dimId, &blockId) == 2) {
                        pass = true;
                    }

                    if (pass) {
                        // check dimId
                        if (dimId < kDimIdOverworld || dimId >= kDimIdCount) {
                            pass = false;
                        }
                        if (pass) {
                            this->geojson_block.push_back(DimIdBlockId{dimId, blockId});
                            // world->dimDataList[dimId]->blockToGeoJSONList.push_back(blockId);
                        }
                    }

                    if (!pass) {
                        logger::error("Failed to parse --geojson-block {}", optarg);
                        this->valid = false;
                    }
                    break;
                }
                case 'Z': {
                    bool pass = false;
                    int32_t dimId = 0, x1 = 0, y1 = 0, z1 = 0, x2 = 0, y2 = 0, z2 = 0;
                    char fnSchematic[2048];
                    memset(fnSchematic, 0, 2048);
                    // todo - ugly sscanf for a string
                    if (sscanf(optarg, "%d,%d,%d,%d,%d,%d,%d,%s", &dimId, &x1, &y1, &z1, &x2, &y2, &z2, fnSchematic) ==
                        8) {
                        pass = true;
                    }

                    if (pass) {
                        // todo - check params

                        if (dimId < kDimIdOverworld || dimId >= kDimIdCount) {
                            pass = false;
                        }

                        if (pass) {
                            this->schematics.emplace_back(std::make_pair(dimId, Schematic(x1, y1, z1, x2, y2, z2, fnSchematic)));
                            //world->dimDataList[dimId]->addSchematic(x1, y1, z1, x2, y2, z2, fnSchematic);
                        }
                    }

                    if (!pass) {
                        logger::error("Failed to parse --schematic {}", optarg);
                        this->valid = false;
                    }
                    break;
                }
                case 'G':
                    this->doGrid = parseDimIdOptArg(optarg);
                    break;
                case ')':
                    this->doHtml = true;
                    break;
                case '[': {
                    this->doTiles = true;
                    int32_t tw = 0, th = 0;
                    if (optarg) {
                        if (sscanf(optarg, "%d,%d", &tw, &th) == 2) {
                            this->tileWidth = tw;
                            this->tileHeight = th;
                            logger::info("Overriding tile dimensions: {} x {}",
                                         this->tileWidth, this->tileHeight);
                        }
                    }
                    break;
                }
                case ']':
                    this->autoTileFlag = true;
                    break;

                case '=':
                    // html most
                    this->doHtml = true;
                    this->doImageBiome =
                    this->doImageGrass =
                    this->doImageHeightCol =
                    this->doImageHeightColGrayscale =
                    this->doImageHeightColAlpha =
                    this->doImageShadedRelief =
                    this->doImageLightBlock =
                    this->doImageLightSky =
                    this->doImageSlimeChunks =
                            kDoOutputAll;
                    break;

                case '_':
                    // html all
                    this->doHtml = true;
                    this->doImageBiome =
                    this->doImageGrass =
                    this->doImageHeightCol =
                    this->doImageHeightColGrayscale =
                    this->doImageHeightColAlpha =
                    this->doImageShadedRelief =
                    this->doImageLightBlock =
                    this->doImageLightSky =
                    this->doImageSlimeChunks =
                            kDoOutputAll;
                    this->doSlices = kDoOutputAll;
                    break;

                case ':':
                    this->noForceGeoJSONFlag = true;
                    break;

                case 'B':
                    this->doImageBiome = parseDimIdOptArg(optarg);
                    break;
                case 'g':
                    this->doImageGrass = parseDimIdOptArg(optarg);
                    break;
                case 'd':
                    this->doImageHeightCol = parseDimIdOptArg(optarg);
                    break;
                case '#':
                    this->doImageHeightColGrayscale = parseDimIdOptArg(optarg);
                    break;
                case 'a':
                    this->doImageHeightColAlpha = parseDimIdOptArg(optarg);
                    break;
                case 'S':
                    this->doImageShadedRelief = parseDimIdOptArg(optarg);
                    break;
                case 'b':
                    this->doImageLightBlock = parseDimIdOptArg(optarg);
                    break;
                case 's':
                    this->doImageLightSky = parseDimIdOptArg(optarg);
                    break;
                case '%':
                    this->doImageSlimeChunks = parseDimIdOptArg(optarg);
                    break;

                case 'A':
                    this->doImageBiome =
                    this->doImageGrass =
                    this->doImageHeightCol =
                    this->doImageHeightColGrayscale =
                    this->doImageHeightColAlpha =
                    this->doImageShadedRelief =
                    this->doImageLightBlock =
                    this->doImageLightSky =
                    this->doImageSlimeChunks =
                            parseDimIdOptArg(optarg);
                    break;

                case '(':
                    this->doSlices = parseDimIdOptArg(optarg);
                    break;
                case 'M':
                    this->doMovie = parseDimIdOptArg(optarg);
                    break;
                case '*': {
                    auto count = sscanf(optarg, "%d,%d,%d,%d",
                                        &this->movieX, &this->movieY,
                                        &this->movieW, &this->movieH);
                    if (count != 4) {
                        this->valid = false;
                        logger::error("Failed to parse --movie-dim");
                    }
                    break;
                }
                case '$':
                    this->shortRunFlag = true;
                    break;
                case '!':
                    this->colorTestFlag = true;
                    break;
                case 'v':
                    this->verboseFlag = true;
                    break;
                case 'q':
                    this->quietFlag = true;
                    break;
                case 'h':
                    this->helpFlag = true;
                    break;
                default:
                    logger::error("Unrecognized option: {}", optc);
                    this->valid = false;
                    break;
            }
        }
    }

    void Args::initInstance(int argc, char **argv) {
        assert(instance_ == nullptr);
        instance_ = new Args(argc, argv);
    }

    Args *Args::instance_ = nullptr;

    void Args::destroyInstance() {
        assert(instance_ != nullptr);
        delete instance_;
        instance_ = nullptr;
    }

    Args *Args::instance() {
        assert(instance_ != nullptr);
        return instance_;
    }

    void print_usage() {
        printf("%s", usage);
    }
}
