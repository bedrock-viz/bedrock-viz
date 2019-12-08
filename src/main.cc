/*
  Minecraft Pocket Edition (MCPE) World File Visualization & Reporting Tool
  (c) Plethora777, 2015.9.26

  GPL'ed code - see LICENSE

  Requires Mojang's modified LevelDB library (see README.md for details)
  Requires libnbt++ (see README.md for details)

  To build it, use cmake


  todozooz
  * update web ui for new mobs / objects / entities
  * block or item based on ID is no longer valid?! (search on 256 in nbt code)
  
  maybe?
  * move from xml to json?
  * ui for mobs from xml instead of hard-coded?

  todohere

  * as of beta 1.2.x everything has gone quite mad:
  - cubic chunks now have a palette instead of direct block-id's
  - " " no longer have block light / sky light?! (therefore, can't do check spawnable anymore)



  * as of 0.17 - grass color appears to no longer be a thing that is stored in the data files?  remove related code here and in js?


  * as of 0.16 entity id's have extra data in the high bytes -- collect these and see if we can figure out what they are -- see mcpe.nbt.cc
  
  * update inventory images for 0.16 and 0.17


  * web - tool to extract schematics for a 3d cube of space -- produce a layer-by-layer depiction of the blocks in the cube
  -- see: http://minecraft.gamepedia.com/Schematic_file_format  (NBT file format for schematics)
  -- https://irath96.github.io/webNBT/ = web nbt viewer/editor

  * /u/JustinUser -- reset world file outside given area (e.g. X blocks from spawn; given rectangles; etc)

  * see data dump here; http://pastebin.com/tuMyCDyc -- any data we need? (updated: http://pastebin.com/be3dwGFA)
  also: https://www.reddit.com/r/MCPE/comments/4nip1u/updated_the_blocksitems_list_for_0150/

  * tool to show differences between worlds (v1 vs v2 etc)
  -- layer by layer?
  -- geojson?

  * add a tool to help convert from overwold to nether coordinates

  * check minor update to libnbt++ 5/2/2016

  * should error on fail copy js lib files

  * out56.log had a zombie pigman with 'nan' for position - filter this

  * xml missing item 358 (probably filled map); probably other map related things + maps in item frames etc

  * possible xml errors:
  -- another1-dropbox - top of pyramid - quartz with unknown blockdata (6 and 10)
  -- "upper slab, quartz" shows as "upper slab, nether brick" -- R.E. all variants in creative
  -- "flower pot" w/ cactus shows as flower pot with poppy -- R.E. all variants in creative
  -- block of quartz variants appear to be wrong -- R.E. in creative

  * win gui -- 
  -- icon
  -- use taskbar progress bar - see: http://www.codeproject.com/Articles/42345/Windows-Goodies-in-C-Taskbar-Progress-and-Status

  * could use png files to determine avg color of each block
  * add icons for enchanted items?
  * expand icons idea - use lil images of mobs etc in UI?

  * test web app on mobile -- what can be optimized? smaller tiles?

  * option to geojson items in chests etc (e.g. melon seeds; beetroot seeds)

  * NO -- rethink coordinates used in js -- just negate the y-coord? could then adjust mouse pos output accordingly

  * bug: js in tile mode, zoom to extent, does not show full extent (right side is chopped for 'another1')
  -- also see reddit bartszelag world for extreme example (very wide, not so high world)
  -- ol bug?

  * check spawnable code - it might be missing spots?

  * gui: allow users to easily add cmd-line params
  -- a btn for mcpe_viz --help output (to see usage)
  -- a text field for "Add Params"

  * minimize geojson output?
  -- remove spaces
  -- simplify the properties? e.g. GType=Foo where Foo is the type of this record

  * auto-tile (and useTilesFlag) should be per-dimension
  -- js should use per-dimension tile vars instead of global

  * optimize tiler (slow libpng is probably the limiting factor)

  * finalize new elevation stuff
  -- simplify it?  combine the shaded relief and the alpha into one web app ui control + opacity slider -- or into one layer


  todobig

  * investigate parsing mcpc worlds
  -- create an abstraction lib/base class of libminecraftworld (search on 'todolib')
  ---- move leveldb stuff to something like libminecraftworld.leveldb
  ---- create/adapt a lib for libminecraftworld.anvil (and mcregion)
  ---- read AND write
  ---- one way that ppl can convert mcpe to mcpc:
  ------ https://www.reddit.com/r/MCPE/comments/1y996x/tutorial_how_to_convert_worlds_from_pe_to_pc/
  -- note: PocketMine uses mcregion, anvil or leveldb
  ---- see CrystalKingdom.zip for an example pocketmine world w/ mcregion files (level.dat gzipped?)

  * allow user to provide APK (or file from it) and then use assets from the APK in web app?
  -- e.g. - chest contents w/ icons for items

  * add options or separate tool to do world modification
  -- remove chunks (to allow them to regenerate w/ new features from an updated mcpe)

  * add "interesting" blocks to geojson (this is something like the "layers" idea)
  -- end portal frame
  -- diamonds
  -- ores: gold, iron, coal, redstone, lapis
  -- trees: (by type -- use wood up/down?)
  -- cobwebs
  -- flowers?
  -- crops: beetroot esp + netherwart + melon stem + pumpkin stem; lily pad?
  -- ice + packed ice (how to coalesce?)

  * change git layout 
  -- remove winX.zip files; make "release" for each update w/ the winX.zip files
  -- put code in subdirs?

  * android/ios app

  * write script to check for updates to js/ files?

  * check --out -- error if it is a directory

  * parse potions etc in inventories etc

  * do variants for items + entities?

  * do away with image coord space for web app? coords are crazy -- confirmed that negative-Z is north

  * do chunk grid here instead of in web ui? (same func as slime chunks)

  * use boost for filesystem stuff?

  * try with clang; possible to mingw w/ clang?

  * see reddit test1 world -- has this msg (could be result of bad mcedit or similar):
  -- WARNING: Did not find block variant for block(Wooden Double Slab) with blockdata=8 (0x8)

  * join chests for geojson? (pairchest) - would require that we wait to toGeoJSON until we parse all chunks
  -- put ALL chests in a map<x,y,z>; go through list of chests and put PairChest in matching sets and mark all as unprocessed; go thru list and do all, if PairChest mark both as done

  * see if there is interesting info re colors for overview map: http://minecraft.gamepedia.com/Map_item_format

  * option to name output files w/ world name

  * use "solid" info from block info to do something? could it fix the light map?

  * update block xml w/ transparency info - bool or int? or just solid/nonsolid?
  -- use this transparency (or something else? e.g. spawnable flag?) to determine which block is the top block for purposes of the light map
  -- go and look at wiki to see the type of info that is stored per block

  * convert all printf-style stuff to streams

  * find better hsl/hsv to/from rgb funcs; or really, better way to sort colors so that colortest is more useful


  user suggestions

  * tobomori: ability to hide only a block variant (e.g. Tall Grass)
  -- difficult because of fastBlockHideList etc

  todo

  ** cmdline options:
  save set of slices
  save a particular slice
  draw text on slice files (e.g. Y)
  separate logfiles for overworld + nether + unknown
  options to reduce log file detail

  ** maps/instructions to get from point A (e.g. spawn) to biome type X (in blocks but also in landmark form: over 2 seas left at the birch forest etc); same for items and entities

  todo win32/win64 build

  * immediate crash if using -O2? (and now -O1 and -O)
  * leveldb close() issue -- link fails on missing stream stuff
  * leveldb -O2 build issue -- link fails on missing stream stuff
  * leveldb fread_nolock issue -- link fails; forcing msvcrXXX.a crashes on windows
  * log file: change end line to CRLF?

  ** osx build? cross compile tools look kinda horrible; see https://www.macports.org/

  */

// define this to use memcpy instead of manual copy of individual pixel values
// memcpy appears to be approx 1.3% faster for another1 --html-all
#define PIXEL_COPY_MEMCPY

#include <cstdio>
#include <map>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <filesystem>
#include <iostream>
#include <ctime>
#include <string>

#include <leveldb/db.h>
#include <leveldb/env.h>
#include <leveldb/cache.h>
#include <leveldb/decompress_allocator.h>
#include <leveldb/filter_policy.h>

#ifndef _MSC_VER
#include <getopt.h>
#else
#include "getopt/getopt.h"
#endif

#include "define.h"
#include "util.h"
#include "mcpe_viz.h"
#include "nbt.h"
#include "xml.h"
#include "asset.h"


namespace mcpe_viz {

    std::string dirExec;

    Logger logger;
    Logger slogger;

    // todobig -- would be nice for these to be in world class
    double playerPositionImageX = 0.0, playerPositionImageY = 0.0;
    int32_t playerPositionDimensionId = kDimIdOverworld;

    // list of geojson items
    std::vector<std::string> listGeoJSON;

    // palettes
    int32_t palRedBlackGreen[256];

    // info lists (from XML)
    BlockInfo blockInfoList[512];
    ItemInfoList itemInfoList;
    EntityInfoList entityInfoList;
    BiomeInfoList biomeInfoList;
    EnchantmentInfoList enchantmentInfoList;

    IntIntMap mcpcToMcpeBlock;
    IntIntMap mcpeToMcpcBlock;
    IntIntMap mcpcToMcpeItem;
    IntIntMap mcpeToMcpcItem;

    StringIntMap imageFileMap;
    int32_t globalIconImageId = 1;

    PlayerIdToName playerIdToName;

    leveldb::ReadOptions levelDbReadOptions;




    // suggestion from mcpe_sample_setup.cpp
    class NullLogger : public leveldb::Logger {
    public:
        void Logv(const char *, va_list) override {
        }
    };


    // all user options are stored here
    class Control {
    public:
        std::string dirLeveldb;
        std::string fnOutputBase;
        std::string fnCfg;
        std::string fnXml;
        std::string fnLog;
        std::string fnGeoJSON;
        std::string fnHtml;
        std::string fnJs;

        // per-dimension filenames
        std::string fnLayerTop[kDimIdCount];
        std::string fnLayerBiome[kDimIdCount];
        std::string fnLayerHeight[kDimIdCount];
        std::string fnLayerHeightGrayscale[kDimIdCount];
        std::string fnLayerHeightAlpha[kDimIdCount];
        std::string fnLayerBlockLight[kDimIdCount];
        std::string fnLayerSkyLight[kDimIdCount];
        std::string fnLayerSlimeChunks[kDimIdCount];
        std::string fnLayerGrass[kDimIdCount];
        std::string fnLayerShadedRelief[kDimIdCount];
        std::string fnLayerRaw[kDimIdCount][MAX_BLOCK_HEIGHT + 1];

        bool doDetailParseFlag;
        int32_t doMovie;
        int32_t doSlices;
        int32_t doGrid;
        int32_t doHtml;
        int32_t doTiles;
        int32_t doImageBiome;
        int32_t doImageGrass;
        int32_t doImageHeightCol;
        int32_t doImageHeightColGrayscale;
        int32_t doImageHeightColAlpha;
        int32_t doImageLightBlock;
        int32_t doImageLightSky;
        int32_t doImageSlimeChunks;
        int32_t doImageShadedRelief;
        bool autoTileFlag;
        bool noForceGeoJSONFlag;
        bool shortRunFlag;
        bool colorTestFlag;
        bool verboseFlag;
        bool quietFlag;
        int32_t movieX, movieY, movieW, movieH;

        bool doFindImages;
        std::string dirFindImagesIn;
        std::string dirFindImagesOut;

        int32_t heightMode;

        int32_t tileWidth;
        int32_t tileHeight;

        bool fpLogNeedCloseFlag;
        FILE *fpLog;

        // this is the BloomFilterPolicy bits, set to 0 to disable filter
        int32_t leveldbFilter = 10;
        // this is the block_size used by leveldb
        int32_t leveldbBlockSize = 4096;

        Control() {
            init();
        }

        ~Control() {
            if (fpLogNeedCloseFlag) {
                if (fpLog != nullptr) {
                    fclose(fpLog);
                }
            }
        }

        void init() {
            dirLeveldb = "";
            fnXml = "";
            fnOutputBase = "";
            fnLog = "";
            fnGeoJSON = "";
            fnHtml = "";
            fnJs = "";
            doDetailParseFlag = false;

            doMovie = kDoOutputNone;
            doSlices = kDoOutputNone;
            doGrid = kDoOutputNone;
            doHtml = 0;
            doTiles = 0;
            doImageBiome = kDoOutputNone;
            doImageGrass = kDoOutputNone;
            doImageHeightCol = kDoOutputNone;
            doImageHeightColGrayscale = kDoOutputNone;
            doImageHeightColAlpha = kDoOutputNone;
            doImageLightBlock = kDoOutputNone;
            doImageLightSky = kDoOutputNone;
            doImageSlimeChunks = kDoOutputNone;
            doImageShadedRelief = kDoOutputNone;
            noForceGeoJSONFlag = false;

            autoTileFlag = false;
            // todobig - reasonable default? strike a balance between speed/# of files
            tileWidth = 1024;
            tileHeight = 1024;

            doFindImages = false;
            dirFindImagesIn = "";
            dirFindImagesOut = "";

            shortRunFlag = false;
            colorTestFlag = false;
            verboseFlag = false;
            quietFlag = false;
            movieX = movieY = movieW = movieH = 0;
            fpLogNeedCloseFlag = false;
            fpLog = stdout;

            leveldbFilter = 10;
            leveldbBlockSize = 4096;

            // todo - cmdline option for this?
            heightMode = kHeightModeTop;

            for (int32_t did = 0; did < kDimIdCount; did++) {
                fnLayerTop[did] = "";
                fnLayerBiome[did] = "";
                fnLayerHeight[did] = "";
                fnLayerHeightGrayscale[did] = "";
                fnLayerHeightAlpha[did] = "";
                fnLayerBlockLight[did] = "";
                fnLayerSkyLight[did] = "";
                fnLayerSlimeChunks[did] = "";
                fnLayerShadedRelief[did] = "";
                fnLayerGrass[did] = "";
                for (int32_t i = 0; i <= MAX_BLOCK_HEIGHT; i++) {
                    fnLayerRaw[did][i] = "";
                }
            }
        }

        void setupOutput() {
            if (fnLog.compare("-") == 0) {
                fpLog = stdout;
                fpLogNeedCloseFlag = false;
            } else {
                if (fnLog.size() == 0) {
                    fnLog = fnOutputBase + ".log";
                }
                fpLog = fopen(fnLog.c_str(), "w");
                if (fpLog) {
                    fpLogNeedCloseFlag = true;
                } else {
                    fprintf(stderr,
                            "ERROR: Failed to create output log file (%s error=%s (%d)).  Reverting to stdout...\n",
                            fnLog.c_str(), strerror(errno), errno);
                    fpLog = stdout;
                    fpLogNeedCloseFlag = false;
                }
            }

            // setup logger
            logger.setStdout(fpLog);
            logger.setStderr(stderr);

            if (doHtml) {
                fnGeoJSON = fnOutputBase + ".geojson";

                listGeoJSON.clear();

                fnHtml = fnOutputBase + ".html";
                fnJs = fnOutputBase + ".js";
            }
        }

    };

    Control control;


    void makePalettes() {
        // create red-green ramp; red to black and then black to green
        makeHslRamp(palRedBlackGreen, 0, 61, 0.0, 0.0, 0.9, 0.9, 0.8, 0.1);
        makeHslRamp(palRedBlackGreen, 63, MAX_BLOCK_HEIGHT, 0.4, 0.4, 0.9, 0.9, 0.1, 0.8);
        // force 62 (sea level) to gray
        palRedBlackGreen[62] = 0x303030;

        // fill 128..255 with purple (we should never see this color)
        for (int32_t i = (MAX_BLOCK_HEIGHT + 1); i < 256; i++) {
            palRedBlackGreen[i] = kColorDefault;
        }

        // convert palette
        for (int32_t i = 0; i < 256; i++) {
            palRedBlackGreen[i] = htobe32(palRedBlackGreen[i]);
        }
    }



    // todolib - these funcs should be in a class?

    // calculate an offset into mcpe chunk data for block data
    inline int32_t _calcOffsetBlock_LevelDB_v2(int32_t x, int32_t z, int32_t y) {
        return (((x * 16) + z) * (MAX_BLOCK_HEIGHT_127 + 1)) + y;
    }

    // calculate an offset into mcpe chunk data for column data
    inline int32_t _calcOffsetColumn_LevelDB_v2(int32_t x, int32_t z) {
        // NOTE! this is the OPPOSITE of block data (oy)
        return (z * 16) + x;
    }

    inline uint8_t getBlockId_LevelDB_v2(const char *p, int32_t x, int32_t z, int32_t y) {
        return (p[_calcOffsetBlock_LevelDB_v2(x, z, y)] & 0xff);
    }

    uint8_t getBlockData_LevelDB_v2(const char *p, int32_t x, int32_t z, int32_t y) {
        int32_t off = _calcOffsetBlock_LevelDB_v2(x, z, y);
        int32_t off2 = off / 2;
        int32_t mod2 = off % 2;
        int32_t v = p[32768 + off2];
        if (mod2 == 0) {
            return v & 0x0f;
        } else {
            return (v & 0xf0) >> 4;
        }
    }

    // a block opacity value? (e.g. glass is 0xf, water is semi (0xc) and an opaque block is 0x0)
    uint8_t getBlockSkyLight_LevelDB_v2(const char *p, int32_t x, int32_t z, int32_t y) {
        int32_t off = _calcOffsetBlock_LevelDB_v2(x, z, y);
        int32_t off2 = off / 2;
        int32_t mod2 = off % 2;
        int32_t v = p[32768 + 16384 + off2];
        if (mod2 == 0) {
            return v & 0x0f;
        } else {
            return (v & 0xf0) >> 4;
        }
    }

    // block light is light value from torches et al -- super cool looking as an image, but it looks like block light is probably stored in air blocks which are above top block
    uint8_t getBlockBlockLight_LevelDB_v2(const char *p, int32_t x, int32_t z, int32_t y) {
        int32_t off = _calcOffsetBlock_LevelDB_v2(x, z, y);
        int32_t off2 = off / 2;
        int32_t mod2 = off % 2;
        int32_t v = p[32768 + 16384 + 16384 + off2];
        if (mod2 == 0) {
            return v & 0x0f;
        } else {
            return (v & 0xf0) >> 4;
        }
    }

    // height of top *solid* block? (e.g. a glass block will NOT be the top block here)
    uint8_t getColData_Height_LevelDB_v2(const char *buf, int32_t x, int32_t z) {
        int32_t off = _calcOffsetColumn_LevelDB_v2(x, z);
        int8_t v = buf[32768 + 16384 + 16384 + 16384 + off];
        return v;
    }

    // this is 4-bytes: lsb is biome, the high 3-bytes are RGB grass color
    uint32_t getColData_GrassAndBiome_LevelDB_v2(const char *buf, int32_t x, int32_t z) {
        int32_t off = _calcOffsetColumn_LevelDB_v2(x, z) * 4;
        int32_t v;
        memcpy(&v, &buf[32768 + 16384 + 16384 + 16384 + 256 + off], 4);
        return v;
    }


    // calculate an offset into mcpe chunk data for block data
    inline int32_t _calcOffsetBlock_LevelDB_v3(int32_t x, int32_t z, int32_t y) {
        return (((x * 16) + z) * 16) + y;
    }

    inline uint8_t getBlockId_LevelDB_v3(const char *p, int32_t x, int32_t z, int32_t y) {
        return (p[_calcOffsetBlock_LevelDB_v3(x, z, y) + 1] & 0xff);
    }

    uint8_t getBlockData_LevelDB_v3(const char *p, size_t plen, int32_t x, int32_t z, int32_t y) {
        int32_t off = _calcOffsetBlock_LevelDB_v3(x, z, y);
        int32_t off2 = off / 2;
        int32_t mod2 = off % 2;
        // todonow - temp test to find bug
        size_t tmp_offset = (16 * 16 * 16) + 1 + off2;
        if (tmp_offset >= plen) {
            if (control.verboseFlag) {
                slogger.msg(kLogError, "getBlockData_LevelDB_v3 get data out of bounds! (%d >= %d) (%d %d %d)\n",
                            (int32_t) tmp_offset, (int32_t) plen, x, z, y);
            }
            return 0;
        }
        int32_t v = p[tmp_offset];
        //int32_t v = p[(16*16*16) + 1 + off2];
        if (mod2 == 0) {
            return v & 0x0f;
        } else {
            return (v & 0xf0) >> 4;
        }
    }

    // todozooz - this is getting crazy
    uint8_t getBlockData_LevelDB_v3__fake_v7(const int16_t *p, size_t plen, int32_t x, int32_t z, int32_t y) {
        int32_t off = _calcOffsetBlock_LevelDB_v3(x, z, y);
        int32_t off2 = off / 2;
        int32_t mod2 = off % 2;
        // todonow - temp test to find bug
        size_t tmp_offset = (16 * 16 * 16) + 1 + off2;
        if (tmp_offset >= plen) {
            if (control.verboseFlag) {
                slogger.msg(kLogError, "getBlockData_LevelDB_v3 get data out of bounds! (%d >= %d) (%d %d %d)\n",
                            (int32_t) tmp_offset, (int32_t) plen, x, z, y);
            }
            return 0;
        }
        int32_t v = p[tmp_offset];
        //int32_t v = p[(16*16*16) + 1 + off2];
        if (mod2 == 0) {
            return v & 0x0f;
        } else {
            return (v & 0xf0) >> 4;
        }
    }

    // a block opacity value? (e.g. glass is 0xf, water is semi (0xc) and an opaque block is 0x0)
    uint8_t getBlockSkyLight_LevelDB_v3(const char *p, size_t plen, int32_t x, int32_t z, int32_t y) {
        int32_t off = _calcOffsetBlock_LevelDB_v3(x, z, y);
        int32_t off2 = off / 2;
        int32_t mod2 = off % 2;
        // todonow - temp test to find bug
        size_t tmp_offset = (16 * 16 * 16) + 1 + (16 * 16 * 8) + off2;
        if (tmp_offset >= plen) {
            if (control.verboseFlag) {
                slogger.msg(kLogError, "getBlockSkyLight_LevelDB_v3 get data out of bounds! (%d >= %d) (%d %d %d)\n",
                            (int32_t) tmp_offset, (int32_t) plen, x, z, y);
            }
            return 0;
        }
        int32_t v = p[tmp_offset];
        // int32_t v = p[(16*16*16) + 1 + (16*16*8) + off2];
        if (mod2 == 0) {
            return v & 0x0f;
        } else {
            return (v & 0xf0) >> 4;
        }
    }

    // block light is light value from torches et al -- super cool looking as an image, but it looks like block light is probably stored in air blocks which are above top block
    uint8_t getBlockBlockLight_LevelDB_v3(const char *p, size_t plen, int32_t x, int32_t z, int32_t y) {
        int32_t off = _calcOffsetBlock_LevelDB_v3(x, z, y);
        int32_t off2 = off / 2;
        int32_t mod2 = off % 2;
        // todonow - temp test to find bug
        size_t tmp_offset = (16 * 16 * 16) + 1 + (16 * 16 * 8) + (16 * 16 * 8) + off2;
        if (tmp_offset >= plen) {
            if (control.verboseFlag) {
                slogger.msg(kLogError, "getBlockBlockLight_LevelDB_v3 get data out of bounds! (%d >= %d) (%d %d %d)\n",
                            (int32_t) tmp_offset, (int32_t) plen, x, z, y);
            }
            return 0;
        }
        int32_t v = p[tmp_offset];
        //int32_t v = p[(16*16*16) + 1 + (16*16*8) + (16*16*8) + off2];
        if (mod2 == 0) {
            return v & 0x0f;
        } else {
            return (v & 0xf0) >> 4;
        }
    }


    // calculate an offset into mcpe chunk data for column data
    inline int32_t _calcOffsetColumn_LevelDB_v3(int32_t x, int32_t z) {
        // NOTE! this is the OPPOSITE of block data (oy)
        return (z * 16) + x;
    }

    // height appears to be stored as a 2-byte int
    // height of top *solid* block? (e.g. a glass block will NOT be the top block here)
    uint8_t getColData_Height_LevelDB_v3(const char *buf, int32_t x, int32_t z) {
        int32_t off = _calcOffsetColumn_LevelDB_v3(x, z) * 2;
        int8_t v = buf[off];
        return v;
    }

    // this is 3-bytes: lsb is biome?, the high 2-bytes are RGB grass color?
    uint32_t getColData_GrassAndBiome_LevelDB_v3(const char *buf, int32_t buflen, int32_t x, int32_t z) {
        // format appears to be:
        // 16x16 of 2-byte ints for HEIGHT OF TOP BLOCK
        // 16x16 of 4-byte ints for BIOME and GRASS COLOR
        // todo -- there was a bug in early 0.17 that really messed this data up
        // tood -- grass colors are pretty weird (some are 01 01 01)
        // as of, v0.17.01 we'll just roll with it and adjust as necessary

        //int32_t off = _calcOffsetColumn_LevelDB_v3(x,z) * 4;
        int32_t off = _calcOffsetColumn_LevelDB_v3(x, z);
        int32_t v = 0;

        // HACK! to work around MCPE bug (biome data is not complete in this record as of v0.17.01
        if ((512 + off + 1) <= buflen) {
            // memcpy(&v,&buf[512 + off],1);
            v = buf[512 + off];
        } else {
            // nothing - this is deals with the bug in early 0.17
        }

        return v;
    }



    // todobig - this (XXX_v3_fullchunk) needs to be cleaner/simpler

    // calculate an offset into mcpe chunk data for block data
    inline int32_t _calcOffsetBlock_LevelDB_v3_fullchunk(int32_t x, int32_t z, int32_t y) {
        return (((x * 16) + z) * MAX_BLOCK_HEIGHT) + y;
    }

    inline uint8_t getData_LevelDB_v3_fullchunk(const char *p, int32_t x, int32_t z, int32_t y) {
        return p[_calcOffsetBlock_LevelDB_v3_fullchunk(x, z, y)];
    }


    inline uint8_t
    getBlockId_LevelDB_v7(const char *p, int blocksPerWord, int bitsPerBlock, int32_t x, int32_t z, int32_t y) {
        //int bitstart = ( (((x*16) + z) * 16) + y ) * bitsPerBlock;
        // int bitstart = ( (((y*16) + x) * 16) + z ) * bitsPerBlock;

        int blockPos = (((x * 16) + z) * 16) + y;
        // we find which 4-byte word we want
        int wordStart = blockPos / blocksPerWord;
        // we find the bit offset within that 4-byte word
        int bitOffset = (blockPos % blocksPerWord) * bitsPerBlock;
        int bitStart = wordStart * 4 * 8 + bitOffset;
        return getBitsFromBytes(p, bitStart, bitsPerBlock);
    }


    // todomajor -- see tomcc gist re multiple storages in ONE cubick chunk in version == 8

    inline int32_t
    setupBlockVars_v7(const char *cdata, int32_t &blocksPerWord, int32_t &bitsPerBlock, bool &paddingFlag,
                      int32_t &offsetBlockInfoList, int32_t &extraOffset) {

        int32_t v = -1;

        if (cdata[0] == 0x01) {
            v = cdata[1];
            extraOffset = 0;
        } else {
            // this is version 8+, cdata[1] contains the number of storage groups in this cubic chunk (can be more than 1)
            v = cdata[2];
            extraOffset = 1;
        }

        switch (v) {
            case 0x02:
                blocksPerWord = 32;
                bitsPerBlock = 1;
                offsetBlockInfoList = 512;
                break;
            case 0x04:
                blocksPerWord = 16;
                bitsPerBlock = 2;
                offsetBlockInfoList = 1024;
                break;
            case 0x06:
                blocksPerWord = 10;
                bitsPerBlock = 3;
                paddingFlag = true;
                offsetBlockInfoList = 1640;
                break;
            case 0x08:
                blocksPerWord = 8;
                bitsPerBlock = 4;
                offsetBlockInfoList = 2048;
                break;
            case 0x0a:
                blocksPerWord = 6;
                bitsPerBlock = 5;
                paddingFlag = true;
                offsetBlockInfoList = 2732;
                break;
            case 0x0c:
                blocksPerWord = 5;
                bitsPerBlock = 6;
                paddingFlag = true;
                offsetBlockInfoList = 3280;
                break;
            case 0x10:
                blocksPerWord = 4;
                bitsPerBlock = 8;
                offsetBlockInfoList = (4096 / blocksPerWord) * 4;
                break;
            case 0x20:
                blocksPerWord = 2;
                bitsPerBlock = 16;
                offsetBlockInfoList = (4096 / blocksPerWord) * 4;
                break;
            default:
                slogger.msg(kLogError, "Unknown chunk cdata[1] value = %d\n", (int) v);
                logger.msg(kLogError, "Unknown chunk cdata[1] value = %d\n", (int) v);
                return -1;
        }

        //    logger.msg(kLogInfo, "setupBlockVars_v7 v=%d bpw=%d bpb=%d pf=%d ob=%d\n", v, blocksPerWord, bitsPerBlock, (int)paddingFlag, offsetBlockInfoList);
        return 0;
    }

    int32_t convertChunkV7toV3(const char *cdata, size_t cdata_size, int16_t *emuchunk) {
        // we have a v7 chunk and we want to unpack it into a v3-like chunk
        // determine location of chunk palette

        // some details here: https://gist.github.com/Tomcc/a96af509e275b1af483b25c543cfbf37

        int32_t blocksPerWord = -1;
        int32_t bitsPerBlock = -1;
        bool paddingFlag = false;
        int32_t offsetBlockInfoList = -1;
        int32_t extraOffset = -1;

        memset(emuchunk, 0, NUM_BYTES_CHUNK_V3 * sizeof(int16_t));

        if (setupBlockVars_v7(cdata, blocksPerWord, bitsPerBlock, paddingFlag, offsetBlockInfoList, extraOffset) != 0) {
            return -1;
        }

        // read chunk palette and associate old-school block id's
        MyNbtTagList tagList;
        int xoff = offsetBlockInfoList + 6 + extraOffset;
        parseNbtQuiet(&cdata[xoff], cdata_size - xoff, cdata[offsetBlockInfoList + 3], tagList);

        std::vector<int32_t> chunkBlockPalette_BlockId(tagList.size());
        std::vector<int32_t> chunkBlockPalette_BlockData(tagList.size());

        for (size_t i = 0; i < tagList.size(); i++) {
            // check tagList
            if (tagList[i].second->get_type() == nbt::tag_type::Compound) {
                nbt::tag_compound tc = tagList[i].second->as<nbt::tag_compound>();

                bool processedFlag = false;
                if (tc.has_key("name", nbt::tag_type::String)) {
                    std::string bname = tc["name"].as<nbt::tag_string>().get();
                    int bdata = 0;
                    if (tc.has_key("val", nbt::tag_type::Short)) {
                        bdata = tc["val"].as<nbt::tag_short>().get();
                    }
                    int32_t blockId, blockData;
                    if (getBlockByUname(bname, blockId, blockData) == 0) {
                        chunkBlockPalette_BlockId[i] = blockId;
                        // todonow - correct?
                        chunkBlockPalette_BlockData[i] = bdata;
                    } else {
                        logger.msg(kLogWarning, "Did not find block uname '%s' in XML file\n", bname.c_str());
                        // todonow - reasonable?
                        chunkBlockPalette_BlockId[i] = 0;
                        chunkBlockPalette_BlockData[i] = 0;
                    }
                    processedFlag = true;
                }
                if (!processedFlag) {
                    slogger.msg(kLogError, "(Safe) Did not find 'name' tag in a chunk palette! (i=%d) (len=%d)\n",
                                (int) i, (int) tagList.size());
                    //todozooz - dump tc to screen log
                }
            } else {
                logger.msg(kLogWarning, "Unexpected NBT format in _do_chunk_v7\n");
            }
        }

        //todozooz -- new 16-bit block-id's (instead of 8-bit) are a BIG issue - this needs attention here
        // iterate over chunk space
        uint8_t paletteBlockId, blockData;
        int32_t blockId;
        for (int32_t cy = 0; cy < 16; cy++) {
            for (int32_t cx = 0; cx < 16; cx++) {
                for (int32_t cz = 0; cz < 16; cz++) {
                    paletteBlockId = getBlockId_LevelDB_v7(&cdata[2 + extraOffset], blocksPerWord, bitsPerBlock, cx, cz,
                                                           cy);

                    // look up blockId
                    //todonow error checking
                    if (paletteBlockId < chunkBlockPalette_BlockId.size()) {
                        blockId = chunkBlockPalette_BlockId[paletteBlockId];
                        blockData = chunkBlockPalette_BlockData[paletteBlockId];
                    } else {
                        blockId = 0;
                        blockData = 0;
                        logger.msg(kLogWarning, "Found chunk palette id out of range %d (size=%d)\n", paletteBlockId,
                                   (int) chunkBlockPalette_BlockId.size());
                    }

                    int32_t bdoff = _calcOffsetBlock_LevelDB_v3(cx, cz, cy);

                    emuchunk[bdoff + 1] = blockId;

                    // put block data
                    int32_t bdoff2 = bdoff / 2;
                    int32_t bdmod2 = bdoff % 2;
                    // todonow - temp test to find bug
                    size_t tmp_offset = (16 * 16 * 16) + 1 + bdoff2;
                    if (bdmod2 == 0) {
                        emuchunk[tmp_offset] |= (blockData & 0x0f);
                    } else {
                        emuchunk[tmp_offset] |= (blockData & 0x0f) << 4;
                    }
                }
            }
        }
        return 0;
    }



    // todolib - move to util?

    int32_t myParseInt32(const char *p, int32_t startByte) {
        int32_t ret;
        memcpy(&ret, &p[startByte], 4);
        return ret;
    }

    int8_t myParseInt8(const char *p, int32_t startByte) {
        return (p[startByte] & 0xff);
    }


    bool has_key(const ItemInfoList &m, int32_t k) {
        return m.find(k) != m.end();
    }

    bool has_key(const EntityInfoList &m, int32_t k) {
        return m.find(k) != m.end();
    }

    bool has_key(const BiomeInfoList &m, int32_t k) {
        return m.find(k) != m.end();
    }

    bool has_key(const EnchantmentInfoList &m, int32_t k) {
        return m.find(k) != m.end();
    }

    bool has_key(const IntIntMap &m, int32_t k) {
        return m.find(k) != m.end();
    }

    bool has_key(const StringIntMap &m, const std::string &k) {
        return m.find(k) != m.end();
    }


    int32_t findEntityByUname(const EntityInfoList &m, std::string &un) {
        // convert search key to lower case
        std::string uname = un;
        std::transform(uname.begin(), uname.end(), uname.begin(), ::tolower);
        for (const auto &it : m) {
            for (const auto &u : it.second->unameList) {
                if (u == uname) {
                    return it.first;
                }
            }
        }
        return -1;
    }

    int32_t findIdByItemName(std::string &un) {
        std::string uname = un;
        std::transform(uname.begin(), uname.end(), uname.begin(), ::tolower);
        for (const auto &it : itemInfoList) {
            for (const auto &u : it.second->unameList) {
                if (u == uname) {
                    return it.first;
                }
            }
        }
        return -1;
    }

    int32_t findIdByBlockName(std::string &un) {
        std::string uname = un;
        std::transform(uname.begin(), uname.end(), uname.begin(), ::tolower);
        for (const auto &it : blockInfoList) {
            for (const auto &u : it.unameList) {
                if (u == uname) {
                    return it.id;
                }
            }
        }
        return -1;
    }


    // todobig - it would be nice to do something like this, but unique_ptr stands in the way...
#if 0
                                                                                                                            BlockInfo* getBlockInfo(int32_t id, int32_t blockData) {
    if ( blockInfoList[id].isValid() ) {
      if ( blockInfoList[id].hasVariants() ) {
        for ( auto& iter : blockInfoList[id].variantList ) {
          if ( iter->blockdata == blockData ) {
            // found variant
            return iter;
          }
        }
        // todo err
        slogger.msg(kLogWarning, "getBlockInfo failed to find variant id=%d blockdata=%d\n", id, blockData);
        return NULL;
      } else {
        return &blockInfoList[id];
      }
    }

    slogger.msg(kLogWarning, "getBlockInfoList failed to find id=%d blockdata=%d\n", id, blockData);
    return NULL;
  }
#endif

    int32_t getBlockByUname(const std::string &un, int32_t &blockId, int32_t &blockData) {
        // convert search key to lower case
        std::string uname = un;
        std::transform(uname.begin(), uname.end(), uname.begin(), ::tolower);
        for (const auto &it : blockInfoList) {
            for (const auto &u : it.unameList) {
                if (u == uname) {
                    blockId = it.id;
                    blockData = 0;
                    return 0;
                }
            }

            for (const auto &itbv : it.variantList) {
                for (const auto &u : itbv->unameList) {
                    if (u == uname) {
                        blockId = it.id;
                        blockData = itbv->blockdata;
                        return 0;
                    }
                }
            }
        }

        // force to "air"
        blockId = 0;
        blockData = 0;
        slogger.msg(kLogWarning, "getBlockByUname failed to find uname=%s\n", uname.c_str());
        return -1;
    }

    std::string getBlockName(int32_t id, int32_t blockdata) {
        if (blockInfoList[id].isValid()) {
            if (blockInfoList[id].hasVariants()) {
                for (const auto &iter : blockInfoList[id].variantList) {
                    if (iter->blockdata == blockdata) {
                        // found variant
                        return iter->name;
                    }
                }
                // warn about missing variant, but return parent's name
                slogger.msg(kLogWarning, "getBlockName failed to find variant id=%d (0x%x) blockdata=%d (0x%x)\n", id,
                            id, blockdata, blockdata);
                return blockInfoList[id].name;
            } else {
                return blockInfoList[id].name;
            }
        }

        slogger.msg(kLogWarning, "getBlockName failed to find id=%d (0x%x) blockdata=%d (0x%x)\n", id, id, blockdata,
                    blockdata);
        char tmpstring[256];
        sprintf(tmpstring, "(Unknown-block-id-%d-data-%d)", id, blockdata);
        return std::string(tmpstring);
    }

    std::string getItemName(int32_t id, int32_t extraData, bool nameBasedFlag) {
        if (has_key(itemInfoList, id)) {
            if (itemInfoList[id]->hasVariants()) {
                for (const auto &iter : itemInfoList[id]->variantList) {
                    if (iter->extraData == extraData) {
                        // found variant
                        return iter->name;
                    }
                }
                // warn about missing variant, but return parent's name
                slogger.msg(kLogWarning, "getItemName failed to find variant id=%d (0x%x) extradata=%d (0x%x) nbf=%d\n",
                            id, id, extraData, extraData, (int) nameBasedFlag);
                return itemInfoList[id]->name;
            } else {
                return itemInfoList[id]->name;
            }
        }

        slogger.msg(kLogWarning, "getItemName failed to find id=%d (0x%x) extradata=%d (0x%x) nbf=%d\n", id, id,
                    extraData, extraData, (int) nameBasedFlag);
        char tmpstring[256];
        sprintf(tmpstring, "(Unknown-item-id-%d-data-%d)", id, extraData);
        return std::string(tmpstring);
    }


    std::string getBiomeName(int32_t idv) {
        if (has_key(biomeInfoList, idv)) {
            return biomeInfoList[idv]->name;
        }
        char s[256];
        sprintf(s, "ERROR: Failed to find biome id (%d)", idv);
        return std::string(s);
    }


    // todolib - better name for this
    class CheckSpawn {
    public:
        int32_t x, z, distance;

        CheckSpawn(int32_t nx, int32_t nz, int32_t ndist) {
            x = nx;
            z = nz;
            distance = ndist;
        }

        bool contains(int32_t tx, int32_t tz) {
            // todo - faster distance calc? do simple distance tests before sqrt part?
            double dx = x - tx;
            double dz = z - tz;
            double d = sqrt(dx * dx + dz * dz);
            if (d <= distance) {
                return true;
            }
            return false;
        }
    };

    typedef std::vector<std::unique_ptr<CheckSpawn> > CheckSpawnList;


    // todolib - better name for this
    class Schematic {
    public:
        int32_t x1, y1, z1;
        int32_t x2, y2, z2;
        std::string fn;

        Schematic(int32_t tx1, int32_t ty1, int32_t tz1,
                  int32_t tx2, int32_t ty2, int32_t tz2,
                  const char *fnSchematic) {
            // we order the coordinates here so it's easier later
            x1 = std::min(tx1, tx2);
            x2 = std::max(tx1, tx2);
            y1 = std::min(ty1, ty2);
            y2 = std::max(ty1, ty2);
            z1 = std::min(tz1, tz2);
            z2 = std::max(tz1, tz2);
            fn = fnSchematic;
        }

        std::string toString() {
            char tmpstring[2048];
            sprintf(tmpstring, "p1=%d,%d,%d p2=%d,%d,%d fn=%s", x1, y1, z1, x2, y2, z2, fn.c_str()
            );
            return std::string(tmpstring);
        }

    };

    typedef std::vector<std::unique_ptr<Schematic> > SchematicList;


    class ChunkData {
    public:
    };


    // todobig - perhaps this is silly (storing all this info per-chunk)
    class ChunkData_LevelDB : public ChunkData {
    public:
        // todobig - move to private?
        int32_t chunkX, chunkZ;
        //todozooz -- new 16-bit block-id's (instead of 8-bit) are a BIG issue - this needs attention here
        uint16_t blocks[16][16];
        uint8_t data[16][16];
        uint32_t grassAndBiome[16][16];
        uint8_t topBlockY[16][16];
        uint8_t heightCol[16][16];
        uint8_t topLight[16][16];
        bool checkSpawnFlag;
        int32_t chunkFormatVersion;

        // we parse the block (et al) data in a chunk from leveldb
        ChunkData_LevelDB() {
            // clear the data we track
            memset(blocks, 0, sizeof(blocks));

            // todobig - clears are redundant?
            memset(data, 0, sizeof(data));
            //memset(grassAndBiome, 0, 16*16*sizeof(uint32_t));
            memset(topBlockY, 0, sizeof(topBlockY));
            //memset(heightCol,0, 16*16*sizeof(uint8_t));
            memset(topLight, 0, sizeof(topLight));

            checkSpawnFlag = false;
            chunkFormatVersion = -1;
        }

        /*
      obsolete_ChunkData_LevelDB(int32_t chunkFormatVersion, int32_t tchunkX, int32_t tchunkY, int32_t tchunkZ, const char* cdata,
      int32_t dimensionId, const std::string& dimName,
      Histogram& histogramGlobalBlock, Histogram& histogramGlobalBiome,
      const bool* fastBlockHideList, const bool* fastBlockForceTopList,
      const bool* fastBlockToGeoJSON,
      const CheckSpawnList& listCheckSpawn) {

      switch ( chunkFormatVersion ) {
      case 2:
      // pre-v0.17
      _do_chunk_v2 ( tchunkX, tchunkY, tchunkZ, cdata,
      dimensionId, dimName,
      histogramGlobalBlock, histogramGlobalBiome,
      fastBlockHideList, fastBlockForceTopList,
      fastBlockToGeoJSON,
      listCheckSpawn );
      return;
      case 3:
      // 0.17 and after?
      _do_chunk_v3 ( tchunkX, tchunkY, tchunkZ, cdata,
      dimensionId, dimName,
      histogramGlobalBlock, histogramGlobalBiome,
      fastBlockHideList, fastBlockForceTopList,
      fastBlockToGeoJSON,
      listCheckSpawn );
      return;
      }

      slogger.msg(kLogError, "Unknown chunk format version (%d).  Cannot add chunk.\n", chunkFormatVersion);
      return;
      }
    */

        int32_t _do_chunk_v2(int32_t tchunkX, int32_t tchunkZ, const char *cdata,
                             int32_t dimensionId, const std::string &dimName,
                             Histogram &histogramGlobalBlock, Histogram &histogramGlobalBiome,
                             const bool *fastBlockHideList, const bool *fastBlockForceTopList,
                             const bool *fastBlockToGeoJSON,
                             const CheckSpawnList &listCheckSpawn) {
            chunkX = tchunkX;
            chunkZ = tchunkZ;
            chunkFormatVersion = 2;

            int16_t histogramBlock[512];
            int16_t histogramBiome[256];
            memset(histogramBlock, 0, sizeof(histogramBlock));
            memset(histogramBiome, 0, sizeof(histogramBiome));

            // see if we need to check any columns in this chunk for spawnable
            int32_t wx = chunkX * 16;
            int32_t wz = chunkZ * 16;
            for (const auto &it : listCheckSpawn) {
                if (it->contains(wx, wz)) {
                    checkSpawnFlag = true;
                    break;
                } else if (it->contains(wx, wz + 15)) {
                    checkSpawnFlag = true;
                    break;
                } else if (it->contains(wx + 15, wz + 15)) {
                    checkSpawnFlag = true;
                    break;
                } else if (it->contains(wx + 15, wz)) {
                    checkSpawnFlag = true;
                    break;
                }
            }

            // iterate over chunk space
            uint8_t blockId, biomeId;
            for (int32_t cy = MAX_BLOCK_HEIGHT_127; cy >= 0; cy--) {
                for (int32_t cx = 0; cx < 16; cx++) {
                    for (int32_t cz = 0; cz < 16; cz++) {
                        blockId = getBlockId_LevelDB_v2(cdata, cx, cz, cy);
                        histogramBlock[blockId]++;
                        histogramGlobalBlock.add(blockId);

                        // todobig - handle block variant?
                        if (fastBlockToGeoJSON[blockId]) {
                            double ix, iy;
                            char tmpstring[512];
                            worldPointToGeoJSONPoint(dimensionId, chunkX * 16 + cx, chunkZ * 16 + cz, ix, iy);
                            sprintf(tmpstring, ""
                                               "\"Name\": \"%s\", "
                                               "\"Block\": true, "
                                               "\"Dimension\": \"%d\", "
                                               "\"Pos\": [%d, %d, %d]"
                                               "} }", blockInfoList[blockId].name.c_str(), dimensionId,
                                    chunkX * 16 + cx, cy, chunkZ * 16 + cz
                            );
                            std::string json = ""
                                               + makeGeojsonHeader(ix, iy)
                                               + tmpstring;
                            listGeoJSON.push_back(json);
                        }

                        // check spawnable -- cannot check spawn at 0 or MAX_BLOCK_HEIGHT because we need above/below blocks
                        if (checkSpawnFlag && (cy > 0 && cy < MAX_BLOCK_HEIGHT_127)) {
                            bool continueCheckSpawnFlag = false;
                            for (const auto &it : listCheckSpawn) {
                                if (it->contains(wx + cx, wz + cz)) {
                                    continueCheckSpawnFlag = true;
                                    break;
                                }
                            }
                            if (continueCheckSpawnFlag) {

                                // note: rules adapted from: http://minecraft.gamepedia.com/Spawn

                                // todobig - is this missing some spawnable blocks?

                                // "the spawning block itself must be non-opaque and non-liquid"
                                // we add: non-solid
                                if (!blockInfoList[blockId].isOpaque() &&
                                    !blockInfoList[blockId].isLiquid() &&
                                    !blockInfoList[blockId].isSolid()) {

                                    // "the block directly above it must be non-opaque"

                                    uint8_t aboveBlockId = getBlockId_LevelDB_v2(cdata, cx, cz, cy + 1);
                                    if (!blockInfoList[aboveBlockId].isOpaque()) {

                                        // "the block directly below it must have a solid top surface (opaque, upside down slabs / stairs and others)"
                                        // "the block directly below it may not be bedrock or barrier" -- take care of with 'spawnable'

                                        uint8_t belowBlockId = getBlockId_LevelDB_v2(cdata, cx, cz, cy - 1);
                                        uint8_t belowBlockData = getBlockData_LevelDB_v2(cdata, cx, cz, cy - 1);

                                        //if ( blockInfoList[belowBlockId].isOpaque() && blockInfoList[belowBlockId].isSpawnable(belowBlockData) ) {
                                        if (blockInfoList[belowBlockId].isSpawnable(belowBlockData)) {

                                            // check the light level
                                            uint8_t bl = getBlockBlockLight_LevelDB_v2(cdata, cx, cz, cy);
                                            if (bl <= 7) {
                                                // spwawnable! add it to the list
                                                double ix, iy;
                                                char tmpstring[512];
                                                worldPointToGeoJSONPoint(dimensionId, chunkX * 16 + cx,
                                                                         chunkZ * 16 + cz, ix, iy);
                                                sprintf(tmpstring, ""
                                                                   "\"Spawnable\":true,"
                                                                   "\"Name\":\"Spawnable\","
                                                                   "\"LightLevel\":\"%d\","
                                                                   "\"Dimension\":\"%d\","
                                                                   "\"Pos\":[%d,%d,%d]"
                                                                   "}}", (int) bl, dimensionId, chunkX * 16 + cx, cy,
                                                        chunkZ * 16 + cz
                                                );
                                                std::string json = ""
                                                                   + makeGeojsonHeader(ix, iy)
                                                                   + tmpstring;
                                                listGeoJSON.push_back(json);
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        // todo - check for isSolid?

                        if (blockId != 0) {  // current block is NOT air
                            if ((blocks[cx][cz] == 0 &&  // top block is not already set
                                 !fastBlockHideList[blockId]) ||
                                fastBlockForceTopList[blockId]) {

                                blocks[cx][cz] = blockId;
                                data[cx][cz] = getBlockData_LevelDB_v2(cdata, cx, cz, cy);
                                topBlockY[cx][cz] = cy;

#if 1
                                // todo - we are getting the block light ABOVE this block (correct?)
                                // todo - this will break if we are using force-top stuff
                                int32_t cy2 = cy;
                                if (blockInfoList[blockId].isSolid()) {
                                    // move to block above this block
                                    cy2++;
                                    if (cy2 > MAX_BLOCK_HEIGHT_127) { cy2 = MAX_BLOCK_HEIGHT_127; }
                                } else {
                                    // if not solid, don't adjust
                                }
                                uint8_t sl = getBlockSkyLight_LevelDB_v2(cdata, cx, cz, cy2);
                                uint8_t bl = getBlockBlockLight_LevelDB_v2(cdata, cx, cz, cy2);
                                // we combine the light nibbles into a byte
                                topLight[cx][cz] = (sl << 4) | bl;
#endif
                            }
                        }
                    }
                }
            }

            // get per-column data
            for (int32_t cx = 0; cx < 16; cx++) {
                for (int32_t cz = 0; cz < 16; cz++) {
                    heightCol[cx][cz] = getColData_Height_LevelDB_v2(cdata, cx, cz);
                    grassAndBiome[cx][cz] = getColData_GrassAndBiome_LevelDB_v2(cdata, cx, cz);

                    biomeId = (uint8_t) (grassAndBiome[cx][cz] & 0xFF);
                    histogramBiome[biomeId]++;
                    histogramGlobalBiome.add(biomeId);

#if 0
                                                                                                                                            // todo - testing idea about lighting - get lighting from top solid block - result is part good, part crazy
          int32_t ty = heightCol[cx][cz] + 1;
          if ( ty > MAX_BLOCK_HEIGHT_127 ) { ty=MAX_BLOCK_HEIGHT_127; }
          uint8_t sl = getBlockSkyLight_LevelDB_v2(cdata, cx,cz,ty);
          uint8_t bl = getBlockBlockLight_LevelDB_v2(cdata, cx,cz,ty);
          topLight[cx][cz] = (sl << 4) | bl;
#endif
                }
            }

            if (control.quietFlag) {
                return 0;
            }

            // print chunk info
            logger.msg(kLogInfo1, "Top Blocks (block-id:block-data:biome-id):\n");
            // note the different use of cx/cz here
            uint32_t rawData;
            for (int32_t cz = 0; cz < 16; cz++) {
                for (int32_t cx = 0; cx < 16; cx++) {
                    rawData = grassAndBiome[cx][cz];
                    biomeId = (uint8_t) (rawData & 0xFF);
                    logger.msg(kLogInfo1, "%03x:%x:%02x ", (int) blocks[cx][cz], (int) data[cx][cz], (int) biomeId);
                }
                logger.msg(kLogInfo1, "\n");
            }
            logger.msg(kLogInfo1, "Block Histogram:\n");
            for (int32_t i = 0; i < 512; i++) {
                if (histogramBlock[i] > 0) {
                    logger.msg(kLogInfo1, "%s-hg: %02x: %6d (%s)\n", dimName.c_str(), i, histogramBlock[i],
                               blockInfoList[i].name.c_str());
                }
            }
            logger.msg(kLogInfo1, "Biome Histogram:\n");
            for (int32_t i = 0; i < 256; i++) {
                if (histogramBiome[i] > 0) {
                    std::string biomeName(getBiomeName(i));
                    logger.msg(kLogInfo1, "%s-hg-biome: %02x: %6d (%s)\n", dimName.c_str(), i, histogramBiome[i],
                               biomeName.c_str());
                }
            }
            logger.msg(kLogInfo1, "Block Light (skylight:blocklight:heightcol):\n");
            for (int32_t cz = 0; cz < 16; cz++) {
                for (int32_t cx = 0; cx < 16; cx++) {
                    logger.msg(kLogInfo1, "%x:%x:%02x ", (int) ((topLight[cx][cz] >> 4) & 0xf),
                               (int) (topLight[cx][cz] & 0xf), (int) heightCol[cx][cz]);
                }
                logger.msg(kLogInfo1, "\n");
            }

            return 0;
        }


        int32_t _do_chunk_v3(int32_t tchunkX, int32_t tchunkY, int32_t tchunkZ, const char *cdata, size_t cdata_size,
                             int32_t dimensionId, const std::string &dimName,
                             Histogram &histogramGlobalBlock,
                             const bool *fastBlockHideList, const bool *fastBlockForceTopList,
                             const bool *fastBlockToGeoJSON,
                             const CheckSpawnList &listCheckSpawn) {
            chunkX = tchunkX;
            int32_t chunkY = tchunkY;
            chunkZ = tchunkZ;
            chunkFormatVersion = 3;

            // todonow todostopper - this is problematic for cubic chunks
            int16_t histogramBlock[512];
            int16_t histogramBiome[256];
            memset(histogramBlock, 0, sizeof(histogramBlock));
            memset(histogramBiome, 0, sizeof(histogramBiome));

            // see if we need to check any columns in this chunk for spawnable
            int32_t wx = chunkX * 16;
            int32_t wz = chunkZ * 16;
            for (const auto &it : listCheckSpawn) {
                if (it->contains(wx, wz)) {
                    checkSpawnFlag = true;
                    break;
                } else if (it->contains(wx, wz + 15)) {
                    checkSpawnFlag = true;
                    break;
                } else if (it->contains(wx + 15, wz + 15)) {
                    checkSpawnFlag = true;
                    break;
                } else if (it->contains(wx + 15, wz)) {
                    checkSpawnFlag = true;
                    break;
                }
            }

            // iterate over chunk space
            uint8_t blockId, biomeId;
            for (int32_t cy = 0; cy < 16; cy++) {
                for (int32_t cx = 0; cx < 16; cx++) {
                    for (int32_t cz = 0; cz < 16; cz++) {
                        blockId = getBlockId_LevelDB_v3(cdata, cx, cz, cy);
                        histogramBlock[blockId]++;
                        histogramGlobalBlock.add(blockId);

                        // todobig - handle block variant?
                        if (fastBlockToGeoJSON[blockId]) {
                            double ix, iy;
                            char tmpstring[512];
                            worldPointToGeoJSONPoint(dimensionId, chunkX * 16 + cx, chunkZ * 16 + cz, ix, iy);
                            sprintf(tmpstring, ""
                                               "\"Name\": \"%s\", "
                                               "\"Block\": true, "
                                               "\"Dimension\": \"%d\", "
                                               "\"Pos\": [%d, %d, %d]"
                                               "} }", blockInfoList[blockId].name.c_str(), dimensionId,
                                    chunkX * 16 + cx, chunkY * 16 + cy, chunkZ * 16 + cz
                            );
                            std::string json = ""
                                               + makeGeojsonHeader(ix, iy)
                                               + tmpstring;
                            listGeoJSON.push_back(json);
                        }

                        // note: we check spawnable later

                        // todo - check for isSolid?

                        int32_t realy = chunkY * 16 + cy;
                        if (blockId != 0) {  // current block is NOT air
                            // todonow - this will break forcetop!
                            if ((realy >= topBlockY[cx][cz] &&
                                 !fastBlockForceTopList[blocks[cx][cz]] &&
                                 // blocks[cx][cz] == 0 &&  // top block is not already set
                                 !fastBlockHideList[blockId]) ||
                                fastBlockForceTopList[blockId]) {

                                blocks[cx][cz] = blockId;
                                data[cx][cz] = getBlockData_LevelDB_v3(cdata, cdata_size, cx, cz, cy);
                                topBlockY[cx][cz] = realy;

                                int32_t cy2 = cy;

                                // todonow todohere todobig todostopper - can't do this until we have the whole chunk
#if 1
                                // todo - we are getting the block light ABOVE this block (correct?)
                                // todo - this will break if we are using force-top stuff
                                if (blockInfoList[blockId].isSolid()) {
                                    // move to block above this block
                                    cy2++;
                                    if (cy2 > MAX_BLOCK_HEIGHT) { cy2 = MAX_BLOCK_HEIGHT; }
                                } else {
                                    // if not solid, don't adjust
                                }
#endif
                                uint8_t sl = getBlockSkyLight_LevelDB_v3(cdata, cdata_size, cx, cz, cy2);
                                uint8_t bl = getBlockBlockLight_LevelDB_v3(cdata, cdata_size, cx, cz, cy2);
                                // we combine the light nibbles into a byte
                                topLight[cx][cz] = (sl << 4) | bl;
                            }
                        }
                    }
                }
            }

            if (control.quietFlag) {
                return 0;
            }

            // todonow todobig todohere todostopper - this is not valid until we have the whole chunk

            if (false) {
                // print chunk info
                logger.msg(kLogInfo1, "Top Blocks (block-id:block-data:biome-id):\n");
                // note the different use of cx/cz here
                uint32_t rawData;
                for (int32_t cz = 0; cz < 16; cz++) {
                    for (int32_t cx = 0; cx < 16; cx++) {
                        rawData = grassAndBiome[cx][cz];
                        biomeId = (uint8_t) (rawData & 0xFF);
                        logger.msg(kLogInfo1, "%03x:%x:%02x ", (int) blocks[cx][cz], (int) data[cx][cz], (int) biomeId);
                    }
                    logger.msg(kLogInfo1, "\n");
                }
                logger.msg(kLogInfo1, "Block Histogram:\n");
                for (int32_t i = 0; i < 512; i++) {
                    if (histogramBlock[i] > 0) {
                        logger.msg(kLogInfo1, "%s-hg: %02x: %6d (%s)\n", dimName.c_str(), i, histogramBlock[i],
                                   blockInfoList[i].name.c_str());
                    }
                }
                logger.msg(kLogInfo1, "Biome Histogram:\n");
                for (int32_t i = 0; i < 256; i++) {
                    if (histogramBiome[i] > 0) {
                        std::string biomeName(getBiomeName(i));
                        logger.msg(kLogInfo1, "%s-hg-biome: %02x: %6d (%s)\n", dimName.c_str(), i, histogramBiome[i],
                                   biomeName.c_str());
                    }
                }
                logger.msg(kLogInfo1, "Block Light (skylight:blocklight:heightcol):\n");
                for (int32_t cz = 0; cz < 16; cz++) {
                    for (int32_t cx = 0; cx < 16; cx++) {
                        logger.msg(kLogInfo1, "%x:%x:%02x ", (int) ((topLight[cx][cz] >> 4) & 0xf),
                                   (int) (topLight[cx][cz] & 0xf), (int) heightCol[cx][cz]);
                    }
                    logger.msg(kLogInfo1, "\n");
                }
            }

            return 0;
        }


        int32_t _do_chunk_v7(int32_t tchunkX, int32_t tchunkY, int32_t tchunkZ, const char *cdata, size_t cdata_size,
                             int32_t dimensionId, const std::string &dimName,
                             Histogram &histogramGlobalBlock,
                             const bool *fastBlockHideList, const bool *fastBlockForceTopList,
                             const bool *fastBlockToGeoJSON,
                             const CheckSpawnList &listCheckSpawn) {
            chunkX = tchunkX;
            int32_t chunkY = tchunkY;
            chunkZ = tchunkZ;
            chunkFormatVersion = 7;

            // todonow todostopper - this is problematic for cubic chunks
            int16_t histogramBlock[512];
            int16_t histogramBiome[256];
            memset(histogramBlock, 0, sizeof(histogramBlock));
            memset(histogramBiome, 0, sizeof(histogramBiome));

            // see if we need to check any columns in this chunk for spawnable
            int32_t wx = chunkX * 16;
            int32_t wz = chunkZ * 16;
            for (const auto &it : listCheckSpawn) {
                if (it->contains(wx, wz)) {
                    checkSpawnFlag = true;
                    break;
                } else if (it->contains(wx, wz + 15)) {
                    checkSpawnFlag = true;
                    break;
                } else if (it->contains(wx + 15, wz + 15)) {
                    checkSpawnFlag = true;
                    break;
                } else if (it->contains(wx + 15, wz)) {
                    checkSpawnFlag = true;
                    break;
                }
            }

            // determine location of chunk palette
            int32_t blocksPerWord = -1;
            int32_t bitsPerBlock = -1;
            bool paddingFlag = false;
            int32_t offsetBlockInfoList = -1;
            int32_t extraOffset = -1;

            //logger.msg(kLogWarning,"hey -- cdata %02x %02x %02x\n", cdata[0], cdata[1], cdata[2]);

            if (setupBlockVars_v7(cdata, blocksPerWord, bitsPerBlock, paddingFlag, offsetBlockInfoList, extraOffset) !=
                0) {
                return -1;
            }

            // read chunk palette and associate old-school block id's
            MyNbtTagList tagList;
            int xoff = offsetBlockInfoList + 6 + extraOffset;
            // debug
            if (false) {
                logger.msg(kLogWarning,
                           "hey -- cdata[0..2] = %02x %02x %02x // blocksPerWord=%02x bitsPerBlock=%02x maxPal=%.0lf // before nbt = %02x %02x %02x %02x %02x %02x\n",
                           (int) cdata[0], (int) cdata[1], (int) cdata[2], blocksPerWord, bitsPerBlock,
                           pow(2.0, (double) bitsPerBlock), (unsigned int) cdata[offsetBlockInfoList + 0] & 0xff,
                           (unsigned int) cdata[offsetBlockInfoList + 1] & 0xff,
                           (unsigned int) cdata[offsetBlockInfoList + 2] & 0xff,
                           (unsigned int) cdata[offsetBlockInfoList + 3] & 0xff,
                           (unsigned int) cdata[offsetBlockInfoList + 4] & 0xff,
                           (unsigned int) cdata[offsetBlockInfoList + 5] & 0xff
                );
            }
            parseNbtQuiet(&cdata[xoff], cdata_size - xoff, cdata[offsetBlockInfoList + 3], tagList);
            //parseNbt("chunk-palette",&cdata[xoff], cdata_size-xoff, tagList);

            std::vector<int32_t> chunkBlockPalette_BlockId(tagList.size());
            std::vector<int32_t> chunkBlockPalette_BlockData(tagList.size());

            for (size_t i = 0; i < tagList.size(); i++) {
                // check tagList
                if (tagList[i].second->get_type() == nbt::tag_type::Compound) {
                    nbt::tag_compound tc = tagList[i].second->as<nbt::tag_compound>();

                    bool processedFlag = false;
                    if (tc.has_key("name", nbt::tag_type::String)) {
                        std::string bname = tc["name"].as<nbt::tag_string>().get();
                        int bdata = 0;
                        if (tc.has_key("val", nbt::tag_type::Short)) {
                            bdata = tc["val"].as<nbt::tag_short>().get();
                        }
                        int32_t blockId, blockData;
                        if (getBlockByUname(bname, blockId, blockData) == 0) {
                            chunkBlockPalette_BlockId[i] = blockId;
                            // todonow - correct?
                            chunkBlockPalette_BlockData[i] = bdata;
                        } else {
                            logger.msg(kLogWarning, "Did not find block uname '%s' in XML file\n", bname.c_str());
                            // todonow - reasonable?
                            chunkBlockPalette_BlockId[i] = 0;
                            chunkBlockPalette_BlockData[i] = 0;
                        }
                        processedFlag = true;
                    }
                    if (!processedFlag) {
                        slogger.msg(kLogError, "(Safe) Did not find 'name' tag in a chunk palette! (i=%d) (len=%d)\n",
                                    (int) i, (int) tagList.size());
                        //todozooz - dump tc to screen log
                    }
                } else {
                    logger.msg(kLogWarning, "Unexpected NBT format in _do_chunk_v7\n");
                }
            }

            //todozooz -- new 16-bit block-id's (instead of 8-bit) are a BIG issue - this needs attention here
            // iterate over chunk space
            uint8_t paletteBlockId, blockData, biomeId;
            int32_t blockId;
            for (int32_t cy = 0; cy < 16; cy++) {
                for (int32_t cx = 0; cx < 16; cx++) {
                    for (int32_t cz = 0; cz < 16; cz++) {
                        paletteBlockId = getBlockId_LevelDB_v7(&cdata[2 + extraOffset], blocksPerWord, bitsPerBlock, cx,
                                                               cz, cy);

                        // look up blockId
                        //todonow error checking
                        if (paletteBlockId < chunkBlockPalette_BlockId.size()) {
                            blockId = chunkBlockPalette_BlockId[paletteBlockId];
                            blockData = chunkBlockPalette_BlockData[paletteBlockId];
                        } else {
                            blockId = 0;
                            blockData = 0;
                            logger.msg(kLogWarning, "Found chunk palette id out of range %d (size=%d)\n",
                                       paletteBlockId, (int) chunkBlockPalette_BlockId.size());
                        }
                        histogramBlock[blockId]++;
                        histogramGlobalBlock.add(blockId);

                        // todobig - handle block variant?
                        if (fastBlockToGeoJSON[blockId]) {
                            double ix, iy;
                            char tmpstring[512];
                            worldPointToGeoJSONPoint(dimensionId, chunkX * 16 + cx, chunkZ * 16 + cz, ix, iy);
                            sprintf(tmpstring, ""
                                               "\"Name\": \"%s\", "
                                               "\"Block\": true, "
                                               "\"Dimension\": \"%d\", "
                                               "\"Pos\": [%d, %d, %d]"
                                               "} }", blockInfoList[blockId].name.c_str(), dimensionId,
                                    chunkX * 16 + cx, chunkY * 16 + cy, chunkZ * 16 + cz
                            );
                            std::string json = ""
                                               + makeGeojsonHeader(ix, iy)
                                               + tmpstring;
                            listGeoJSON.push_back(json);
                        }

                        // note: we check spawnable later

                        // todo - check for isSolid?

                        int32_t realy = chunkY * 16 + cy;
                        if (blockId != 0) {  // current block is NOT air
                            // todonow - this will break forcetop!
                            if ((realy >= topBlockY[cx][cz] &&
                                 !fastBlockForceTopList[blocks[cx][cz]] &&
                                 // blocks[cx][cz] == 0 &&  // top block is not already set
                                 !fastBlockHideList[blockId]) ||
                                fastBlockForceTopList[blockId]) {

                                blocks[cx][cz] = blockId;
                                data[cx][cz] = blockData; // getBlockData_LevelDB_v3(cdata, cdata_size, cx,cz,cy);
                                topBlockY[cx][cz] = realy;

                                int32_t cy2 = cy;

                                // todonow todohere todobig todostopper - can't do this until we have the whole chunk
#if 1
                                // todo - we are getting the block light ABOVE this block (correct?)
                                // todo - this will break if we are using force-top stuff
                                if (blockInfoList[blockId].isSolid()) {
                                    // move to block above this block
                                    cy2++;
                                    if (cy2 > MAX_BLOCK_HEIGHT) { cy2 = MAX_BLOCK_HEIGHT; }
                                } else {
                                    // if not solid, don't adjust
                                }
#endif
                                // todonow todohere -- no blocklight or skylight in v7 chunks?!
                                uint8_t sl = 0; // getBlockSkyLight_LevelDB_v3(cdata, cdata_size, cx,cz,cy2);
                                uint8_t bl = 0; // getBlockBlockLight_LevelDB_v3(cdata, cdata_size, cx,cz,cy2);
                                // we combine the light nibbles into a byte
                                topLight[cx][cz] = (sl << 4) | bl;
                            }
                        }
                    }
                }
            }

            if (control.quietFlag) {
                return 0;
            }

            // todonow todobig todohere todostopper - this is not valid until we have the whole chunk

            if (false) {
                // print chunk info
                logger.msg(kLogInfo1, "Top Blocks (block-id:block-data:biome-id):\n");
                // note the different use of cx/cz here
                uint32_t rawData;
                for (int32_t cz = 0; cz < 16; cz++) {
                    for (int32_t cx = 0; cx < 16; cx++) {
                        rawData = grassAndBiome[cx][cz];
                        biomeId = (uint8_t) (rawData & 0xFF);
                        logger.msg(kLogInfo1, "%03x:%x:%02x ", (int) blocks[cx][cz], (int) data[cx][cz], (int) biomeId);
                    }
                    logger.msg(kLogInfo1, "\n");
                }
                logger.msg(kLogInfo1, "Block Histogram:\n");
                for (int32_t i = 0; i < 512; i++) {
                    if (histogramBlock[i] > 0) {
                        logger.msg(kLogInfo1, "%s-hg: %02x: %6d (%s)\n", dimName.c_str(), i, histogramBlock[i],
                                   blockInfoList[i].name.c_str());
                    }
                }
                logger.msg(kLogInfo1, "Biome Histogram:\n");
                for (int32_t i = 0; i < 256; i++) {
                    if (histogramBiome[i] > 0) {
                        std::string biomeName(getBiomeName(i));
                        logger.msg(kLogInfo1, "%s-hg-biome: %02x: %6d (%s)\n", dimName.c_str(), i, histogramBiome[i],
                                   biomeName.c_str());
                    }
                }
                logger.msg(kLogInfo1, "Block Light (skylight:blocklight:heightcol):\n");
                for (int32_t cz = 0; cz < 16; cz++) {
                    for (int32_t cx = 0; cx < 16; cx++) {
                        logger.msg(kLogInfo1, "%x:%x:%02x ", (int) ((topLight[cx][cz] >> 4) & 0xf),
                                   (int) (topLight[cx][cz] & 0xf), (int) heightCol[cx][cz]);
                    }
                    logger.msg(kLogInfo1, "\n");
                }
            }

            return 0;
        }


        int32_t _do_chunk_biome_v3(int32_t tchunkX, int32_t tchunkZ, const char *cdata, int32_t cdatalen,
                                   Histogram &histogramGlobalBiome) {
            chunkX = tchunkX;
            chunkZ = tchunkZ;

            // debug
            if (false) {
                logger.msg(kLogInfo1, "biome_v3: x=%d z=%d datalen=%d -- data:", tchunkX, tchunkZ, cdatalen);
                bool newlineStart = true;
                for (int i = 0; i < cdatalen; i++) {
                    if (newlineStart) {
                        logger.msg(kLogInfo1, "\nbiome_v3_data:");
                        newlineStart = false;
                    }
                    logger.msg(kLogInfo1, " %02x", cdata[i]);
                    if (((i + 1) % 16) == 0) {
                        newlineStart = true;
                    }
                }
                logger.msg(kLogInfo1, "\n");
            }

            int16_t histogramBiome[256];
            memset(histogramBiome, 0, sizeof(histogramBiome));

            // get per-column data
            uint8_t biomeId = 0;
            for (int32_t cx = 0; cx < 16; cx++) {
                for (int32_t cz = 0; cz < 16; cz++) {
                    heightCol[cx][cz] = getColData_Height_LevelDB_v3(cdata, cx, cz);
                    grassAndBiome[cx][cz] = getColData_GrassAndBiome_LevelDB_v3(cdata, cdatalen, cx, cz);

                    biomeId = (uint8_t) (grassAndBiome[cx][cz] & 0xFF);
                    histogramBiome[biomeId]++;
                    histogramGlobalBiome.add(biomeId);

#if 0
                                                                                                                                            // todo - testing idea about lighting - get lighting from top solid block - result is part good, part crazy
          int32_t ty = heightCol[cx][cz] + 1;
          if ( ty > MAX_BLOCK_HEIGHT ) { ty=MAX_BLOCK_HEIGHT; }
          uint8_t sl = getBlockSkyLight_LevelDB_v3(cdata, cx,cz,ty);
          uint8_t bl = getBlockBlockLight_LevelDB_v3(cdata, cx,cz,ty);
          topLight[cx][cz] = (sl << 4) | bl;
#endif
                }
            }


            return 0;
        }

        int32_t checkSpawnable(leveldb::DB *db, int32_t dimId, const CheckSpawnList &listCheckSpawn) {

            if (chunkFormatVersion != 3 || !checkSpawnFlag) {
                // we do not need to check this chunk
                return 0;
            }

            // we have a chunk that is v3 and contains at least some pixels which need to be checked
            // we need to collect all available cubic chunks

            const int32_t blockDataMaxSize = 16 * 16 * MAX_BLOCK_HEIGHT;
            //      const int32_t blockidSubchunkSize = 16 * 16 * 16;
            //      const int32_t blockdataSubchunkSize = 16 * 16 * 8;
            //      const int32_t blocklightSubchunkSize = 16 * 16 * 8;
            char *blockidData = new char[blockDataMaxSize];
            char *blockdataData = new char[blockDataMaxSize];
            char *blocklightData = new char[blockDataMaxSize];

            memset(blockidData, 0, blockDataMaxSize);
            memset(blockdataData, 0, blockDataMaxSize);
            memset(blocklightData, 0, blockDataMaxSize);

            // get the data
            char keybuf[128];
            int32_t keybuflen;
            int32_t kw = dimId;
            uint8_t kt_v3 = 0x2f;
            leveldb::Status dstatus;
            std::string svalue;
            const char *pchunk = nullptr;
            size_t pchunk_size;
            for (int8_t cubicy = 0; cubicy < MAX_CUBIC_Y; cubicy++) {

                // todobug - this fails around level 112? on another1 -- weird -- run a valgrind to see where we're messing up
                //check valgrind output

                // construct key to get the chunk
                if (dimId == kDimIdOverworld) {
                    //overworld
                    memcpy(&keybuf[0], &chunkX, sizeof(int32_t));
                    memcpy(&keybuf[4], &chunkZ, sizeof(int32_t));
                    memcpy(&keybuf[8], &kt_v3, sizeof(uint8_t));
                    memcpy(&keybuf[9], &cubicy, sizeof(uint8_t));
                    keybuflen = 10;
                } else {
                    // nether (and probably any others that are added)
                    memcpy(&keybuf[0], &chunkX, sizeof(int32_t));
                    memcpy(&keybuf[4], &chunkZ, sizeof(int32_t));
                    memcpy(&keybuf[8], &kw, sizeof(int32_t));
                    memcpy(&keybuf[12], &kt_v3, sizeof(uint8_t));
                    memcpy(&keybuf[13], &cubicy, sizeof(uint8_t));
                    keybuflen = 14;
                }

                dstatus = db->Get(levelDbReadOptions, leveldb::Slice(keybuf, keybuflen), &svalue);
                if (dstatus.ok()) {
                    pchunk = svalue.data();
                    pchunk_size = svalue.size();

                    // copy data
                    // todobig - make this faster with memcpy's -- it is important to consider the way we'll extract the data later :)
                    for (int32_t cx = 0; cx < 16; cx++) {
                        for (int32_t cz = 0; cz < 16; cz++) {
                            for (int32_t ccy = 0; ccy < 16; ccy++) {
                                int32_t cy = cubicy * 16 + ccy;

                                int32_t off = _calcOffsetBlock_LevelDB_v3_fullchunk(cx, cz, cy);
                                blockidData[off] = getBlockId_LevelDB_v3(pchunk, cx, cz, ccy);
                                blockdataData[off] = getBlockData_LevelDB_v3(pchunk, pchunk_size, cx, cz, ccy);
                                blocklightData[off] = getBlockBlockLight_LevelDB_v3(pchunk, pchunk_size, cx, cz, ccy);
                            }
                        }
                    }

                    //      memcpy(&blockidData[cubicy * blockidSubchunkSize], &pchunk[1], blockidSubchunkSize);
                    //      memcpy(&blockdataData[cubicy * blockdataSubchunkSize], &pchunk[(16*16*16)+1], blockdataSubchunkSize);
                    //      memcpy(&blocklightData[cubicy * blocklightSubchunkSize], &pchunk[(16*16*16) + (16*16*8) + (16*16*8) + 1], blocklightSubchunkSize);
                }
            }

            // we have all the data, now we check spawnable
            int32_t wx = chunkX * 16;
            int32_t wz = chunkZ * 16;
            for (int32_t cy = MAX_BLOCK_HEIGHT; cy >= 0; cy--) {
                for (int32_t cx = 0; cx < 16; cx++) {
                    for (int32_t cz = 0; cz < 16; cz++) {

                        uint8_t blockId = getData_LevelDB_v3_fullchunk(blockidData, cx, cz, cy);

                        // check spawnable -- cannot check spawn at 0 or MAX_BLOCK_HEIGHT because we need above/below blocks
                        if (cy > 0 && cy < MAX_BLOCK_HEIGHT) {
                            bool continueCheckSpawnFlag = false;
                            for (const auto &it : listCheckSpawn) {
                                if (it->contains(wx + cx, wz + cz)) {
                                    continueCheckSpawnFlag = true;
                                    break;
                                }
                            }
                            if (continueCheckSpawnFlag) {

                                // note: rules adapted from: http://minecraft.gamepedia.com/Spawn

                                // todobig - is this missing some spawnable blocks?

                                // "the spawning block itself must be non-opaque and non-liquid"
                                // we add: non-solid
                                if (!blockInfoList[blockId].isOpaque() &&
                                    !blockInfoList[blockId].isLiquid() &&
                                    !blockInfoList[blockId].isSolid()) {

                                    // "the block directly above it must be non-opaque"

                                    uint8_t aboveBlockId = getData_LevelDB_v3_fullchunk(blockidData, cx, cz, cy + 1);
                                    if (!blockInfoList[aboveBlockId].isOpaque()) {

                                        // "the block directly below it must have a solid top surface (opaque, upside down slabs / stairs and others)"
                                        // "the block directly below it may not be bedrock or barrier" -- take care of with 'spawnable'

                                        uint8_t belowBlockId = getData_LevelDB_v3_fullchunk(blockidData, cx, cz,
                                                                                            cy - 1);
                                        uint8_t belowBlockData = getData_LevelDB_v3_fullchunk(blockdataData, cx, cz,
                                                                                              cy - 1);

                                        //if ( blockInfoList[belowBlockId].isOpaque() && blockInfoList[belowBlockId].isSpawnable(belowBlockData) ) {
                                        if (blockInfoList[belowBlockId].isSpawnable(belowBlockData)) {

                                            // check the light level
                                            uint8_t bl = getData_LevelDB_v3_fullchunk(blocklightData, cx, cz, cy);
                                            if (bl <= 7) {
                                                // spwawnable! add it to the list
                                                double ix, iy;
                                                char tmpstring[512];
                                                worldPointToGeoJSONPoint(dimId, chunkX * 16 + cx, chunkZ * 16 + cz, ix,
                                                                         iy);
                                                sprintf(tmpstring, ""
                                                                   "\"Spawnable\":true,"
                                                                   "\"Name\":\"Spawnable\","
                                                                   "\"LightLevel\":\"%d\","
                                                                   "\"Dimension\":\"%d\","
                                                                   "\"Pos\":[%d,%d,%d]"
                                                                   "}}", (int) bl, dimId, chunkX * 16 + cx, cy,
                                                        chunkZ * 16 + cz
                                                );
                                                std::string json = ""
                                                                   + makeGeojsonHeader(ix, iy)
                                                                   + tmpstring;
                                                listGeoJSON.push_back(json);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            delete[] blockidData;
            delete[] blockdataData;
            delete[] blocklightData;

            return 0;
        }
    };


    class DimensionData {
    public:
    };

    class DimensionData_LevelDB : public DimensionData {
    private:
        std::string name;
        int32_t dimId;

        typedef std::pair<uint32_t, uint32_t> ChunkKey;
        typedef std::map<ChunkKey, std::unique_ptr<ChunkData_LevelDB> > ChunkData_LevelDB_Map;
        ChunkData_LevelDB_Map chunks;

        int32_t minChunkX, maxChunkX;
        int32_t minChunkZ, maxChunkZ;
        bool chunkBoundsValid;

        int32_t histogramChunkType[256];
        Histogram histogramGlobalBlock;
        Histogram histogramGlobalBiome;

        bool fastBlockForceTopList[512];
        bool fastBlockHideList[512];
        bool fastBlockToGeoJSONList[512];

        // convenience vars from world object
        std::string worldName;
        int32_t worldSpawnX, worldSpawnZ;
        int64_t worldSeed;

        bool chunks_has_key(const ChunkData_LevelDB_Map &m, const ChunkKey &k) {
            return m.find(k) != m.end();
        }

    public:
        // todobig - move these to private?
        std::vector<int32_t> blockForceTopList;
        std::vector<int32_t> blockHideList;
        std::vector<int32_t> blockToGeoJSONList;

        CheckSpawnList listCheckSpawn;
        SchematicList listSchematic;

        DimensionData_LevelDB() {
            name = "(UNKNOWN)";
            dimId = -1;
            chunkBoundsValid = false;
            minChunkX = 0;
            maxChunkX = 0;
            minChunkZ = 0;
            maxChunkZ = 0;
            memset(histogramChunkType, 0, sizeof(histogramChunkType));
            worldName = "(UNKNOWN)";
            worldSpawnX = worldSpawnZ = 0;
            worldSeed = 0;
        }

        void setWorldInfo(const std::string &wName, int32_t wSpawnX, int32_t wSpawnZ, int64_t wSeed) {
            worldName = wName;
            worldSpawnX = wSpawnX;
            worldSpawnZ = wSpawnZ;
            worldSeed = wSeed;
        }

        void updateFastLists() {
            for (int32_t bid = 0; bid < 512; bid++) {
                fastBlockHideList[bid] = vectorContains(blockHideList, bid);
                fastBlockForceTopList[bid] = vectorContains(blockForceTopList, bid);
                fastBlockToGeoJSONList[bid] = vectorContains(blockToGeoJSONList, bid);
            }
        }

        void setName(const std::string &s) {
            name = s;
        }

        const std::string &getName() const {
            return name;
        }

        void setDimId(int32_t id) { dimId = id; }

        void addHistogramChunkType(uint8_t t) {
            histogramChunkType[t]++;
        }

        void unsetChunkBoundsValid() {
            minChunkX = minChunkZ = maxChunkX = maxChunkZ = 0;
            chunkBoundsValid = false;
        }

        bool getChunkBoundsValid() {
            return chunkBoundsValid;
        }

        void setChunkBoundsValid() {
            chunkBoundsValid = true;
        }

        void reportChunkBounds() {
            const int32_t chunkW = (maxChunkX - minChunkX + 1);
            const int32_t chunkH = (maxChunkZ - minChunkZ + 1);
            const int32_t imageW = chunkW * 16;
            const int32_t imageH = chunkH * 16;

            slogger.msg(kLogInfo1, "  Bounds (chunk): DimId=%d X=(%d %d) Z=(%d %d)\n", dimId, minChunkX, maxChunkX,
                        minChunkZ, maxChunkZ
            );
            slogger.msg(kLogInfo1, "  Bounds (pixel): DimId=%d X=(%d %d) Z=(%d %d) Image=(%d %d)\n", dimId,
                        minChunkX * 16, maxChunkX * 16, minChunkZ * 16, maxChunkZ * 16, imageW, imageH
            );
        }

        void addToChunkBounds(int32_t chunkX, int32_t chunkZ) {
            minChunkX = std::min(minChunkX, chunkX);
            maxChunkX = std::max(maxChunkX, chunkX);
            minChunkZ = std::min(minChunkZ, chunkZ);
            maxChunkZ = std::max(maxChunkZ, chunkZ);
        }

        int32_t getMinChunkX() { return minChunkX; }

        int32_t getMaxChunkX() { return maxChunkX; }

        int32_t getMinChunkZ() { return minChunkZ; }

        int32_t getMaxChunkZ() { return maxChunkZ; }

        int32_t addChunk(int32_t tchunkFormatVersion, int32_t chunkX, int32_t chunkY, int32_t chunkZ, const char *cdata,
                         size_t cdata_size) {
            ChunkKey chunkKey(chunkX, chunkZ);
            switch (tchunkFormatVersion) {
                case 2:
                    // pre-0.17
                    chunks[chunkKey] = std::unique_ptr<ChunkData_LevelDB>(new ChunkData_LevelDB());
                    return chunks[chunkKey]->_do_chunk_v2(chunkX, chunkZ, cdata, dimId, name,
                                                          histogramGlobalBlock, histogramGlobalBiome,
                                                          fastBlockHideList, fastBlockForceTopList,
                                                          fastBlockToGeoJSONList,
                                                          listCheckSpawn);
                    return 0;
                case 3:
                    // 0.17 and later?
                    // we need to process all sub-chunks, not just blindy add them

                    if (!chunks_has_key(chunks, chunkKey)) {
                        chunks[chunkKey] = std::unique_ptr<ChunkData_LevelDB>(new ChunkData_LevelDB());
                    }

                    return chunks[chunkKey]->_do_chunk_v3(chunkX, chunkY, chunkZ, cdata, cdata_size, dimId, name,
                                                          histogramGlobalBlock,
                                                          fastBlockHideList, fastBlockForceTopList,
                                                          fastBlockToGeoJSONList,
                                                          listCheckSpawn);
                case 7:
                    // 1.2.x betas?
                    // we need to process all sub-chunks, not just blindy add them

                    if (!chunks_has_key(chunks, chunkKey)) {
                        chunks[chunkKey] = std::unique_ptr<ChunkData_LevelDB>(new ChunkData_LevelDB());
                    }

                    return chunks[chunkKey]->_do_chunk_v7(chunkX, chunkY, chunkZ, cdata, cdata_size, dimId, name,
                                                          histogramGlobalBlock,
                                                          fastBlockHideList, fastBlockForceTopList,
                                                          fastBlockToGeoJSONList,
                                                          listCheckSpawn);
                    return 0;
            }
            slogger.msg(kLogError, "UNKNOWN CHUNK FORMAT (%d)\n", tchunkFormatVersion);
            return -1;
        }

        int32_t addChunkColumnData(int32_t tchunkFormatVersion, int32_t chunkX, int32_t chunkZ, const char *cdata,
                                   int32_t cdatalen) {
            switch (tchunkFormatVersion) {
                case 2:
                    // pre-0.17
                    // column data is in the main record
                    break;
                case 3:
                    // 0.17 and later?
                    // we need to process all sub-chunks, not just blindy add them

                    ChunkKey chunkKey(chunkX, chunkZ);
                    if (!chunks_has_key(chunks, chunkKey)) {
                        chunks[chunkKey] = std::unique_ptr<ChunkData_LevelDB>(new ChunkData_LevelDB());
                    }

                    return chunks[chunkKey]->_do_chunk_biome_v3(chunkX, chunkZ, cdata, cdatalen, histogramGlobalBiome);
            }
            slogger.msg(kLogError, "UNKNOWN CHUNK FORMAT (%d)\n", tchunkFormatVersion);
            return -1;
        }

        int32_t checkSpawnable(leveldb::DB *db) {
            for (const auto &it : chunks) {
                it.second->checkSpawnable(db, dimId, listCheckSpawn);
            }
            return 0;
        }

        //todolib - move this out?
        bool checkDoForDim(int32_t v) {
            if (v == kDoOutputAll) {
                return true;
            }
            if (v == dimId) {
                return true;
            }
            return false;
        }

        // todolib - move this out?
        int32_t addCheckSpawn(int32_t checkX, int32_t checkZ, int32_t distance) {
            listCheckSpawn.push_back(std::unique_ptr<CheckSpawn>(new CheckSpawn(checkX, checkZ, distance)));
            return 0;
        }

        // todolib - move this out?
        int32_t addSchematic(int32_t x1, int32_t y1, int32_t z1,
                             int32_t x2, int32_t y2, int32_t z2,
                             const char *fnSchematic) {
            listSchematic.push_back(std::unique_ptr<Schematic>(new Schematic(x1, y1, z1, x2, y2, z2, fnSchematic)));
            return 0;
        }

        void worldPointToImagePoint(double wx, double wz, double &ix, double &iy, bool geoJsonFlag) {
            const int32_t chunkOffsetX = -minChunkX;
            const int32_t chunkOffsetZ = -minChunkZ;

            if (geoJsonFlag) {
                const int32_t chunkH = (maxChunkZ - minChunkZ + 1);
                const int32_t imageH = chunkH * 16;

                ix = wx + (chunkOffsetX * 16);
                // todobig - correct calc here?
                iy = (imageH - 1) - (wz + (chunkOffsetZ * 16));


                // todobig -- for geojson, image == world (with y coordinate negated)
                if (false) {
                    ix = wx;
                    iy = -wz;
                }

            } else {
                ix = wx + (chunkOffsetX * 16);
                iy = wz + (chunkOffsetZ * 16);
            }

            // adjust for nether
            /*
        if ( dimId == kDimIdNether ) {
        ix /= 8;
        iy /= 8;
        }
      */
        }

        void doOutputStats() {
            logger.msg(kLogInfo1, "\n%s Statistics:\n", name.c_str());
            logger.msg(kLogInfo1, "chunk-count: %d\n", (int) chunks.size());
            logger.msg(kLogInfo1, "Min-dim:  %d %d\n", minChunkX, minChunkZ);
            logger.msg(kLogInfo1, "Max-dim:  %d %d\n", maxChunkX, maxChunkZ);
            int32_t dx = (maxChunkX - minChunkX + 1);
            int32_t dz = (maxChunkZ - minChunkZ + 1);
            logger.msg(kLogInfo1, "diff-dim: %d %d\n", dx, dz);
            logger.msg(kLogInfo1, "pixels:   %d %d\n", dx * 16, dz * 16);

            logger.msg(kLogInfo1, "\nGlobal Chunk Type Histogram:\n");
            for (int32_t i = 0; i < 256; i++) {
                if (histogramChunkType[i] > 0) {
                    logger.msg(kLogInfo1, "hg-chunktype: %02x %6d\n", i, histogramChunkType[i]);
                }
            }

            double htotal;
            HistogramVector hvector;

            logger.msg(kLogInfo1, "\nGlobal Block Histogram (block-id count pct name):\n");
            htotal = histogramGlobalBlock.getTotal();
            hvector = histogramGlobalBlock.sort(1);
            for (auto &it : hvector) {
                int32_t k = it.first;
                int32_t v = it.second;
                double pct = ((double) v / htotal) * 100.0;
                logger.msg(kLogInfo1, "hg-globalblock: 0x%02x %10d %7.3lf%% %s\n", k, v, pct,
                           blockInfoList[k].name.c_str());
            }

            logger.msg(kLogInfo1, "\nGlobal Biome Histogram (biome-id count pct name):\n");
            htotal = histogramGlobalBiome.getTotal();
            hvector = histogramGlobalBiome.sort(1);
            for (auto &it : hvector) {
                int32_t k = it.first;
                int32_t v = it.second;
                double pct = ((double) v / htotal) * 100.0;
                //      logger.msg(kLogInfo1,"hg-globalbiome: 0x%02x %10d %7.3lf%% %s\n", k, v, pct, biomeInfoList[k]->name.c_str());
                logger.msg(kLogInfo1, "hg-globalbiome: 0x%02x %10d %7.3lf%%\n", k, v, pct);
            }
        }

        std::string makeImageDescription(int32_t imageMode, int32_t layerNumber) {
            std::string ret = "MCPE Viz Image -- World=(" + worldName + ")";
            ret += " Dimension=(" + name + ")";
            ret += " Image=(";
            switch (imageMode) {
                case kImageModeTerrain:
                    ret += "Overview Map";
                    break;
                case kImageModeBiome:
                    ret += "Biome Map";
                    break;
                case kImageModeGrass:
                    ret += "Grass Color Map";
                    break;
                case kImageModeHeightCol:
                    ret += "Top Block Height Map";
                    break;
                case kImageModeHeightColGrayscale:
                    ret += "Top Block Height Map (grayscale)";
                    break;
                case kImageModeHeightColAlpha:
                    ret += "Top Block Height Map (alpha)";
                    break;
                case kImageModeShadedRelief:
                    ret += "Shaded Relief";
                    break;
                case kImageModeBlockLight:
                    ret += "Top Block Light Map";
                    break;
                case kImageModeSkyLight:
                    ret += "Top Block Sky Light Map";
                    break;
                case kImageModeSlimeChunksMCPC:
                    ret += "Slime Chunks Map (MCPC)";
                    break;
                case kImageModeSlimeChunksMCPE:
                    ret += "Slime Chunks Map (MCPE)";
                    break;
                case -1:
                    // special marker for raw layers
                {
                    char tmpstring[256];
                    sprintf(tmpstring, "Raw Layer %d", layerNumber);
                    ret += tmpstring;
                }
                    break;
                default: {
                    char tmpstring[256];
                    sprintf(tmpstring, "UNKNOWN: ImageMode=%d", imageMode);
                    ret += tmpstring;
                }
                    break;
            }
            ret += ")";
            return ret;
        }

        int32_t outputPNG(const std::string &fname, const std::string &imageDescription, uint8_t *buf, int32_t width,
                          int32_t height, bool rgbaFlag) {
            PngWriter png;
            if (png.init(fname, imageDescription, width, height, height, rgbaFlag, true) != 0) {
                return -1;
            }
            int32_t bpp = 3;
            if (rgbaFlag) {
                bpp = 4;
            }
            for (int32_t y = 0; y < height; y++) {
                png.row_pointers[y] = &buf[y * width * bpp];
            }
            png_write_image(png.png, png.row_pointers);
            png.close();
            return 0;
        }


        // todohere - rename - we are in "row(s) at a time" mode here
        int32_t
        outputPNG_init(PngWriter &png, const std::string &fname, const std::string &imageDescription, int32_t width,
                       int32_t height, bool rgbaFlag) {
            if (png.init(fname, imageDescription, width, height, height, rgbaFlag, false) != 0) {
                return -1;
            }
            return 0;
        }

        int32_t outputPNG_writeRow(PngWriter &png, uint8_t *buf) {
            png_write_row(png.png, buf);
            // todo - check err?
            return 0;
        }

        int32_t outputPNG_writeRows(PngWriter &png, uint8_t **rows, uint32_t nrows) {
            png_write_rows(png.png, rows, nrows);
            // todo - check err?
            return 0;
        }

        int32_t outputPNG_close(PngWriter &png) {
            png.close();
            // todo - check err?
            return 0;
        }


        int32_t generateImage(const std::string &fname, const ImageModeType imageMode) {
            const int32_t chunkOffsetX = -minChunkX;
            const int32_t chunkOffsetZ = -minChunkZ;

            const int32_t chunkW = (maxChunkX - minChunkX + 1);
            const int32_t chunkH = (maxChunkZ - minChunkZ + 1);
            const int32_t imageW = chunkW * 16;
            const int32_t imageH = chunkH * 16;

            int32_t bpp = 3;
            bool rgbaFlag = false;
            uint8_t lut[256];

            if (imageMode == kImageModeHeightColAlpha) {
                bpp = 4;
                rgbaFlag = true;
                // todobig - experiment with other ways to do this lut for height alpha
                double vmax = (double) MAX_BLOCK_HEIGHT * (double) MAX_BLOCK_HEIGHT;
                for (int32_t i = 0; i <= MAX_BLOCK_HEIGHT; i++) {
                    // todobig make the offset (32) a cmdline param
                    double ti = ((MAX_BLOCK_HEIGHT + 1) + 32) - i;
                    double v = ((double) (ti * ti) / vmax) * 255.0;
                    if (v > 235.0) { v = 235.0; }
                    if (v < 0.0) { v = 0.0; }
                    lut[i] = v;
                }
            }

            // todohere -- reddit user silvergoat77 has a 1gb (!) world and it is approx 33k x 26k -- alloc chokes on this.
            // the solution is to write a chunk of rows at a time instead of the whole image...
            // but -- the code below is optimized to just iterate through the list and do it's thing instead of searching for each chunk
            // so --- we need to test before / after changing this to step thru in Z/X order

            // note RGB pixels
            uint8_t *buf = new uint8_t[imageW * 16 * bpp];

            uint8_t *rows[16];
            for (int i = 0; i < 16; i++) {
                rows[i] = &buf[i * imageW * bpp];
            }

            int32_t color;
            const char *pcolor;
            if (bpp == 4) {
                pcolor = (const char *) &color;
            } else {
                const char *pcolor_temp = (const char *) &color;
                pcolor = &pcolor_temp[1];
            }

            PngWriter png;
            if (outputPNG_init(png, fname, makeImageDescription(imageMode, 0), imageW, imageH, rgbaFlag) != 0) {
                delete[] buf;
                return -1;
            }

            for (int32_t iz = 0, chunkZ = minChunkZ; iz < imageH; iz += 16, chunkZ++) {

                // clear buffer
                memset(buf, 0, imageW * 16 * bpp);

                for (int32_t ix = 0, chunkX = minChunkX; ix < imageW; ix += 16, chunkX++) {

                    ChunkKey chunkKey(chunkX, chunkZ);
                    if (!chunks_has_key(chunks, chunkKey)) {
                        continue;
                    }

                    const auto &it = chunks.at(chunkKey);

                    int32_t imageX = (it->chunkX + chunkOffsetX) * 16;
                    int32_t imageZ = (it->chunkZ + chunkOffsetZ) * 16;

                    int32_t worldX = it->chunkX * 16;
                    int32_t worldZ = it->chunkZ * 16;

                    for (int32_t cz = 0; cz < 16; cz++) {
                        for (int32_t cx = 0; cx < 16; cx++) {

                            // todobig - we could do EVERYTHING (but initial key scan) in one pass:
                            //   do images here, then iterate over chunkspace again looking for items that populate geojson list

                            // todo - this big conditional inside an inner loop, not so good

                            if (imageMode == kImageModeBiome) {
                                // get biome color
                                int32_t biomeId = it->grassAndBiome[cx][cz] & 0xff;
                                if (has_key(biomeInfoList, biomeId)) {
                                    color = biomeInfoList[biomeId]->color;
                                } else {
                                    slogger.msg(kLogInfo1, "ERROR: Unknown biome %d 0x%x\n", biomeId, biomeId);
                                    color = htobe32(0xff2020);
                                }
                            } else if (imageMode == kImageModeGrass) {
                                // get grass color
                                int32_t grassColor = it->grassAndBiome[cx][cz] >> 8;
                                color = htobe32(grassColor);
                            } else if (imageMode == kImageModeHeightCol) {
                                // get height value and use red-black-green palette
                                if (control.heightMode == kHeightModeTop) {
                                    uint8_t c = it->topBlockY[cx][cz];
                                    color = palRedBlackGreen[c];
                                } else {
                                    uint8_t c = it->heightCol[cx][cz];
                                    color = palRedBlackGreen[c];
                                }
                            } else if (imageMode == kImageModeHeightColGrayscale) {
                                // get height value and make it grayscale
                                if (control.heightMode == kHeightModeTop) {
                                    uint8_t c = it->topBlockY[cx][cz];
                                    color = (c << 24) | (c << 16) | (c << 8);
                                } else {
                                    uint8_t c = it->heightCol[cx][cz];
                                    color = (c << 24) | (c << 16) | (c << 8);
                                }
                            } else if (imageMode == kImageModeHeightColAlpha) {
                                // get height value and make it alpha
                                uint8_t c;
                                if (control.heightMode == kHeightModeTop) {
                                    c = it->topBlockY[cx][cz];
                                } else {
                                    c = it->heightCol[cx][cz];
                                }
                                // c = (90 - (int32_t)it->heightCol[cx][cz]) * 2;
                                c = lut[c];
                                color = ((c & 0xff) << 24);
                            } else if (imageMode == kImageModeBlockLight) {
                                // get block light value and expand it (is only 4-bits)
                                uint8_t c = (it->topLight[cx][cz] & 0x0f) << 4;
                                color = (c << 24) | (c << 16) | (c << 8);
                            } else if (imageMode == kImageModeSkyLight) {
                                // get sky light value and expand it (is only 4-bits)
                                uint8_t c = (it->topLight[cx][cz] & 0xf0);
                                color = (c << 24) | (c << 16) | (c << 8);
                            } else {
                                // regular image
                                int32_t blockid = it->blocks[cx][cz];

                                if (blockInfoList[blockid].hasVariants()) {
                                    // we need to get blockdata
                                    int32_t blockdata = it->data[cx][cz];
                                    bool vfound = false;
                                    for (const auto &itbv : blockInfoList[blockid].variantList) {
                                        if (itbv->blockdata == blockdata) {
                                            vfound = true;
                                            color = itbv->color;
                                            break;
                                        }
                                    }
                                    if (!vfound) {
                                        // todo - warn once per id/blockdata or the output volume could get ridiculous
                                        slogger.msg(kLogInfo1,
                                                    "WARNING: Did not find block variant for block (id=%d (0x%x) '%s') with blockdata=%d (0x%x) MSG1\n",
                                                    blockid, blockid, blockInfoList[blockid].name.c_str(), blockdata,
                                                    blockdata
                                        );
                                        // since we did not find the variant, use the parent block's color
                                        color = blockInfoList[blockid].color;
                                    }
                                } else {
                                    color = blockInfoList[blockid].color;
                                    if (!blockInfoList[blockid].colorSetFlag) {
                                        blockInfoList[blockid].colorSetNeedCount++;
                                    }
                                }
                            }

                            // do grid lines
                            if (checkDoForDim(control.doGrid) && (cx == 0 || cz == 0)) {
                                if ((it->chunkX == 0) && (it->chunkZ == 0) && (cx == 0) && (cz == 0)) {
                                    color = htobe32(0xeb3333);
                                } else {
                                    color = htobe32(0xc1ffc4);
                                }
                            }

#ifdef PIXEL_COPY_MEMCPY
                            memcpy(&buf[((cz) * imageW + (imageX + cx)) * bpp], pcolor, bpp);
#else
                                                                                                                                                    // todobig - support for bpp here
              // todo - any use in optimizing the offset calc?
              buf[((cz) * imageW + (imageX + cx)) * 3] = pcolor[1];
              buf[((cz) * imageW + (imageX + cx)) * 3 + 1] = pcolor[2];
              buf[((cz) * imageW + (imageX + cx)) * 3 + 2] = pcolor[3];
#endif

                            // report interesting coordinates
                            if (dimId == kDimIdOverworld && imageMode == kImageModeTerrain) {
                                int32_t tix = (imageX + cx);
                                int32_t tiz = (imageZ + cz);
                                int32_t twx = (worldX + cx);
                                int32_t twz = (worldZ + cz);
                                if ((twx == 0) && (twz == 0)) {
                                    slogger.msg(kLogInfo1, "    Info: World (0, 0) is at image (%d, %d)\n", tix, tiz);
                                }
                                // todobig - just report this somwhere instead of having to pass the spawn params
                                if ((twx == worldSpawnX) && (twz == worldSpawnZ)) {
                                    slogger.msg(kLogInfo1, "    Info: World Spawn (%d, %d) is at image (%d, %d)\n",
                                                worldSpawnX, worldSpawnZ, tix, tiz);
                                }
                            }
                        }
                    }
                }
                // write rows
                outputPNG_writeRows(png, rows, 16);
            }

            // output the image
            outputPNG_close(png);

            delete[] buf;

            // report items that need to have their color set properly (in the XML file)
            if (imageMode == kImageModeTerrain) {
                for (int32_t i = 0; i < 512; i++) {
                    if (blockInfoList[i].colorSetNeedCount) {
                        slogger.msg(kLogInfo1, "    Need pixel color for: 0x%x '%s' (%d)\n", i,
                                    blockInfoList[i].name.c_str(), blockInfoList[i].colorSetNeedCount);
                    }
                }
            }
            return 0;
        }


        // adapted from: https://gist.github.com/protolambda/00b85bf34a75fd8176342b1ad28bfccc
        bool isSlimeChunk_MCPE(int32_t cX, int32_t cZ) {
            //
            // MCPE slime-chunk checker
            // From Minecraft: Pocket Edition 0.15.0 (0.15.0.50_V870150050)
            // Reverse engineered by @protolambda and @jocopa3
            //
            // NOTE:
            // - The world-seed doesn't seem to be incorporated into the randomness, which is very odd.
            //   This means that every world has its slime-chunks in the exact same chunks!
            //   This is not officially confirmed yet.
            // - Reverse engineering this code cost a lot of time,
            //   please add CREDITS when you are copying this.
            //   Copy the following into your program source:
            //     MCPE slime-chunk checker; reverse engineered by @protolambda and @jocopa3
            //

            // chunkX/Z are the chunk-coordinates, used in the DB keys etc.
            // Unsigned int32, using 64 bit longs to work-around the sign issue.
            int64_t chunkX_uint = cX & 0xffffffffL;
            int64_t chunkZ_uint = cZ & 0xffffffffL;

            // Combine X and Z into a 32 bit int (again, long to work around sign issue)
            int64_t seed = (chunkX_uint * 0x1f1f1f1fL) ^chunkZ_uint;

            // The random function MCPE uses, not the same as MCPC!
            // This is a Mersenne Twister; MT19937 by Takuji Nishimura and Makoto Matsumoto.
            // Java adaption source: http://dybfin.wustl.edu/teaching/compufinj/MTwister.java
            //MTwister random = new MTwister();
            std::mt19937 random;
            //random.init_genrand(seed);
            random.seed(seed);

            // The output of the random function, first operand of the asm umull instruction
            //int64_tn = random.genrand_int32();
            int64_t n = random();

            // The other operand, magic bit number that keeps characteristics
            // In binary: 1100 1100 1100 1100 1100 1100 1100 1101
            int64_t m = 0xcccccccdL;

            // umull (unsigned long multiplication)
            // Stores the result of multiplying two int32 integers in two registers (lo and hi).
            // Java workaround: store the result in a 64 bit long, instead of two 32 bit registers.
            int64_t product = n * m;

            // The umull instruction puts the result in a lo and a hi register, the lo one is not used.
            int64_t hi = (product >> 32) & 0xffffffffL;

            // Make room for 3 bits, preparation for decrease of randomness by a factor 10.
            int64_t hi_shift3 = (hi >> 0x3) & 0xffffffffL;

            // Multiply with 10 (3 bits)
            // ---> effect: the 3 bit randomness decrease expresses a 1 in a 10 chance.
            int64_t res = (((hi_shift3 + (hi_shift3 * 0x4)) & 0xffffffffL) * 0x2) & 0xffffffffL;

            // Final check: is the input equal to 10 times less random, but comparable, output.
            // Every chunk has a 1 in 10 chance to be a slime-chunk.
            return n == res;
        }


        int32_t generateImageSpecial(const std::string &fname, const ImageModeType imageMode) {
            const int32_t chunkW = (maxChunkX - minChunkX + 1);
            const int32_t chunkH = (maxChunkZ - minChunkZ + 1);
            const int32_t imageW = chunkW * 16;
            const int32_t imageH = chunkH * 16;

            JavaRandom rnd;
            int64_t rndseed;
            bool slimeChunkFlag = false;

            int32_t bpp = 3;
            bool rgbaFlag = false;
            if (imageMode == kImageModeSlimeChunksMCPC || imageMode == kImageModeSlimeChunksMCPE) {
                bpp = 4;
                rgbaFlag = true;
            }

            // note RGB pixels
            uint8_t *buf = new uint8_t[imageW * 16 * bpp];

            uint8_t *rows[16];
            for (int i = 0; i < 16; i++) {
                rows[i] = &buf[i * imageW * bpp];
            }

            PngWriter png;
            if (outputPNG_init(png, fname, makeImageDescription(imageMode, 0), imageW, imageH, rgbaFlag) != 0) {
                delete[] buf;
                return -1;
            }

            int32_t color;
            for (int32_t iz = 0, chunkZ = minChunkZ; iz < imageH; iz += 16, chunkZ++) {
                memset(buf, 0, imageW * 16 * bpp);
                for (int32_t ix = 0, chunkX = minChunkX; ix < imageW; ix += 16, chunkX++) {

                    if (0) {
                    } else if (imageMode == kImageModeSlimeChunksMCPC) {
                        /*
              from: http://minecraft.gamepedia.com/Slime_chunk#Low_layers
              Random rnd = new Random(seed +
              (long) (xPosition * xPosition * 0x4c1906) +
              (long) (xPosition * 0x5ac0db) +
              (long) (zPosition * zPosition) * 0x4307a7L +
              (long) (zPosition * 0x5f24f) ^ 0x3ad8025f);
              return rnd.nextInt(10) == 0;
            */
                        rndseed =
                                (worldSeed +
                                 (int64_t) (chunkX * chunkX * (int64_t) 0x4c1906) +
                                 (int64_t) (chunkX * (int64_t) 0x5ac0db) +
                                 (int64_t) (chunkZ * chunkZ * (int64_t) 0x4307a7) +
                                 (int64_t) (chunkZ * (int64_t) 0x5f24f)
                                )
                                ^ 0x3ad8025f;
                        rnd.setSeed(rndseed);
                        slimeChunkFlag = (rnd.nextInt(10) == 0);

                        if (slimeChunkFlag) {
                            color = (0xff << 24) | (0xff << 8);
                        } else {
                            color = 0;
                        }

                        for (int32_t sz = 0; sz < 16; sz++) {
                            for (int32_t sx = 0; sx < 16; sx++) {
                                memcpy(&buf[((sz) * imageW + (ix + sx)) * bpp], &color, bpp);
                            }
                        }
                    } else if (imageMode == kImageModeSlimeChunksMCPE) {
                        slimeChunkFlag = isSlimeChunk_MCPE(chunkX, chunkZ);

                        if (slimeChunkFlag) {
                            color = (0xff << 24) | (0xff << 8);
                        } else {
                            color = 0;
                        }

                        for (int32_t sz = 0; sz < 16; sz++) {
                            for (int32_t sx = 0; sx < 16; sx++) {
                                memcpy(&buf[((sz) * imageW + (ix + sx)) * bpp], &color, bpp);
                            }
                        }
                    }
                }
                outputPNG_writeRows(png, rows, 16);
            }

            // output the image
            outputPNG_close(png);

            delete[] buf;

            return 0;
        }

        // originally from: http://openlayers.org/en/v3.10.0/examples/shaded-relief.html
        // but that code is actually *quite* insane
        // rewritten based on:
        //   http://edndoc.esri.com/arcobjects/9.2/net/shared/geoprocessing/spatial_analyst_tools/how_hillshade_works.htm
        int32_t generateShadedRelief(const std::string &fnSrc, const std::string &fnDest) {

            //todobig - make these params
            double data_vert = 5;

            double data_sunEl = 45.0;
            double data_sunAz = 315;
            double data_resolution = 1;

            PngReader pngSrc;
            if (pngSrc.init(fnSrc) != 0) {
                slogger.msg(kLogInfo1, "ERROR: Failed to open src png");
                return -1;
            }

            pngSrc.read_info();

            int32_t srcW = pngSrc.getWidth();
            int32_t srcH = pngSrc.getHeight();
            int32_t colorType = pngSrc.getColorType();
            int32_t bppSrc = 3;
            if (colorType == PNG_COLOR_TYPE_RGB_ALPHA) {
                bppSrc = 4;
            }
            int32_t srcStride = srcW * bppSrc;

            uint8_t *sbuf = new uint8_t[srcStride * 3];

            // todobig - pngwriter support for 8-bit images (don't need RGBA for this)
            int32_t bppDest = 4;

            int32_t destW = srcW;
            int32_t destH = srcH;
            uint8_t *buf = new uint8_t[destW * bppDest];

            PngWriter pngOut;
            if (outputPNG_init(pngOut, fnDest, makeImageDescription(kImageModeShadedRelief, 0), destW, destH, true) !=
                0) {
                delete[] buf;
                pngSrc.close();
                delete[] sbuf;
                return -1;
            }

            /*
        uint8_t lut[256];

        double vmax = 128.0 * 128.0;
        for (int32_t i=0; i < 128; i++) {
        // (log( 1.0 + (double)(128 - i)/4.0 ) / logmax) * 255;
        double ti = (128 + 32) - i;
        double v = ((double)(ti * ti) / vmax) * 255.0;
        if ( v > 230.0 ) { v = 230.0; }
        if ( v < 0.0 ) { v = 0.0; }
        lut[i] = v;
        }
      */

            // weird - mingw32 doesn't get M_PI? - copied from math.h
#ifndef M_PI
#define M_PI            3.14159265358979323846
#endif

            int32_t maxX = srcW - 1;
            int32_t maxY = srcH - 1;
            double twoPi = 2.0 * M_PI;
            double halfPi = M_PI / 2.0;

            // (2)  Zenith_deg = 90 - Altitude
            // (3)  Zenith_rad = Zenith_deg * pi / 180.0
            double zenithRad = (90.0 - data_sunEl) * M_PI / 180.0;

            // (4)  Azimuth_math = 360.0 - Azimuth + 90
            double azimuthMath = 360.0 - data_sunAz + 90.0;
            // (5)  if Azimth_math >= 360.0 : Azimuth_math = Azimuth_math - 360.0
            if (azimuthMath >= 360.0) {
                azimuthMath = azimuthMath - 360.0;
            }
            // (6)  Azimuth_rad = Azimuth_math *  pi / 180.0
            double azimuthRad = azimuthMath * M_PI / 180.0;

            double cosZenithRad = cos(zenithRad);
            double sinZenithRad = sin(zenithRad);

            // todo - since we need to multiply x2 to expand 0..127 to 0..255 we instead halve this (would be 8)
            double dp = data_resolution *
                        4.0;  // data.resolution * 8; // todo - not totally sure about the use of resolution here

            // notes: negative values simply reverse the sun azimuth; the range of interesting values is fairly narrow - somewhere on (0.001..0.8)
            double zFactor = (data_vert / 10.0) - 0.075;

            int32_t x0, x2,
            //y0, y2,
                    offset;
            double z0, z2,
                    dzdx, dzdy,
                    slopeRad, aspectRad, hillshade, fhillshade;


            // prime the src buffers (first two rows are src row 0, then src row 1)
            png_read_row(pngSrc.png, &sbuf[0], NULL);
            memcpy(&sbuf[srcStride], &sbuf[0], srcStride);
            png_read_row(pngSrc.png, &sbuf[srcStride * 2], NULL);

            uint8_t *srcbuf0 = &sbuf[0];
            uint8_t *srcbuf1 = &sbuf[srcStride];
            uint8_t *srcbuf2 = &sbuf[srcStride * 2];

            for (int32_t y1 = 0; y1 < srcH; y1++) {
                // y0 = (y1 == 0) ? 0 : (y1 - 1);
                // y2 = (y1 == maxY) ? maxY : (y1 + 1);

                if (y1 > 0) {
                    // todo - this is slower than it needs to be, we could be clever about where we put the new row and not have to move stuff
                    // move data up
                    memcpy(&sbuf[0], &sbuf[srcStride], srcStride);
                    memcpy(&sbuf[srcStride], &sbuf[srcStride * 2], srcStride);
                    if (y1 < maxY) {
                        // read new row
                        png_read_row(pngSrc.png, &sbuf[srcStride * 2], NULL);
                    }
                }

                // clear output buffer
                memset(buf, 0, destW * bppDest);

                for (int32_t x1 = 0; x1 < srcW; x1++) {
                    x0 = (x1 == 0) ? 0 : (x1 - 1);
                    x2 = (x1 == maxX) ? maxX : (x1 + 1);

                    // z0 = a + 2d + g
                    z0 =
                            srcbuf0[x0 * bppSrc] +
                            srcbuf1[x0 * bppSrc] * 2.0 +
                            srcbuf2[x0 * bppSrc];

                    // z2 = c + 2f + i
                    z2 =
                            srcbuf0[x2 * bppSrc] +
                            srcbuf1[x2 * bppSrc] * 2.0 +
                            srcbuf2[x2 * bppSrc];

                    // (7)  [dz/dx] = ((c + 2f + i) - (a + 2d + g)) / (8 * cellsize)
                    dzdx = (z2 - z0) / dp;


                    // z0 = a + 2b + c
                    z0 =
                            srcbuf0[x0 * bppSrc] +
                            srcbuf0[x1 * bppSrc] * 2.0 +
                            srcbuf0[x2 * bppSrc];

                    // z2 = g + 2h + i
                    z2 =
                            srcbuf2[x0 * bppSrc] +
                            srcbuf2[x1 * bppSrc] * 2.0 +
                            srcbuf2[x2 * bppSrc];

                    // (8)  [dz/dy] = ((g + 2h + i) - (a + 2b + c))  / (8 * cellsize)
                    dzdy = (z2 - z0) / dp;

                    // (9)  Slope_rad = ATAN (z_factor * sqrt ([dz/dx]2 + [dz/dy]2))
                    slopeRad = atan(zFactor * sqrt(dzdx * dzdx + dzdy * dzdy));

                    if (dzdx != 0.0) {
                        aspectRad = atan2(dzdy, -dzdx);

                        if (aspectRad < 0) {
                            aspectRad += twoPi;
                        }
                    } else {
                        if (dzdy > 0.0) {
                            aspectRad = halfPi;
                        } else if (dzdy < 0.0) {
                            aspectRad = twoPi - halfPi;
                        } else {
                            // aspectRad is fine
                            aspectRad = 0.0; // todo - this is my guess; algo notes are ambiguous
                        }
                    }

                    // (1)  Hillshade = 255.0 * ((cos(Zenith_rad) * cos(Slope_rad)) +
                    //        (sin(Zenith_rad) * sin(Slope_rad) * cos(Azimuth_rad - Aspect_rad)))
                    // Note that if the calculation of Hillshade value is < 0, the cell value will be = 0.

                    // todo - worth doing a sin/cos LUT?
                    fhillshade = 255.0 * ((cosZenithRad * cos(slopeRad)) +
                                          (sinZenithRad * sin(slopeRad) * cos(azimuthRad - aspectRad)));

                    if (fhillshade < 0.0) {
                        hillshade = 0;
                    } else {
                        hillshade = round(fhillshade);
                    }

                    offset = (x1) * bppDest;
                    // rgb
                    buf[offset] =
                    buf[offset + 1] =
                    buf[offset + 2] = hillshade;

                    // alpha
                    // note: reduce the opacity for brighter parts; idea is to reduce haziness
                    // todo - adjust this -- we want BLACK to be opaque; WHITE to be transparent (or is it gray that should be transparent)
                    //buf[offset+3] = 255 - (hillshade / 2);
                    //buf[offset+3] = lut[ srcbuf1[x1 * bppSrc] ];
                    buf[offset + 3] = 255;
                }

                // output image data
                outputPNG_writeRow(pngOut, buf);

            }

            outputPNG_close(pngOut);

            delete[] buf;

            pngSrc.close();

            delete[] sbuf;

            return 0;
        }



        // a run on old code (generateMovie):
        // > time ./mcpe_viz --db another1/ --out test-all1/out1 --log log.tall1 --html-all
        // 2672.248u 5.461s 45:47.69 97.4% 0+0k 72+1424304io 0pf+0w
        // approx 45 minutes

        // a run on this new code (generateSlices):
        // 957.146u 4.463s 16:30.58 97.0%  0+0k 2536+1424304io 1pf+0w
        // approx 17 minutes

        // 823.212u 13.623s 14:23.86 96.8% 0+0k 2536+1423088io 1pf+0w
        // 830.614u 13.871s 14:52.42 94.6% 0+0k 2536+1423088io 1pf+0w

        // change memcpy to byte copy:
        // 828.757u 13.842s 14:29.31 96.9% 0+0k 2536+1422656io 1pf+0w

        // with -O3
        // 827.337u 13.430s 14:27.58 96.9% 0+0k 2568+1422656io 1pf+0w

        // with -O3 and png zlib set to 1 (default is 9) and filter set to NONE
        //   with zlib=9 == 405M    test-all2
        //   with zlib=1 == 508M    test-all2-zlib1/
        // 392.597u 13.598s 7:07.25 95.0%  0+0k 2568+1633896io 1pf+0w

        // as above, but disabled setting filter to NONE
        //   with zlib=1 == 572M    test-all2-zlib1/
        // 660.763u 13.799s 11:40.99 96.2% 0+0k 2568+1764624io 1pf+0w

        // 402.214u 13.937s 7:26.63 93.1%  0+0k 2568+1633896io 1pf+0w

        // without -O3, with zlib=1 and filter NONE
        // 404.518u 13.369s 7:25.09 93.8%  0+0k 2536+1633896io 1pf+0w

        // so we're at ~6.5x faster now

        // 2015.10.24:
        // 372.432u 13.435s 6:50.66 93.9%  0+0k 419456+1842944io 210pf+0w

        int32_t generateSlices(leveldb::DB *db, const std::string &fnBase) {
            const int32_t chunkOffsetX = -minChunkX;
            const int32_t chunkOffsetZ = -minChunkZ;

            const int32_t chunkW = (maxChunkX - minChunkX + 1);
            const int32_t chunkH = (maxChunkZ - minChunkZ + 1);
            const int32_t imageW = chunkW * 16;
            const int32_t imageH = chunkH * 16;

            char keybuf[128];
            int32_t keybuflen;
            int32_t kw = dimId;
            uint8_t kt = 0x30;
            uint8_t kt_v3 = 0x2f;
            leveldb::Status dstatus;

            slogger.msg(kLogInfo1, "    Writing all images in one pass\n");

            std::string svalue;

            int32_t color;
            const char *pcolor = (const char *) &color;

            int16_t *emuchunk = new int16_t[NUM_BYTES_CHUNK_V3];

            // create png helpers
            PngWriter png[MAX_BLOCK_HEIGHT + 1];
            for (int32_t cy = 0; cy <= MAX_BLOCK_HEIGHT; cy++) {
                std::string fnameTmp = fnBase + ".mcpe_viz_slice.full.";
                fnameTmp += name;
                fnameTmp += ".";
                sprintf(keybuf, "%03d", cy);
                fnameTmp += keybuf;
                fnameTmp += ".png";

                control.fnLayerRaw[dimId][cy] = fnameTmp;

                if (png[cy].init(fnameTmp, makeImageDescription(-1, cy), imageW, imageH, 16, false, true) != 0) {
                    delete[] emuchunk;
                    return -1;
                }
            }

            // create row buffers
            uint8_t *rbuf[MAX_BLOCK_HEIGHT + 1];
            for (int32_t cy = 0; cy <= MAX_BLOCK_HEIGHT; cy++) {
                rbuf[cy] = new uint8_t[(imageW * 3) * 16];
                // setup row pointers
                for (int32_t cz = 0; cz < 16; cz++) {
                    png[cy].row_pointers[cz] = &rbuf[cy][(cz * imageW) * 3];
                }
            }

            // create a helper buffer which contains topBlockY for the entire image
            uint8_t currTopBlockY = MAX_BLOCK_HEIGHT;
            size_t bsize = (size_t) imageW * (size_t) imageH;
            uint8_t *tbuf = new uint8_t[bsize];
            memset(tbuf, MAX_BLOCK_HEIGHT, bsize);
            for (const auto &it : chunks) {
                int32_t ix = (it.second->chunkX + chunkOffsetX) * 16;
                int32_t iz = (it.second->chunkZ + chunkOffsetZ) * 16;
                for (int32_t cz = 0; cz < 16; cz++) {
                    for (int32_t cx = 0; cx < 16; cx++) {
                        tbuf[(iz + cz) * imageW + (ix + cx)] = it.second->topBlockY[cx][cz];
                    }
                }
            };

            int32_t foundCt = 0, notFoundCt2 = 0;
            //todozooz -- new 16-bit block-id's (instead of 8-bit) are a BIG issue - this needs attention here
            uint8_t blockdata;
            int32_t blockid;

            // we operate on sets of 16 rows (which is one chunk high) of image z
            int32_t runCt = 0;
            for (int32_t imageZ = 0, chunkZ = minChunkZ; imageZ < imageH; imageZ += 16, chunkZ++) {

                if ((runCt++ % 20) == 0) {
                    slogger.msg(kLogInfo1, "    Row %d of %d\n", imageZ, imageH);
                }

                for (int32_t imageX = 0, chunkX = minChunkX; imageX < imageW; imageX += 16, chunkX++) {

                    // FIRST - we try pre-0.17 chunks

                    // construct key to get the chunk
                    if (dimId == kDimIdOverworld) {
                        //overworld
                        memcpy(&keybuf[0], &chunkX, sizeof(int32_t));
                        memcpy(&keybuf[4], &chunkZ, sizeof(int32_t));
                        memcpy(&keybuf[8], &kt, sizeof(uint8_t));
                        keybuflen = 9;
                    } else {
                        // nether (and probably any others that are added)
                        memcpy(&keybuf[0], &chunkX, sizeof(int32_t));
                        memcpy(&keybuf[4], &chunkZ, sizeof(int32_t));
                        memcpy(&keybuf[8], &kw, sizeof(int32_t));
                        memcpy(&keybuf[12], &kt, sizeof(uint8_t));
                        keybuflen = 13;
                    }

                    dstatus = db->Get(levelDbReadOptions, leveldb::Slice(keybuf, keybuflen), &svalue);
                    if (dstatus.ok()) {

                        // we got a pre-0.17 chunk
                        const char *ochunk = nullptr;
                        const char *pchunk = nullptr;

                        pchunk = svalue.data();
                        ochunk = pchunk;
                        // size_t ochunk_size = svalue.size();
                        foundCt++;

                        // we step through the chunk in the natural order to speed things up
                        for (int32_t cx = 0; cx < 16; cx++) {
                            for (int32_t cz = 0; cz < 16; cz++) {
                                currTopBlockY = tbuf[(imageZ + cz) * imageW + imageX + cx];
                                for (int32_t cy = 0; cy <= MAX_BLOCK_HEIGHT_127; cy++) {

                                    // todo - if we use this, we get blockdata errors... somethings not right
                                    //blockid = *(pchunk++);
                                    blockid = getBlockId_LevelDB_v2(ochunk, cx, cz, cy);

                                    if (blockid == 0 && (cy > currTopBlockY) && (dimId != kDimIdNether)) {

                                        // special handling for air -- keep existing value if we are above top block
                                        // the idea is to show air underground, but hide it above so that the map is not all black pixels @ y=MAX_BLOCK_HEIGHT
                                        // however, we do NOT do this for the nether. because: the nether

                                        // we need to copy this pixel from another layer
                                        memcpy(&rbuf[cy][((cz * imageW) + imageX + cx) * 3],
                                               &rbuf[currTopBlockY][((cz * imageW) + imageX + cx) * 3],
                                               3);

                                    } else {

                                        if (blockInfoList[blockid].hasVariants()) {
                                            // we need to get blockdata

                                            blockdata = getBlockData_LevelDB_v2(ochunk, cx, cz, cy);

                                            bool vfound = false;
                                            for (const auto &itbv : blockInfoList[blockid].variantList) {
                                                if (itbv->blockdata == blockdata) {
                                                    vfound = true;
                                                    color = itbv->color;
                                                    break;
                                                }
                                            }
                                            if (!vfound) {
                                                // todo - warn once per id/blockdata or the output volume could get ridiculous
                                                slogger.msg(kLogInfo1,
                                                            "WARNING: Did not find block variant for block (id=%d (0x%x) '%s') with blockdata=%d (0x%x) MSG2\n",
                                                            blockid, blockid, blockInfoList[blockid].name.c_str(),
                                                            blockdata, blockdata
                                                );

                                                // since we did not find the variant, use the parent block's color
                                                color = blockInfoList[blockid].color;
                                            }
                                        } else {
                                            color = blockInfoList[blockid].color;
                                        }

#ifdef PIXEL_COPY_MEMCPY
                                        memcpy(&rbuf[cy][((cz * imageW) + imageX + cx) * 3], &pcolor[1], 3);
#else
                                                                                                                                                                // todo - any use in optimizing the offset calc?
                    rbuf[cy][((cz*imageW) + imageX + cx)*3] = pcolor[1];
                    rbuf[cy][((cz*imageW) + imageX + cx)*3 + 1] = pcolor[2];
                    rbuf[cy][((cz*imageW) + imageX + cx)*3 + 2] = pcolor[3];
#endif
                                    }
                                }

                                // to support 256h worlds, for v2 chunks, we need to make 128..255 the same as 127
                                // todo - could optimize this
                                for (int cy = 128; cy <= MAX_BLOCK_HEIGHT; cy++) {
                                    memcpy(&rbuf[cy][((cz * imageW) + imageX + cx) * 3],
                                           &rbuf[127][((cz * imageW) + imageX + cx) * 3], 3);
                                }

                            }
                        }
                    } else {

                        // we did NOT find a pre-0.17 chunk...

                        // SECOND -- we try post 0.17 chunks

                        // we need to iterate over all possible y cubic chunks here...
                        int32_t cubicFoundCount = 0;
                        for (int8_t cubicy = 0; cubicy < MAX_CUBIC_Y; cubicy++) {

                            // todobug - this fails around level 112? on another1 -- weird -- run a valgrind to see where we're messing up
                            //check valgrind output

                            // construct key to get the chunk
                            if (dimId == kDimIdOverworld) {
                                //overworld
                                memcpy(&keybuf[0], &chunkX, sizeof(int32_t));
                                memcpy(&keybuf[4], &chunkZ, sizeof(int32_t));
                                memcpy(&keybuf[8], &kt_v3, sizeof(uint8_t));
                                memcpy(&keybuf[9], &cubicy, sizeof(uint8_t));
                                keybuflen = 10;
                            } else {
                                // nether (and probably any others that are added)
                                memcpy(&keybuf[0], &chunkX, sizeof(int32_t));
                                memcpy(&keybuf[4], &chunkZ, sizeof(int32_t));
                                memcpy(&keybuf[8], &kw, sizeof(int32_t));
                                memcpy(&keybuf[12], &kt_v3, sizeof(uint8_t));
                                memcpy(&keybuf[13], &cubicy, sizeof(uint8_t));
                                keybuflen = 14;
                            }

                            dstatus = db->Get(levelDbReadOptions, leveldb::Slice(keybuf, keybuflen), &svalue);
                            if (dstatus.ok()) {
                                cubicFoundCount++;

                                // we got a post-0.17 cubic chunk

                                const char *rchunk = svalue.data();
                                const int16_t *pchunk_word = (int16_t *) svalue.data();
                                const char *pchunk_byte = (char *) svalue.data();
                                size_t ochunk_size = svalue.size();
                                const int16_t *ochunk_word = pchunk_word;
                                const char *ochunk_byte = pchunk_byte;
                                bool wordModeFlag = false;
                                foundCt++;

                                // determine if it is a v7 chunk and process accordingly
                                //todozooz - here is where it gets weird
                                if (rchunk[0] != 0x0) {
                                    // we have a v7 chunk - emulate v3
                                    convertChunkV7toV3(rchunk, ochunk_size, emuchunk);
                                    wordModeFlag = true;
                                    pchunk_word = emuchunk;
                                    ochunk_word = emuchunk;
                                    ochunk_size = NUM_BYTES_CHUNK_V3;
                                } else {
                                    wordModeFlag = false;
                                    // slogger.msg(kLogWarning,"Found a non-v7 chunk\n");
                                }

                                // the first byte is not interesting to us (it is version #?)
                                pchunk_word++;
                                pchunk_byte++;

                                // we step through the chunk in the natural order to speed things up
                                for (int32_t cx = 0; cx < 16; cx++) {
                                    for (int32_t cz = 0; cz < 16; cz++) {
                                        currTopBlockY = tbuf[(imageZ + cz) * imageW + imageX + cx];
                                        for (int32_t ccy = 0; ccy < 16; ccy++) {
                                            int32_t cy = cubicy * 16 + ccy;

                                            // todo - if we use this, we get blockdata errors... somethings not right
                                            if (wordModeFlag) {
                                                blockid = *(pchunk_word++);
                                            } else {
                                                //todozooz - getting blockid manually fixes issue
                                                // blockid = *(pchunk_byte++);
                                                blockid = getBlockId_LevelDB_v3(ochunk_byte, cx, cz, ccy);
                                            }

                                            // blockid = getBlockId_LevelDB_v3(ochunk, cx,cz,ccy);

                                            if (blockid == 0 && (cy > currTopBlockY) && (dimId != kDimIdNether)) {

                                                // special handling for air -- keep existing value if we are above top block
                                                // the idea is to show air underground, but hide it above so that the map is not all black pixels @ y=MAX_BLOCK_HEIGHT
                                                // however, we do NOT do this for the nether. because: the nether

                                                // we need to copy this pixel from another layer
                                                memcpy(&rbuf[cy][((cz * imageW) + imageX + cx) * 3],
                                                       &rbuf[currTopBlockY][((cz * imageW) + imageX + cx) * 3],
                                                       3);

                                            } else {

                                                if (blockid >= 0 && blockid < 512) {
                                                    if (blockInfoList[blockid].hasVariants()) {
                                                        // we need to get blockdata

                                                        if (wordModeFlag) {
                                                            blockdata = getBlockData_LevelDB_v3__fake_v7(ochunk_word,
                                                                                                         ochunk_size,
                                                                                                         cx, cz, ccy);
                                                        } else {
                                                            blockdata = getBlockData_LevelDB_v3(ochunk_byte,
                                                                                                ochunk_size, cx, cz,
                                                                                                ccy);
                                                        }

                                                        bool vfound = false;
                                                        for (const auto &itbv : blockInfoList[blockid].variantList) {
                                                            if (itbv->blockdata == blockdata) {
                                                                vfound = true;
                                                                color = itbv->color;
                                                                break;
                                                            }
                                                        }
                                                        if (!vfound) {
                                                            // todo - warn once per id/blockdata or the output volume could get ridiculous
                                                            slogger.msg(kLogInfo1,
                                                                        "WARNING: Did not find block variant for block (id=%d (0x%x) '%s') with blockdata=%d (0x%x) MSG3\n",
                                                                        blockid, blockid,
                                                                        blockInfoList[blockid].name.c_str(), blockdata,
                                                                        blockdata
                                                            );
                                                            // since we did not find the variant, use the parent block's color
                                                            color = blockInfoList[blockid].color;
                                                        }
                                                    } else {
                                                        color = blockInfoList[blockid].color;
                                                    }
                                                } else {
                                                    // bad blockid
                                                    //todozooz todostopper - we get a lot of these w/ negative blockid around row 4800 of world 'another1'
                                                    slogger.msg(kLogError,
                                                                "Invalid blockid=%d (image %d %d) (cc %d %d %d)\n",
                                                                blockid, imageX, imageZ, cx, cz, cy
                                                    );
                                                    // set an unused color
                                                    color = htobe32(0xf010d0);
                                                }

#ifdef PIXEL_COPY_MEMCPY
                                                memcpy(&rbuf[cy][((cz * imageW) + imageX + cx) * 3], &pcolor[1], 3);
#else
                                                                                                                                                                        // todo - any use in optimizing the offset calc?
                        rbuf[cy][((cz*imageW) + imageX + cx)*3] = pcolor[1];
                        rbuf[cy][((cz*imageW) + imageX + cx)*3 + 1] = pcolor[2];
                        rbuf[cy][((cz*imageW) + imageX + cx)*3 + 2] = pcolor[3];
#endif
                                            }
                                        }
                                    }
                                }
                            } else {
                                // we did NOT find the cubic chunk, which means that it is 100% air

                                for (int32_t cx = 0; cx < 16; cx++) {
                                    for (int32_t cz = 0; cz < 16; cz++) {
                                        currTopBlockY = tbuf[(imageZ + cz) * imageW + imageX + cx];
                                        for (int32_t ccy = 0; ccy < 16; ccy++) {
                                            int32_t cy = cubicy * 16 + ccy;
                                            if ((cy > currTopBlockY) && (dimId != kDimIdNether)) {
                                                // special handling for air -- keep existing value if we are above top block
                                                // the idea is to show air underground, but hide it above so that the map is not all black pixels @ y=MAX_BLOCK_HEIGHT
                                                // however, we do NOT do this for the nether. because: the nether

                                                // we need to copy this pixel from another layer
                                                memcpy(&rbuf[cy][((cz * imageW) + imageX + cx) * 3],
                                                       &rbuf[currTopBlockY][((cz * imageW) + imageX + cx) * 3],
                                                       3);
                                            } else {
                                                memset(&rbuf[cy][((cz * imageW) + imageX + cx) * 3], 0, 3);
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        if (cubicFoundCount <= 0) {

                            // FINALLY -- we did not find the chunk at all
                            notFoundCt2++;
                            // slogger.msg(kLogInfo1,"WARNING: Did not find chunk in leveldb x=%d z=%d status=%s\n", chunkX, chunkZ, dstatus.ToString().c_str());

                            // we need to clear this area
                            for (int32_t cy = 0; cy <= MAX_BLOCK_HEIGHT; cy++) {
                                for (int32_t cz = 0; cz < 16; cz++) {
                                    memset(&rbuf[cy][((cz * imageW) + imageX) * 3], 0, 16 * 3);
                                }
                            }
                            // todonow - need this?
                            //continue;
                        }
                    }

                }

                // put the png rows
                // todo - png lib is SLOW - worth it to alloc a larger window (16-row increments) and write in batches?
                for (int32_t cy = 0; cy <= MAX_BLOCK_HEIGHT; cy++) {
                    png_write_rows(png[cy].png, png[cy].row_pointers, 16);
                }
            }

            for (int32_t cy = 0; cy <= MAX_BLOCK_HEIGHT; cy++) {
                delete[] rbuf[cy];
                png[cy].close();
            }

            delete[] tbuf;

            // slogger.msg(kLogInfo1,"    Chunk Info: Found = %d / Not Found (our list) = %d / Not Found (leveldb) = %d\n", foundCt, notFoundCt1, notFoundCt2);

            delete[] emuchunk;
            return 0;
        }


        int32_t generateMovie(leveldb::DB *db, const std::string &fnBase, const std::string &fnOut, bool makeMovieFlag,
                              bool useCropFlag) {
            const int32_t chunkOffsetX = -minChunkX;
            const int32_t chunkOffsetZ = -minChunkZ;

            const int32_t chunkW = (maxChunkX - minChunkX + 1);
            const int32_t chunkH = (maxChunkZ - minChunkZ + 1);
            const int32_t imageW = chunkW * 16;
            const int32_t imageH = chunkH * 16;

            int32_t divisor = 1;
            if (dimId == kDimIdNether) {
                // if nether, we divide coordinates by 8
                divisor = 8;
            }

            int32_t cropX, cropZ, cropW, cropH;

            if (useCropFlag) {
                cropX = control.movieX / divisor;
                cropZ = control.movieY / divisor;
                cropW = control.movieW / divisor;
                cropH = control.movieH / divisor;
            } else {
                cropX = cropZ = 0;
                cropW = imageW;
                cropH = imageH;
            }

            // note RGB pixels
            uint8_t *buf = new uint8_t[cropW * cropH * 3];
            memset(buf, 0, cropW * cropH * 3);

            // todobig - we *could* write image data to flat files during dbParse and then convert
            //   these flat files into png here (but temp disk space requirements are *huge*); could try gzwrite etc

            std::string svalue;
            const char *pchunk = nullptr;
            int32_t pchunkX = 0;
            int32_t pchunkZ = 0;

            int32_t color;
            const char *pcolor = (const char *) &color;
            for (int32_t cy = 0; cy <= MAX_BLOCK_HEIGHT; cy++) {
                // todo - make this part a func so that user can ask for specific slices from the cmdline?
                slogger.msg(kLogInfo1, "  Layer %d\n", cy);
                for (const auto &it : chunks) {
                    int32_t imageX = (it.second->chunkX + chunkOffsetX) * 16;
                    int32_t imageZ = (it.second->chunkZ + chunkOffsetZ) * 16;

                    for (int32_t cz = 0; cz < 16; cz++) {
                        int32_t iz = (imageZ + cz);

                        for (int32_t cx = 0; cx < 16; cx++) {
                            int32_t ix = (imageX + cx);

                            if (!useCropFlag ||
                                ((ix >= cropX) && (ix < (cropX + cropW)) && (iz >= cropZ) && (iz < (cropZ + cropH)))) {

                                if (pchunk == nullptr || (pchunkX != it.second->chunkX) ||
                                    (pchunkZ != it.second->chunkZ)) {
                                    // get the chunk
                                    // construct key
                                    char keybuf[20];
                                    int32_t keybuflen;
                                    int32_t kx = it.second->chunkX, kz = it.second->chunkZ, kw = dimId;
                                    //todohere todostopper - needs attention for 256h
                                    uint8_t kt = 0x30;
                                    switch (dimId) {
                                        case kDimIdOverworld:
                                            //overworld
                                            memcpy(&keybuf[0], &kx, sizeof(int32_t));
                                            memcpy(&keybuf[4], &kz, sizeof(int32_t));
                                            memcpy(&keybuf[8], &kt, sizeof(uint8_t));
                                            keybuflen = 9;
                                            break;
                                        default:
                                            // nether
                                            memcpy(&keybuf[0], &kx, sizeof(int32_t));
                                            memcpy(&keybuf[4], &kz, sizeof(int32_t));
                                            memcpy(&keybuf[8], &kw, sizeof(int32_t));
                                            memcpy(&keybuf[12], &kt, sizeof(uint8_t));
                                            keybuflen = 13;
                                            break;
                                    }
                                    leveldb::Slice key(keybuf, keybuflen);
                                    leveldb::Status dstatus = db->Get(levelDbReadOptions, key, &svalue);
                                    if (!dstatus.ok()) {
                                        slogger.msg(kLogInfo1, "WARNING: LevelDB operation returned status=%s\n",
                                                    dstatus.ToString().c_str());
                                    }
                                    pchunk = svalue.data();
                                    pchunkX = it.second->chunkX;
                                    pchunkZ = it.second->chunkZ;
                                }

                                uint8_t blockid = getBlockId_LevelDB_v2(pchunk, cx, cz, cy);

                                if (blockid == 0 && (cy > it.second->topBlockY[cx][cz]) && (dimId != kDimIdNether)) {
                                    // special handling for air -- keep existing value if we are above top block
                                    // the idea is to show air underground, but hide it above so that the map is not all black pixels @ y=MAX_BLOCK_HEIGHT
                                    // however, we do NOT do this for the nether. because: the nether
                                } else {

                                    if (blockInfoList[blockid].hasVariants()) {
                                        // we need to get blockdata
                                        int32_t blockdata = it.second->data[cx][cz];
                                        bool vfound = false;
                                        for (const auto &itbv : blockInfoList[blockid].variantList) {
                                            if (itbv->blockdata == blockdata) {
                                                vfound = true;
                                                color = itbv->color;
                                                break;
                                            }
                                        }
                                        if (!vfound) {
                                            // todo - warn once per id/blockdata or the output volume could get ridiculous
                                            slogger.msg(kLogInfo1,
                                                        "WARNING: Did not find block variant for block (id=%d (0x%x) '%s') with blockdata=%d (0x%x) MSG4\n",
                                                        blockid, blockid, blockInfoList[blockid].name.c_str(),
                                                        blockdata, blockdata
                                            );

                                            // since we did not find the variant, use the parent block's color
                                            color = blockInfoList[blockid].color;
                                        }
                                    } else {
                                        color = blockInfoList[blockid].color;
                                    }

                                    // do grid lines
                                    if (checkDoForDim(control.doGrid) && (cx == 0 || cz == 0)) {
                                        if ((it.second->chunkX == 0) && (it.second->chunkZ == 0) && (cx == 0) &&
                                            (cz == 0)) {
                                            // highlight (0,0)
                                            color = htobe32(0xeb3333);
                                        } else {
                                            color = htobe32(0xc1ffc4);
                                        }
                                    }

#ifdef PIXEL_COPY_MEMCPY
                                    memcpy(&buf[(((imageZ + cz) - cropZ) * cropW + ((imageX + cx) - cropX)) * 3],
                                           &pcolor[1], 3);
#else
                                                                                                                                                            // todo - any use in optimizing the offset calc?
                  buf[ (((imageZ + cz) - cropZ) * cropW + ((imageX + cx) - cropX)) * 3] = pcolor[1];
                  buf[ (((imageZ + cz) - cropZ) * cropW + ((imageX + cx) - cropX)) * 3 + 1] = pcolor[2];
                  buf[ (((imageZ + cz) - cropZ) * cropW + ((imageX + cx) - cropX)) * 3 + 2] = pcolor[3];
#endif
                                }
                            }
                        }
                    }
                }

                // output the image
                std::string fnameTmp = fnBase + ".mcpe_viz_slice.";
                if (!makeMovieFlag) {
                    fnameTmp += "full.";
                }
                fnameTmp += name;
                fnameTmp += ".";
                char xtmp[100];
                sprintf(xtmp, "%03d", cy);
                fnameTmp += xtmp;
                fnameTmp += ".png";

                control.fnLayerRaw[dimId][cy] = fnameTmp;

                outputPNG(fnameTmp, makeImageDescription(-1, cy), buf, cropW, cropH, false);
            }

            delete[] buf;

            if (makeMovieFlag) {
                // "ffmpeg" method
                std::string fnameTmp = fnBase + ".mcpe_viz_slice.";
                fnameTmp += name;
                fnameTmp += ".%03d.png";

                // todo - ffmpeg on win32? need bin path option?
                // todo - provide other user options for ffmpeg cmd line params?
                std::string cmdline = std::string("ffmpeg -y -framerate 1 -i " + fnameTmp + " -c:v libx264 -r 30 ");
                cmdline += fnOut;
                int32_t ret = system(cmdline.c_str());
                if (ret != 0) {
                    slogger.msg(kLogInfo1, "Failed to create movie ret=(%d) cmd=(%s)\n", ret, cmdline.c_str());
                }

                // todo - delete temp slice files? cmdline option to NOT delete
            }

            return 0;
        }

        int32_t doOutput_GeoJSON() {
            // put spawnable info
            for (const auto &it : listCheckSpawn) {
                // spwawnable! add it to the list
                double ix, iy;
                char tmpstring[512];
                worldPointToGeoJSONPoint(dimId, it->x, it->z, ix, iy);
                sprintf(tmpstring, ""
                                   "\"Spawnable\":true,"
                                   "\"Name\":\"SpawnableBoundingCircle\","
                                   "\"BoundingCircle\":\"1\","
                                   "\"Clickable\":\"0\","
                                   "\"Dimension\":\"%d\","
                                   "\"Radius\":\"%d\","
                                   "\"Pos\":[%d,%d,%d]"
                                   "}}", dimId, it->distance, it->x, 0, it->z
                );
                std::string json = ""
                                   + makeGeojsonHeader(ix, iy)
                                   + tmpstring;
                listGeoJSON.push_back(json);
            }

            return 0;
        }

        int32_t doOutput_Schematic(leveldb::DB *db) {
            for (const auto &schematic : listSchematic) {
                int32_t sizex = schematic->x2 - schematic->x1 + 1;
                int32_t sizey = schematic->y2 - schematic->y1 + 1;
                int32_t sizez = schematic->z2 - schematic->z1 + 1;

                // std::vector<int8_t>
                nbt::tag_byte_array blockArray;
                nbt::tag_byte_array blockDataArray;

                char keybuf[128];
                int32_t keybuflen;
                int32_t kw = dimId;
                //todohere todostopper - needs attention for 256h
                uint8_t kt = 0x30;
                leveldb::Status dstatus;

                slogger.msg(kLogInfo1, "  Processing Schematic: %s\n", schematic->toString().c_str());

                std::string svalue;
                const char *pchunk = nullptr;

                //int32_t color;
                //const char *pcolor = (const char*)&color;


                int32_t foundCt = 0, notFoundCt2 = 0;
                uint8_t blockid, blockdata;

                int32_t prevChunkX = 0;
                int32_t prevChunkZ = 0;
                bool prevChunkValid = false;

                // todozzz - if schematic area is larger than one chunk (65k byte array limit), then create multiple chunk-sized schematic files and name then .schematic.11.22 (where 11=x_chunk & 22=z_chunk)

                for (int32_t imageY = schematic->y1; imageY <= schematic->y2; imageY++) {

                    for (int32_t imageZ = schematic->z1; imageZ <= schematic->z2; imageZ++) {
                        int32_t chunkZ = imageZ / 16;
                        int32_t coz = imageZ % 16;

                        for (int32_t imageX = schematic->x1; imageX <= schematic->x2; imageX++) {
                            int32_t chunkX = imageX / 16;
                            int32_t cox = imageX % 16;

                            if (prevChunkValid && (chunkX == prevChunkX) && (chunkZ == prevChunkZ)) {
                                // we already have the chunk
                            } else {
                                // we need to read the chunk

                                prevChunkValid = false;

                                // construct key to get the chunk
                                if (dimId == kDimIdOverworld) {
                                    //overworld
                                    memcpy(&keybuf[0], &chunkX, sizeof(int32_t));
                                    memcpy(&keybuf[4], &chunkZ, sizeof(int32_t));
                                    memcpy(&keybuf[8], &kt, sizeof(uint8_t));
                                    keybuflen = 9;
                                } else {
                                    // nether (and probably any others that are added)
                                    memcpy(&keybuf[0], &chunkX, sizeof(int32_t));
                                    memcpy(&keybuf[4], &chunkZ, sizeof(int32_t));
                                    memcpy(&keybuf[8], &kw, sizeof(int32_t));
                                    memcpy(&keybuf[12], &kt, sizeof(uint8_t));
                                    keybuflen = 13;
                                }

                                dstatus = db->Get(levelDbReadOptions, leveldb::Slice(keybuf, keybuflen), &svalue);
                                if (!dstatus.ok()) {
                                    notFoundCt2++;
                                    slogger.msg(kLogInfo1,
                                                "WARNING: Did not find chunk in leveldb x=%d z=%d status=%s\n", chunkX,
                                                chunkZ, dstatus.ToString().c_str());
                                    blockArray.push_back(0);
                                    blockDataArray.push_back(0);
                                    continue;
                                }

                                pchunk = svalue.data();

                                prevChunkValid = true;
                                prevChunkX = chunkX;
                                prevChunkZ = chunkZ;
                                foundCt++;
                            }

                            blockid = getBlockId_LevelDB_v2(pchunk, cox, coz, imageY);
                            blockdata = getBlockData_LevelDB_v2(pchunk, cox, coz, imageY);

                            blockArray.push_back(blockid);
                            blockDataArray.push_back(blockdata);
                        }
                    }
                }

                std::string fnOut = control.fnOutputBase + ".schematic." + schematic->fn + ".nbt";

                writeSchematicFile(fnOut, sizex, sizey, sizez, blockArray, blockDataArray);

                //slogger.msg(kLogInfo1,"    Chunk Info: Found = %d / Not Found (our list) = %d / Not Found (leveldb) = %d\n", foundCt, notFoundCt1, notFoundCt2);
            }
            return 0;
        }


        int32_t doOutput(leveldb::DB *db) {
            slogger.msg(kLogInfo1, "Do Output: %s\n", name.c_str());

            doOutputStats();

            doOutput_GeoJSON();

            // we put images in subdir
            std::string fnBase = mybasename(control.fnOutputBase);
            std::string dirOut = mydirname(control.fnOutputBase) + "/images";
            local_mkdir(dirOut.c_str());

            slogger.msg(kLogInfo1, "  Generate Image\n");
            control.fnLayerTop[dimId] = std::string(dirOut + "/" + fnBase + "." + name + ".map.png");
            generateImage(control.fnLayerTop[dimId], kImageModeTerrain);

            if (checkDoForDim(control.doImageBiome)) {
                slogger.msg(kLogInfo1, "  Generate Biome Image\n");
                control.fnLayerBiome[dimId] = std::string(dirOut + "/" + fnBase + "." + name + ".biome.png");
                generateImage(control.fnLayerBiome[dimId], kImageModeBiome);
            }
            if (checkDoForDim(control.doImageGrass)) {
                slogger.msg(kLogInfo1, "  Generate Grass Image\n");
                control.fnLayerGrass[dimId] = std::string(dirOut + "/" + fnBase + "." + name + ".grass.png");
                generateImage(control.fnLayerGrass[dimId], kImageModeGrass);
            }
            if (checkDoForDim(control.doImageHeightCol)) {
                slogger.msg(kLogInfo1, "  Generate Height Column Image\n");
                control.fnLayerHeight[dimId] = std::string(dirOut + "/" + fnBase + "." + name + ".height_col.png");
                generateImage(control.fnLayerHeight[dimId], kImageModeHeightCol);
            }
            if (checkDoForDim(control.doImageHeightColGrayscale)) {
                slogger.msg(kLogInfo1, "  Generate Height Column (grayscale) Image\n");
                control.fnLayerHeightGrayscale[dimId] = std::string(
                        dirOut + "/" + fnBase + "." + name + ".height_col_grayscale.png");
                generateImage(control.fnLayerHeightGrayscale[dimId], kImageModeHeightColGrayscale);
            }
            if (checkDoForDim(control.doImageHeightColAlpha)) {
                slogger.msg(kLogInfo1, "  Generate Height Column (alpha) Image\n");
                control.fnLayerHeightAlpha[dimId] = std::string(
                        dirOut + "/" + fnBase + "." + name + ".height_col_alpha.png");
                generateImage(control.fnLayerHeightAlpha[dimId], kImageModeHeightColAlpha);
            }
            if (checkDoForDim(control.doImageLightBlock)) {
                slogger.msg(kLogInfo1, "  Generate Block Light Image\n");
                control.fnLayerBlockLight[dimId] = std::string(dirOut + "/" + fnBase + "." + name + ".light_block.png");
                generateImage(control.fnLayerBlockLight[dimId], kImageModeBlockLight);
            }
            if (checkDoForDim(control.doImageLightSky)) {
                slogger.msg(kLogInfo1, "  Generate Sky Light Image\n");
                control.fnLayerSkyLight[dimId] = std::string(dirOut + "/" + fnBase + "." + name + ".light_sky.png");
                generateImage(control.fnLayerSkyLight[dimId], kImageModeSkyLight);
            }
            if (checkDoForDim(control.doImageSlimeChunks)) {
                slogger.msg(kLogInfo1, "  Generate Slime Chunks Image\n");
                control.fnLayerSlimeChunks[dimId] = std::string(
                        dirOut + "/" + fnBase + "." + name + ".slime_chunks.png");
                generateImageSpecial(control.fnLayerSlimeChunks[dimId], kImageModeSlimeChunksMCPE);
            }

            if (checkDoForDim(control.doImageShadedRelief)) {
                slogger.msg(kLogInfo1, "  Generate Shaded Relief Image\n");
                control.fnLayerShadedRelief[dimId] = std::string(
                        dirOut + "/" + fnBase + "." + name + ".shaded_relief.png");

                if (false) {
                    // todobig - idea is to oversample the src image and then get higher resolution shaded relief - but, openlayers does not cooperate with this idea :) -- could fiddle with it later
                    // todo - param for oversample
                    std::string fnTemp = std::string(dirOut + "/" + fnBase + "." + name + ".shaded_relief.temp.png");
                    if (oversampleImage(control.fnLayerHeightGrayscale[dimId], fnTemp, 2) == 0) {
                        generateShadedRelief(fnTemp, control.fnLayerShadedRelief[dimId]);
                        // remove temporary file
                        deleteFile(fnTemp);
                    }
                } else {
                    generateShadedRelief(control.fnLayerHeightGrayscale[dimId], control.fnLayerShadedRelief[dimId]);
                }
            }

            if (checkDoForDim(control.doMovie)) {
                slogger.msg(kLogInfo1, "  Generate movie\n");
                generateMovie(db, dirOut + "/" + fnBase, std::string(control.fnOutputBase + "." + name + ".mp4"), true,
                              true);
            }

            if (checkDoForDim(control.doSlices)) {
                slogger.msg(kLogInfo1, "  Generate full-size slices\n");
                generateSlices(db, dirOut + "/" + fnBase);
            }

            doOutput_Schematic(db);

            // reset
            for (int32_t i = 0; i < 512; i++) {
                blockInfoList[i].colorSetNeedCount = 0;
            }

            return 0;
        }
    };


    // todobig - move to util?
    int32_t
    printKeyValue(const char *key, int32_t key_size, const char *value, int32_t value_size, bool printKeyAsStringFlag) {
        logger.msg(kLogInfo1, "WARNING: Unparsed Record: key_size=%d key_string=[%s] key_hex=[", key_size,
                   (printKeyAsStringFlag ? key : "(SKIPPED)"));
        for (int32_t i = 0; i < key_size; i++) {
            if (i > 0) { logger.msg(kLogInfo1, " "); }
            logger.msg(kLogInfo1, "%02x", ((int) key[i] & 0xff));
        }
        logger.msg(kLogInfo1, "] value_size=%d value_hex=[", value_size);
        for (int32_t i = 0; i < value_size; i++) {
            if (i > 0) { logger.msg(kLogInfo1, " "); }
            logger.msg(kLogInfo1, "%02x", ((int) value[i] & 0xff));
        }
        logger.msg(kLogInfo1, "]\n");
        return 0;
    }


    // note: this is an attempt to remove "bad" chunks as seen in "nyan.zip" world
    inline bool legalChunkPos(int32_t chunkX, int32_t chunkZ) {
        if ((uint32_t) chunkX == 0x80000000 && (uint32_t) chunkZ == 0x80000000) {
            return false;
        }
        return true;
    }


    // base class for a minecraft world
    class MinecraftWorld {
    private:
        std::string worldName;
        int64_t worldSeed;
        int32_t worldSpawnX;
        int32_t worldSpawnY;
        int32_t worldSpawnZ;

    public:
        MinecraftWorld() {
            worldName = "UNKNOWN-NOT-SET";
            worldSeed = 0;
            worldSpawnX = 0;
            worldSpawnY = 0;
            worldSpawnZ = 0;
        }

        void setWorldName(const std::string &s) { worldName = s; }

        const std::string &getWorldName() const { return worldName; }

        void setWorldSeed(int64_t s) { worldSeed = s; }

        int64_t getWorldSeed() const { return worldSeed; }

        void setWorldSpawnX(int32_t v) { worldSpawnX = v; }

        int32_t getWorldSpawnX() const { return worldSpawnX; }

        void setWorldSpawnY(int32_t v) { worldSpawnY = v; }

        int32_t getWorldSpawnY() const { return worldSpawnY; }

        void setWorldSpawnZ(int32_t v) { worldSpawnZ = v; }

        int32_t getWorldSpawnZ() const { return worldSpawnZ; }
    };


    class MinecraftWorld_LevelDB : public MinecraftWorld {
    private:
        leveldb::DB *db;
        std::unique_ptr<leveldb::Options> dbOptions;
        int32_t totalRecordCt;

    public:
        // todobig - move to private?
        std::unique_ptr<DimensionData_LevelDB> dimDataList[kDimIdCount];

        MinecraftWorld_LevelDB() {
            db = nullptr;

            levelDbReadOptions.fill_cache = false;
            // suggestion from leveldb/mcpe_sample_setup.cpp
            levelDbReadOptions.decompress_allocator = new leveldb::DecompressAllocator();


            dbOptions = std::unique_ptr<leveldb::Options>(new leveldb::Options);
            //dbOptions->compressors[0] = new leveldb::ZlibCompressor();
            dbOptions->create_if_missing = false;

            // this filter is supposed to reduce disk reads - light testing indicates that it is faster when doing 'html-all'
            if (control.leveldbFilter > 0) {
                dbOptions->filter_policy = leveldb::NewBloomFilterPolicy(control.leveldbFilter);
            }

            dbOptions->block_size = control.leveldbBlockSize;

            // start: suggestions from leveldb/mcpe_sample_setup.cpp
            //create a 40 mb cache (we use this on ~1gb devices)
            dbOptions->block_cache = leveldb::NewLRUCache(40 * 1024 * 1024);

            //create a 4mb write buffer, to improve compression and touch the disk less
            dbOptions->write_buffer_size = 4 * 1024 * 1024;

            //disable internal logging. The default logger will still print out things to a file
            dbOptions->info_log = new NullLogger();

            //use the new raw-zip compressor to write (and read)
//      dbOptions->compressors[0] = new leveldb::ZlibCompressorRaw(-1);
            dbOptions->compression = leveldb::kZlibRawCompression;
            //also setup the old, slower compressor for backwards compatibility. This will only be used to read old compressed blocks.
//      dbOptions->compressors[1] = new leveldb::ZlibCompressor();
            // end: suggestions from leveldb/mcpe_sample_setup.cpp


            for (int32_t i = 0; i < kDimIdCount; i++) {
                dimDataList[i] = std::unique_ptr<DimensionData_LevelDB>(new DimensionData_LevelDB());
                dimDataList[i]->setDimId(i);
                dimDataList[i]->unsetChunkBoundsValid();
            }
            dimDataList[kDimIdOverworld]->setName("overworld");
            dimDataList[kDimIdNether]->setName("nether");
            dimDataList[kDimIdTheEnd]->setName("the-end");
        }

        ~MinecraftWorld_LevelDB() {
            dbClose();
        }

        int32_t parseLevelFile(const std::string &fname) {
            FILE *fp = fopen(fname.c_str(), "rb");
            if (!fp) {
                slogger.msg(kLogInfo1, "ERROR: Failed to open input file (fn=%s error=%s (%d))\n", fname.c_str(),
                            strerror(errno), errno);
                return -1;
            }

            int32_t fVersion;
            int32_t bufLen;
            fread(&fVersion, sizeof(int32_t), 1, fp);
            fread(&bufLen, sizeof(int32_t), 1, fp);

            slogger.msg(kLogInfo1, "parseLevelFile: name=%s version=%d len=%d\n", fname.c_str(), fVersion, bufLen);

            int32_t ret = -2;
            if (bufLen > 0) {
                // read content
                char *buf = new char[bufLen];
                fread(buf, 1, bufLen, fp);
                fclose(fp);

                MyNbtTagList tagList;
                ret = parseNbt("level.dat: ", buf, bufLen, tagList);

                if (ret == 0) {
                    nbt::tag_compound tc = tagList[0].second->as<nbt::tag_compound>();

                    setWorldSpawnX(tc["SpawnX"].as<nbt::tag_int>().get());
                    setWorldSpawnY(tc["SpawnY"].as<nbt::tag_int>().get());
                    setWorldSpawnZ(tc["SpawnZ"].as<nbt::tag_int>().get());
                    slogger.msg(kLogInfo1, "  Found World Spawn: x=%d y=%d z=%d\n", getWorldSpawnX(), getWorldSpawnY(),
                                getWorldSpawnZ());

                    setWorldSeed(tc["RandomSeed"].as<nbt::tag_long>().get());
                }

                delete[] buf;
            } else {
                fclose(fp);
            }

            return ret;
        }

        int32_t parseLevelName(const std::string &fname) {
            FILE *fp = fopen(fname.c_str(), "r");
            if (!fp) {
                slogger.msg(kLogInfo1, "ERROR: Failed to open input file (fn=%s error=%s (%d))\n", fname.c_str(),
                            strerror(errno), errno);
                return -1;
            }

            char buf[1025];
            memset(buf, 0, 1025);
            fgets(buf, 1024, fp);

            setWorldName(buf);

            slogger.msg(kLogInfo1, "  Level name is [%s]\n", (strlen(buf) > 0) ? buf : "(UNKNOWN)");
            logger.msg(kLogInfo1, "\nlevelname.txt: Level name is [%s]\n", (strlen(buf) > 0) ? buf : "(UNKNOWN)");
            fclose(fp);

            return 0;
        }

        int32_t init() {
            int32_t ret;

            ret = parseLevelFile(std::string(control.dirLeveldb + "/level.dat"));
            if (ret != 0) {
                slogger.msg(kLogInfo1, "ERROR: Failed to parse level.dat file.  Exiting...\n");
                slogger.msg(kLogInfo1, "** Hint: --db must point to the dir which contains level.dat\n");
                return -1;
            }

            ret = parseLevelName(std::string(control.dirLeveldb + "/levelname.txt"));
            if (ret != 0) {
                slogger.msg(kLogInfo1, "WARNING: Failed to parse levelname.txt file.\n");
                slogger.msg(kLogInfo1, "** Hint: --db must point to the dir which contains levelname.txt\n");
            }

            // update dimension data
            for (int32_t i = 0; i < kDimIdCount; i++) {
                dimDataList[i]->setWorldInfo(getWorldName(), getWorldSpawnX(), getWorldSpawnZ(), getWorldSeed());
            }

            return 0;
        }

        int32_t dbOpen(const std::string &dirDb) {
            // todobig - leveldb read-only? snapshot?
            slogger.msg(kLogInfo1, "DB Open: dir=%s\n", dirDb.c_str());
            leveldb::Status dstatus = leveldb::DB::Open(*dbOptions, std::string(dirDb + "/db"), &db);
            slogger.msg(kLogInfo1, "DB Open Status: %s (block_size=%d bloom_filter_bits=%d)\n",
                        dstatus.ToString().c_str(), control.leveldbBlockSize, control.leveldbFilter);
            fflush(stderr);
            if (!dstatus.ok()) {
                slogger.msg(kLogInfo1, "ERROR: LevelDB operation returned status=%s\n", dstatus.ToString().c_str());
                exit(-2);
            }
            return 0;
        }

        int32_t dbClose() {
            if (db != nullptr) {
                delete db;
                db = nullptr;
            }
            // todonow - disabled for now - crashes
            if (false) {
                if (dbOptions != nullptr) {
//          if ( dbOptions->compressors[1] ) {
//            delete dbOptions->compressors[1];
//            dbOptions->compressors[0] = nullptr;
//          }
//          if ( dbOptions->compressors[0] ) {
//            delete dbOptions->compressors[0];
//            dbOptions->compressors[0] = nullptr;
//          }
                    if (dbOptions->filter_policy != NULL) {
                        delete dbOptions->filter_policy;
                        dbOptions->filter_policy = NULL;
                    }
                }
            }
            return 0;
        }

        int32_t calcChunkBounds() {
            // see if we already calculated bounds
            bool passFlag = true;
            for (int32_t i = 0; i < kDimIdCount; i++) {
                if (!dimDataList[i]->getChunkBoundsValid()) {
                    passFlag = false;
                }
            }
            if (passFlag) {
                return 0;
            }

            // clear bounds
            for (int32_t i = 0; i < kDimIdCount; i++) {
                dimDataList[i]->unsetChunkBoundsValid();
            }

            int32_t chunkX = -1, chunkZ = -1, chunkDimId = -1, chunkType = -1;

            slogger.msg(kLogInfo1, "Scan keys to get world boundaries\n");
            int32_t recordCt = 0;

            // todobig - is there a faster way to enumerate the keys?
            leveldb::Iterator *iter = db->NewIterator(levelDbReadOptions);
            leveldb::Slice skey;
            int32_t key_size;
            const char *key;
            for (iter->SeekToFirst(); iter->Valid(); iter->Next()) {
                skey = iter->key();
                key_size = skey.size();
                key = skey.data();

                ++recordCt;
                if (control.shortRunFlag && recordCt > 1000) {
                    break;
                }

                if (key_size == 9) {
                    chunkX = myParseInt32(key, 0);
                    chunkZ = myParseInt32(key, 4);
                    chunkType = myParseInt8(key, 8);

                    // sanity checks
                    if (chunkType == 0x30) {
                        // pre-0.17 chunk block data
                        if (legalChunkPos(chunkX, chunkZ)) {
                            dimDataList[0]->addToChunkBounds(chunkX, chunkZ);
                        }
                    }
                }
                if (key_size == 10) {
                    // post-0.17 chunk block data
                    chunkX = myParseInt32(key, 0);
                    chunkZ = myParseInt32(key, 4);
                    chunkType = myParseInt8(key, 8);

                    // sanity checks
                    if (chunkType == 0x2f) {
                        if (legalChunkPos(chunkX, chunkZ)) {
                            dimDataList[0]->addToChunkBounds(chunkX, chunkZ);
                        }
                    }
                } else if (key_size == 13) {
                    // pre-0.17 chunk block data
                    chunkX = myParseInt32(key, 0);
                    chunkZ = myParseInt32(key, 4);
                    chunkDimId = myParseInt32(key, 8);
                    chunkType = myParseInt8(key, 12);

                    // sanity checks
                    if (chunkType == 0x30) {
                        if (legalChunkPos(chunkX, chunkZ)) {
                            dimDataList[chunkDimId]->addToChunkBounds(chunkX, chunkZ);
                        }
                    }
                } else if (key_size == 14) {
                    // post-0.17 chunk block data
                    chunkX = myParseInt32(key, 0);
                    chunkZ = myParseInt32(key, 4);
                    chunkDimId = myParseInt32(key, 8);
                    chunkType = myParseInt8(key, 12);

                    // sanity checks
                    if (chunkType == 0x2f) {
                        if (legalChunkPos(chunkX, chunkZ)) {
                            dimDataList[chunkDimId]->addToChunkBounds(chunkX, chunkZ);
                        }
                    }
                }
            }

            if (!iter->status().ok()) {
                slogger.msg(kLogInfo1, "WARNING: LevelDB operation returned status=%s\n",
                            iter->status().ToString().c_str());
            }
            delete iter;

            // mark bounds valid
            for (int32_t i = 0; i < kDimIdCount; i++) {
                dimDataList[i]->setChunkBoundsValid();
                dimDataList[i]->reportChunkBounds();
            }

            slogger.msg(kLogInfo1, "  %d records\n", recordCt);
            totalRecordCt = recordCt;

            return 0;
        }

        // this is where we go through every item in the leveldb, we parse interesting things as we go
        int32_t dbParse() {

            char tmpstring[256];

            int32_t chunkX = -1, chunkZ = -1, chunkDimId = -1, chunkType = -1, chunkTypeSub = -1;
            int32_t chunkFormatVersion = 2; //todonow - get properly


            // we make sure that we know the chunk bounds before we start so that we can translate world coords to image coords
            calcChunkBounds();

            // report hide and force lists
            {
                slogger.msg(kLogInfo1, "Active 'hide-top', 'force-top', and 'geojson-block':\n");
                int32_t itemCt = 0;
                int32_t blockId;
                for (int32_t dimId = 0; dimId < kDimIdCount; dimId++) {
                    dimDataList[dimId]->updateFastLists();
                    for (const auto &iter : dimDataList[dimId]->blockHideList) {
                        blockId = iter;
                        slogger.msg(kLogInfo1, "  'hide-top' block: %s - %s (dimId=%d blockId=%d (0x%02x))\n",
                                    dimDataList[dimId]->getName().c_str(), blockInfoList[blockId].name.c_str(), dimId,
                                    blockId, blockId);
                        itemCt++;
                    }

                    for (const auto &iter : dimDataList[dimId]->blockForceTopList) {
                        blockId = iter;
                        slogger.msg(kLogInfo1, "  'force-top' block: %s - %s (dimId=%d blockId=%d (0x%02x))\n",
                                    dimDataList[dimId]->getName().c_str(), blockInfoList[blockId].name.c_str(), dimId,
                                    blockId, blockId);
                        itemCt++;
                    }

                    for (const auto &iter : dimDataList[dimId]->blockToGeoJSONList) {
                        blockId = iter;
                        slogger.msg(kLogInfo1, "  'geojson' block: %s - %s (dimId=%d blockId=%d (0x%02x))\n",
                                    dimDataList[dimId]->getName().c_str(), blockInfoList[blockId].name.c_str(), dimId,
                                    blockId, blockId);
                        itemCt++;
                    }
                }
                if (itemCt == 0) {
                    slogger.msg(kLogInfo1, "None\n");
                }
            }

            slogger.msg(kLogInfo1, "Parse all leveldb records\n");

            MyNbtTagList tagList;
            int32_t recordCt = 0, ret;

            leveldb::Slice skey, svalue;
            size_t key_size;
            size_t cdata_size;
            const char *key;
            const char *cdata;
            std::string dimName, chunkstr;

            leveldb::Iterator *iter = db->NewIterator(levelDbReadOptions);
            for (iter->SeekToFirst(); iter->Valid(); iter->Next()) {

                // note: we get the raw buffer early to avoid overhead (maybe?)
                skey = iter->key();
                key_size = (int) skey.size();
                key = skey.data();

                svalue = iter->value();
                cdata_size = svalue.size();
                cdata = svalue.data();

                ++recordCt;
                if (control.shortRunFlag && recordCt > 1000) {
                    break;
                }
                if ((recordCt % 10000) == 0) {
                    double pct = (double) recordCt / (double) totalRecordCt;
                    slogger.msg(kLogInfo1, "  Processing records: %d / %d (%.1lf%%)\n", recordCt, totalRecordCt,
                                (pct * 100.0));
                }

                logger.msg(kLogInfo1, "\n");

                // we look at the key to determine what we have, some records have text keys

                if (strncmp(key, "BiomeData", key_size) == 0) {
                    // 0x61 +"BiomeData" -- snow accum? -- overworld only?
                    logger.msg(kLogInfo1, "BiomeData value:\n");
                    parseNbt("BiomeData: ", cdata, cdata_size, tagList);
                    // todo - parse tagList? snow accumulation amounts
                } else if (strncmp(key, "Overworld", key_size) == 0) {
                    logger.msg(kLogInfo1, "Overworld value:\n");
                    parseNbt("Overworld: ", cdata, cdata_size, tagList);
                    // todo - parse tagList? a list of "LimboEntities"
                } else if (strncmp(key, "~local_player", key_size) == 0) {
                    logger.msg(kLogInfo1, "Local Player value:\n");
                    ret = parseNbt("Local Player: ", cdata, cdata_size, tagList);
                    if (ret == 0) {
                        parseNbt_entity(-1, "", tagList, true, false, "Local Player", "");
                    }
                } else if ((key_size >= 7) && (strncmp(key, "player_", 7) == 0)) {
                    // note: key contains player id (e.g. "player_-1234")
                    std::string playerRemoteId = std::string(&key[strlen("player_")], key_size - strlen("player_"));

                    logger.msg(kLogInfo1, "Remote Player (id=%s) value:\n", playerRemoteId.c_str());

                    ret = parseNbt("Remote Player: ", cdata, cdata_size, tagList);
                    if (ret == 0) {
                        parseNbt_entity(-1, "", tagList, false, true, "Remote Player", playerRemoteId);
                    }
                } else if (strncmp(key, "villages", key_size) == 0) {
                    logger.msg(kLogInfo1, "Villages value:\n");
                    parseNbt("villages: ", cdata, cdata_size, tagList);
                    // todo - parse tagList? usually empty, unless player is in range of village; test that!
                } else if (strncmp(key, "mVillages", key_size) == 0) {
                    // todobig -- new for 0.13? what is it?
                    logger.msg(kLogInfo1, "mVillages value:\n");
                    ret = parseNbt("mVillages: ", cdata, cdata_size, tagList);
                    if (ret == 0) {
                        parseNbt_mVillages(tagList);
                    }
                } else if (strncmp(key, "game_flatworldlayers", key_size) == 0) {
                    // todobig -- what is it?
                    // example data (standard flat): 5b 37 2c 33 2c 33 2c 32 5d
                    logger.msg(kLogInfo1, "game_flatworldlayers value: (todo)\n");
                    // parseNbt("game_flatworldlayers: ", cdata, cdata_size, tagList);
                    // todo - parse tagList?
                } else if (strncmp(key, "idcounts", key_size) == 0) {
                    // todobig -- new for 0.13? what is it? is it a 4-byte int?
                    logger.msg(kLogInfo1, "idcounts value:\n");
                    parseNbt("idcounts: ", cdata, cdata_size, tagList);
                } else if (strncmp(key, "Nether", key_size) == 0) {
                    logger.msg(kLogInfo1, "Nether value:\n");
                    parseNbt("Nether: ", cdata, cdata_size, tagList);
                    // todo - parse tagList?  list of LimboEntities
                } else if (strncmp(key, "portals", key_size) == 0) {
                    logger.msg(kLogInfo1, "portals value:\n");
                    ret = parseNbt("portals: ", cdata, cdata_size, tagList);
                    if (ret == 0) {
                        parseNbt_portals(tagList);
                    }
                } else if (strncmp(key, "AutonomousEntities", key_size) == 0) {
                    logger.msg(kLogInfo1, "AutonomousEntities value:\n");
                    ret = parseNbt("AutonomousEntities: ", cdata, cdata_size, tagList);
                    // todostopper - what to do with this info?
                    //          if ( ret == 0 ) {
                    //            parseNbt_portals(tagList);
                    //          }
                }

                    // todohere todonow -- new record like "dimension0" - presumably for other dims too
                    //           looks like it could be partially text? nbt?
                    /*
          WARNING: Unparsed Record:
          key_size=10
          key_string=[dimension0^AC<93><9A>]
          key_hex=[64 69 6d 65 6e 73 69 6f 6e 30]
          value_size=65
          value_hex=[0a 00 00 0a 09 00 6d 69 6e 65 73 68 61 66 74 00 0a 06 00 6f 63 65 61 6e 73 00 0a 09 00 73 63 61 74 74 65 72 65 64 00 0a 0a 00 73 74 72 6f 6e 67 68 6f 6c 64 00 0a 07 00 76 69 6c 6c 61 67 65 00 00]


          UNK: NBT Decode Start
          UNK: [] COMPOUND-1 {
          UNK:   [mineshaft] COMPOUND-2 {
          UNK:   } COMPOUND-2
          UNK:   [oceans] COMPOUND-3 {
          UNK:   } COMPOUND-3
          UNK:   [scattered] COMPOUND-4 {
          UNK:   } COMPOUND-4
          UNK:   [stronghold] COMPOUND-5 {
          UNK:   } COMPOUND-5
          UNK:   [village] COMPOUND-6 {
          UNK:   } COMPOUND-6
          UNK: } COMPOUND-1
          UNK: NBT Decode End (1 tags)

        */
                else if (strncmp(key, "dimension", 9) == 0) {
                    std::string keyString(key, key_size);
                    logger.msg(kLogInfo1, "Dimension chunk -- key: (%s) value:\n", keyString.c_str());
                    ret = parseNbt("Dimension: ", cdata, cdata_size, tagList);
                    // todostopper - what to do with this info?
                    //          if ( ret == 0 ) {
                    //            parseNbt_portals(tagList);
                    //          }
                } else if (key_size == 9 || key_size == 10 || key_size == 13 || key_size == 14) {

                    // these are probably chunk records, we parse the key and determine what we've got

                    chunkTypeSub = 0;

                    if (key_size == 9) {
                        // overworld chunk
                        chunkX = myParseInt32(key, 0);
                        chunkZ = myParseInt32(key, 4);
                        chunkDimId = kDimIdOverworld;
                        chunkType = myParseInt8(key, 8);
                        dimName = "overworld";
                        chunkFormatVersion = 2; //todonow - get properly
                    } else if (key_size == 10) {
                        // overworld chunk
                        chunkX = myParseInt32(key, 0);
                        chunkZ = myParseInt32(key, 4);
                        chunkDimId = kDimIdOverworld;
                        chunkType = myParseInt8(key, 8);
                        chunkTypeSub = myParseInt8(key, 9); // todonow - rename
                        dimName = "overworld";
                        chunkFormatVersion = 3; //todonow - get properly
                    } else if (key_size == 13) {
                        // non-overworld chunk
                        chunkX = myParseInt32(key, 0);
                        chunkZ = myParseInt32(key, 4);
                        chunkDimId = myParseInt32(key, 8);
                        chunkType = myParseInt8(key, 12);
                        dimName = "nether";
                        chunkFormatVersion = 2; //todonow - get properly

                        // adjust weird dim id's
                        if (chunkDimId == 0x32373639) {
                            chunkDimId = kDimIdTheEnd;
                        }
                        if (chunkDimId == 0x33373639) {
                            chunkDimId = kDimIdNether;
                        }

                        // check for new dim id's
                        if (chunkDimId != kDimIdNether && chunkDimId != kDimIdTheEnd) {
                            slogger.msg(kLogInfo1,
                                        "WARNING: UNKNOWN -- Found new chunkDimId=0x%x -- we are not prepared for that -- skipping chunk\n",
                                        chunkDimId);
                            continue;
                        }
                    } else if (key_size == 14) {
                        // non-overworld chunk
                        chunkX = myParseInt32(key, 0);
                        chunkZ = myParseInt32(key, 4);
                        chunkDimId = myParseInt32(key, 8);
                        chunkType = myParseInt8(key, 12);
                        chunkTypeSub = myParseInt8(key, 13); // todonow - rename
                        dimName = "nether";
                        chunkFormatVersion = 3; //todonow - get properly

                        // adjust weird dim id's
                        if (chunkDimId == 0x32373639) {
                            chunkDimId = kDimIdTheEnd;
                        }
                        if (chunkDimId == 0x33373639) {
                            chunkDimId = kDimIdNether;
                        }

                        // check for new dim id's
                        if (chunkDimId != kDimIdNether && chunkDimId != kDimIdTheEnd) {
                            slogger.msg(kLogInfo1,
                                        "WARNING: UNKNOWN -- Found new chunkDimId=0x%x -- we are not prepared for that -- skipping chunk\n",
                                        chunkDimId);
                            continue;
                        }
                    }

                    // we check for corrupt chunks
                    if (!legalChunkPos(chunkX, chunkZ)) {
                        slogger.msg(kLogInfo1, "WARNING: Found a chunk with invalid chunk coordinates cx=%d cz=%d\n",
                                    chunkX, chunkZ);
                        continue;
                    }

                    dimDataList[chunkDimId]->addHistogramChunkType(chunkType);

                    // report info about the chunk
                    chunkstr = dimName + "-chunk: ";
                    sprintf(tmpstring, "%d %d (type=0x%02x) (subtype=0x%02x) (size=%d)", chunkX, chunkZ, chunkType,
                            chunkTypeSub, (int32_t) cdata_size);
                    chunkstr += tmpstring;
                    if (true) {
                        // show approximate image coordinates for chunk
                        double tix, tiy;
                        dimDataList[chunkDimId]->worldPointToImagePoint(chunkX * 16, chunkZ * 16, tix, tiy, false);
                        int32_t imageX = tix;
                        int32_t imageZ = tiy;
                        sprintf(tmpstring, " (image %d %d)", (int32_t) imageX, (int32_t) imageZ);
                        chunkstr += tmpstring;
                    }
                    logger.msg(kLogInfo1, "%s\n", chunkstr.c_str());

                    // see what kind of chunk we have
                    // tommo posted useful info about the various record types here (around 0.17 beta):
                    //   https://www.reddit.com/r/MCPE/comments/5cw2tm/level_format_changes_in_mcpe_0171_100/
                    switch (chunkType) {
                        case 0x30:
                            // "LegacyTerrain"
                            // chunk block data
                            // we do the parsing in the destination object to save memcpy's
                            // todonow - would be better to get the version # from the proper chunk record (0x76)
                            dimDataList[chunkDimId]->addChunk(2, chunkX, 0, chunkZ, cdata, cdata_size);
                            break;

                        case 0x31:
                            // "BlockEntity"
                            // tile entity record (e.g. a chest)
                            logger.msg(kLogInfo1, "%s 0x31 chunk (tile entity data):\n", dimName.c_str());
                            ret = parseNbt("0x31-te: ", cdata, cdata_size, tagList);
                            if (ret == 0) {
                                parseNbt_tileEntity(chunkDimId, dimName + "-", tagList);
                            }
                            break;

                        case 0x32:
                            // "Entity"
                            // entity record (e.g. a mob)
                            logger.msg(kLogInfo1, "%s 0x32 chunk (entity data):\n", dimName.c_str());
                            ret = parseNbt("0x32-e: ", cdata, cdata_size, tagList);
                            if (ret == 0) {
                                parseNbt_entity(chunkDimId, dimName + "-", tagList, false, false, "", "");
                            }
                            break;

                        case 0x33:
                            // "PendingTicks"
                            // todo - this appears to be info on blocks that can move: water + lava + fire + sand + gravel
                            logger.msg(kLogInfo1, "%s 0x33 chunk (tick-list):\n", dimName.c_str());
                            parseNbt("0x33-tick: ", cdata, cdata_size, tagList);
                            // todo - parse tagList?
                            // todobig - could show location of active fires
                            break;

                        case 0x34:
                            // "BlockExtraData"
                            logger.msg(kLogInfo1, "%s 0x34 chunk (TODO - MYSTERY RECORD - BlockExtraData)\n",
                                       dimName.c_str());
                            if (control.verboseFlag) {
                                printKeyValue(key, key_size, cdata, cdata_size, false);
                            }
                            // according to tommo (https://www.reddit.com/r/MCPE/comments/5cw2tm/level_format_changes_in_mcpe_0171_100/)
                            // "BlockExtraData"
                            /*
               0x34 ?? does not appear to be NBT data -- overworld only? -- perhaps: b0..3 (count); for each: (int32_t) (int16_t)
               -- there are 206 of these in "another1" world
               -- something to do with snow?
               -- to examine data:
               cat (logfile) | grep "WARNING: Unknown key size" | grep " 34\]" | cut -b75- | sort | nl
            */
                            break;

                        case 0x35:
                            // "BiomeState"
                            logger.msg(kLogInfo1, "%s 0x35 chunk (TODO - MYSTERY RECORD - BiomeState)\n",
                                       dimName.c_str());
                            if (control.verboseFlag) {
                                printKeyValue(key, key_size, cdata, cdata_size, false);
                            }
                            // according to tommo (https://www.reddit.com/r/MCPE/comments/5cw2tm/level_format_changes_in_mcpe_0171_100/)
                            // "BiomeState"
                            /*
              0x35 ?? -- both dimensions -- length 3,5,7,9,11 -- appears to be: b0 (count of items) b1..bn (2-byte ints)
              -- there are 2907 in "another1"
              -- to examine data:
              cat (logfile) | grep "WARNING: Unknown key size" | grep " 35\]" | cut -b75- | sort | nl
            */
                            break;

                        case 0x36:
                            // new for v1.2?
                            logger.msg(kLogInfo1, "%s 0x36 chunk (TODO - MYSTERY RECORD - TBD)\n", dimName.c_str());
                            if (control.verboseFlag) {
                                printKeyValue(key, key_size, cdata, cdata_size, false);
                            }
                            // todo - what is this?
                            // appears to be a single 4-byte integer?
                            break;

                        case 0x39:
                            // new for v1.2?
                            logger.msg(kLogInfo1, "%s 0x39 chunk (TODO - MYSTERY RECORD - TBD)\n", dimName.c_str());
                            if (control.verboseFlag) {
                                printKeyValue(key, key_size, cdata, cdata_size, false);
                            }
                            // todo - what is this?
                            break;

                        case 0x76:
                            // "Version"
                            // todo - this is chunk version information?
                        {
                            // this record is not very interesting, we usually hide it
                            // note: it would be interesting if this is not == 2 (as of MCPE 0.12.x it is always 2)
                            if (control.verboseFlag || ((cdata[0] != 2) && (cdata[0] != 3) && (cdata[0] != 9))) {
                                if (cdata[0] != 2 && cdata[0] != 9) {
                                    logger.msg(kLogInfo1,
                                               "WARNING: UNKNOWN CHUNK VERSION!  %s 0x76 chunk (world format version): v=%d\n",
                                               dimName.c_str(), (int) (cdata[0]));
                                } else {
                                    logger.msg(kLogInfo1, "%s 0x76 chunk (world format version): v=%d\n",
                                               dimName.c_str(), (int) (cdata[0]));
                                }
                            }
                        }
                            break;

                        case 0x2f:
                            // "SubchunkPrefix"
                            // chunk block data - 10241 bytes
                            // todonow -- but have also seen 6145 on v1.1?
                            // we do the parsing in the destination object to save memcpy's
                            // todonow - would be better to get the version # from the proper chunk record (0x76)
                        {
                            int32_t chunkY = chunkTypeSub;
                            // check the first byte to see if anything interesting is in it
                            if (cdata[0] != 0) {
                                //logger.msg(kLogInfo1, "WARNING: UNKNOWN Byte 0 of 0x2f chunk: b0=[%d 0x%02x]\n", (int)cdata[0], (int)cdata[0]);
                                dimDataList[chunkDimId]->addChunk(7, chunkX, chunkY, chunkZ, cdata, cdata_size);
                            } else {
                                if (cdata_size != 6145 && cdata_size != 10241) {
                                    logger.msg(kLogInfo1, "WARNING: UNKNOWN cdata_size=%d of 0x2f chunk\n",
                                               (int) cdata_size);
                                }
                                dimDataList[chunkDimId]->addChunk(chunkFormatVersion, chunkX, chunkY, chunkZ, cdata,
                                                                  cdata_size);
                            }
                        }
                            break;

                        case 0x2d:
                            // "Data2D"
                            // chunk column data - 768 bytes
                            // format appears to be:
                            // 16x16 of 2-byte ints for HEIGHT OF TOP BLOCK
                            // 8x8 of 4-byte ints for BIOME and GRASS COLOR
                            // todonow todobig todohere -- this appears to be an MCPE bug, it should be 16x16, right?
                            // also - grass colors are pretty weird (some are 01 01 01)

                            // todonow - would be better to get the version # from the proper chunk record (0x76)
                        {
                            dimDataList[chunkDimId]->addChunkColumnData(3, chunkX, chunkZ, cdata, cdata_size);
                        }
                            break;


                            /*
               todohere todonow
               new chunk types in 0.17
               0x2d] - size=768
               0x2f 0x00] - size 10241
               ...
               0x2f 0x07] - size 10241


               per chunk data: 2.5 bytes / block
               block id = 1 byte
               block data = 4-bits
               skylight = 4-bits
               blocklight = 4-bits

               16x16x16 of this = 10,240!!
               what is the one extra byte... hmmmm

               NOTE! as of at least v1.1.0 there are also records that are 6145 bytes - they appear
               to exclude the block/sky light parts


               per column data: 5-bytes per column
               height of top block = 1 byte
               grass-and-biome = 4-bytes = lsb bome, high 3-bytes are RGB grass color

               0x2d chunks are 768 bytes which could be column data
               16 x 16 x 3 = 768
               so 3 bytes per column = grass/biome + height + top block
               could this be grass color only?


               0x2f N] chunks are 10241
               this could be 16x16 for 16 vertical blocks
               16 of these would cover 256 build height

               if blocks are 8-bits, that would be 8,192 of the size
               which leaves 2049
               we'd still need block data which is 4-bits per block
               which is: 4,096 bytes.... what's going on here?!
            */

                        default:
                            logger.msg(kLogInfo1, "WARNING: %s unknown chunk - key_size=%d type=0x%x length=%d\n",
                                       dimName.c_str(),
                                       (int32_t) key_size, chunkType, (int32_t) cdata_size);
                            printKeyValue(key, key_size, cdata, cdata_size, true);

                            if (false) {
                                if (cdata_size > 10) {
                                    parseNbt("UNK: ", cdata, cdata_size, tagList);
                                }
                            }
                            break;
                    }
                } else {
                    logger.msg(kLogInfo1, "WARNING: Unknown chunk - key_size=%d cdata_size=%d\n", (int32_t) key_size,
                               (int32_t) cdata_size);
                    printKeyValue(key, key_size, cdata, cdata_size, true);
                    if (false) {
                        // try to nbt decode
                        logger.msg(kLogInfo1, "WARNING: Attempting NBT Decode:\n");
                        parseNbt("WARNING: ", cdata, cdata_size, tagList);
                    }
                }
            }
            slogger.msg(kLogInfo1, "Read %d records\n", recordCt);
            slogger.msg(kLogInfo1, "Status: %s\n", iter->status().ToString().c_str());

            if (!iter->status().ok()) {
                slogger.msg(kLogInfo1, "WARNING: LevelDB operation returned status=%s\n",
                            iter->status().ToString().c_str());
            }
            delete iter;

            return 0;
        }

        int32_t checkSpawnable() {
            // for cubic chunks we need to first know that a chunk needs to be checked for spawnable,
            // then we collect all of that chunk's data and do the spawn checking

            for (int did = 0; did < kDimIdCount; did++) {
                slogger.msg(kLogInfo1, "Check Spawnable: Dimension '%s' (%d)\n", dimDataList[did]->getName().c_str(),
                            did);
                dimDataList[did]->checkSpawnable(db);
            }

            return 0;
        }

        int32_t doOutput_Tile_image(const std::string &fn) {
            if (fn.size() <= 0) {
                return -1;
            }

            std::string dirOut = mydirname(control.fnOutputBase) + "/tiles";
            local_mkdir(dirOut.c_str());

            slogger.msg(kLogInfo1, "Creating tiles for %s...\n", mybasename(fn).c_str());
            PngTiler pngTiler(fn, control.tileWidth, control.tileHeight, dirOut);
            if (pngTiler.doTile() == 0) {
                // all is good
            } else {
                // todobig - error
            }

            return 0;
        }

        int32_t doOutput_Tile() {
            if (!control.doTiles) {
                return 0;
            }

            for (int32_t dimid = 0; dimid < kDimIdCount; dimid++) {
                doOutput_Tile_image(control.fnLayerTop[dimid]);
                doOutput_Tile_image(control.fnLayerBiome[dimid]);
                doOutput_Tile_image(control.fnLayerHeight[dimid]);
                doOutput_Tile_image(control.fnLayerHeightGrayscale[dimid]);
                doOutput_Tile_image(control.fnLayerHeightAlpha[dimid]);
                doOutput_Tile_image(control.fnLayerBlockLight[dimid]);
                doOutput_Tile_image(control.fnLayerSkyLight[dimid]);
                doOutput_Tile_image(control.fnLayerSlimeChunks[dimid]);
                doOutput_Tile_image(control.fnLayerGrass[dimid]);
                doOutput_Tile_image(control.fnLayerShadedRelief[dimid]);
                for (int32_t cy = 0; cy <= MAX_BLOCK_HEIGHT; cy++) {
                    doOutput_Tile_image(control.fnLayerRaw[dimid][cy]);
                }
            }

            return 0;
        }


        std::string makeTileURL(const std::string &fn) {
            std::string ret = mybasename(fn);
            if (!control.doTiles) {
                return "images/" + ret;
            }
            if (ret.size() > 1) {
                return "tiles/" + ret + ".{y}.{x}.png";
            }
            return "";
        }


        int32_t doOutput_html() {
            char tmpstring[1025];

            slogger.msg(kLogInfo1, "Do Output: html viewer\n");

            sprintf(tmpstring, "%s/mcpe_viz.html.template", dirExec.c_str());
            std::string fnHtmlSrc = tmpstring;

            sprintf(tmpstring, "%s/mcpe_viz.js", dirExec.c_str());
            std::string fnJsSrc = tmpstring;

            sprintf(tmpstring, "%s/mcpe_viz.css", dirExec.c_str());
            std::string fnCssSrc = tmpstring;

            // create html file -- need to substitute one variable (extra js file)
            StringReplacementList replaceStrings;

            if (control.noForceGeoJSONFlag) {
                // we do not include the geojson file
                replaceStrings.push_back(std::make_pair(std::string("%JSFILE%"),
                                                        "<script src=\"" +
                                                        std::string(mybasename(control.fnJs.c_str())) +
                                                        "\"></script>"
                                         )
                );
            } else {
                // we do include the geojson file
                replaceStrings.push_back(std::make_pair(std::string("%JSFILE%"),
                                                        "<script src=\"" +
                                                        std::string(mybasename(control.fnJs.c_str())) +
                                                        "\"></script>\n" +
                                                        "<script src=\"" +
                                                        std::string(mybasename(control.fnGeoJSON.c_str())) +
                                                        "\"></script>"
                                         )
                );
            }
            copyFileWithStringReplacement(fnHtmlSrc, control.fnHtml, replaceStrings);

            // create javascript file w/ filenames etc
            FILE *fp = fopen(control.fnJs.c_str(), "w");
            if (fp) {
                time_t xtime = time(NULL);
                char timebuf[256];
#if _MSC_VER
                ctime_s(timebuf, 256, &xtime);
#else 
                ctime_r(&xtime, timebuf);
#endif
                // todo - this is hideous.
                // fix time string
                char *p = strchr(timebuf, '\n');
                if (p) { *p = 0; }

                fprintf(fp,
                        "// mcpe_viz javascript helper file -- created by mcpe_viz program\n"
                        "var worldName = '%s';\n"
                        "var worldSeed = %lld;\n"
                        "var creationTime = '%s';\n"
                        "var creationMcpeVizVersion = '%s';\n"
                        "var loadGeoJSONFlag = %s;\n"
                        "var fnGeoJSON = '%s';\n"
                        "var useTilesFlag = %s;\n"
                        "var tileW = %d;\n"
                        "var tileH = %d;\n"
                        "var dimensionInfo = {\n", escapeString(getWorldName().c_str(), "'").c_str(),
                        (long long int) getWorldSeed(), escapeString(timebuf, "'").c_str(),
                        bedrock_viz::version, control.noForceGeoJSONFlag ? "true" : "false",
                        mybasename(control.fnGeoJSON).c_str(), control.doTiles ? "true" : "false", control.tileWidth,
                        control.tileHeight
                );
                for (int32_t did = 0; did < kDimIdCount; did++) {
                    fprintf(fp, "'%d': {\n", did);
                    fprintf(fp, "  minWorldX: %d,\n", dimDataList[did]->getMinChunkX() * 16);
                    fprintf(fp, "  maxWorldX: %d + 15,\n", dimDataList[did]->getMaxChunkX() * 16);
                    fprintf(fp, "  minWorldY: %d,\n", dimDataList[did]->getMinChunkZ() * 16);
                    fprintf(fp, "  maxWorldY: %d + 15,\n", dimDataList[did]->getMaxChunkZ() * 16);

                    double px = playerPositionImageX;
                    double py = playerPositionImageY;

                    // auto-adjust player position based on where they actually are
                    if (did == kDimIdNether) {
                        if (playerPositionDimensionId != kDimIdNether) {
                            px /= 8;
                            py /= 8;
                        }
                    } else {
                        if (playerPositionDimensionId == kDimIdNether) {
                            px *= 8;
                            py *= 8;
                        }
                    }
                    fprintf(fp, "  playerPosX: %lf,\n", px);
                    fprintf(fp, "  playerPosY: %lf,\n", py);

                    // list of blocks that were added to geojson
                    fprintf(fp, "  geojsonBlocks: [ ");
                    int32_t llen = dimDataList[did]->blockToGeoJSONList.size();
                    for (const auto &it : dimDataList[did]->blockToGeoJSONList) {
                        fprintf(fp, "'%s'", blockInfoList[it].name.c_str());
                        if (--llen > 0) {
                            fprintf(fp, ", ");
                        }
                    }
                    fprintf(fp, " ],\n");

                    fprintf(fp, "  spawnableFlag: %s,\n",
                            (dimDataList[did]->listCheckSpawn.size() > 0) ? "true" : "false");

                    fprintf(fp, "  fnLayerTop: '%s',\n", makeTileURL(control.fnLayerTop[did]).c_str());
                    fprintf(fp, "  fnLayerBiome: '%s',\n", makeTileURL(control.fnLayerBiome[did]).c_str());
                    fprintf(fp, "  fnLayerHeight: '%s',\n", makeTileURL(control.fnLayerHeight[did]).c_str());
                    fprintf(fp, "  fnLayerHeightGrayscale: '%s',\n",
                            makeTileURL(control.fnLayerHeightGrayscale[did]).c_str());
                    fprintf(fp, "  fnLayerHeightAlpha: '%s',\n", makeTileURL(control.fnLayerHeightAlpha[did]).c_str());
                    fprintf(fp, "  fnLayerShadedRelief: '%s',\n",
                            makeTileURL(control.fnLayerShadedRelief[did]).c_str());
                    fprintf(fp, "  fnLayerBlockLight: '%s',\n", makeTileURL(control.fnLayerBlockLight[did]).c_str());
                    fprintf(fp, "  fnLayerSlimeChunks: '%s',\n", makeTileURL(control.fnLayerSlimeChunks[did]).c_str());
                    fprintf(fp, "  fnLayerGrass: '%s',\n", makeTileURL(control.fnLayerGrass[did]).c_str());

                    fprintf(fp, "  listLayers: [\n");
                    for (int32_t i = 0; i <= MAX_BLOCK_HEIGHT; i++) {
                        fprintf(fp, "    '%s',\n", makeTileURL(control.fnLayerRaw[did][i]).c_str());
                    }
                    fprintf(fp, "  ]\n");
                    if ((did + 1) < kDimIdCount) {
                        fprintf(fp, "},\n");
                    } else {
                        fprintf(fp, "}\n");
                    }
                }
                fprintf(fp, "};\n");

                // write block color info
                fprintf(fp,
                        "// a lookup table for identifying blocks in the web app\n"
                        "// key is color (decimal), value is block name\n"
                        "// hacky? it sure is!\n"
                );

                fprintf(fp, "var blockColorLUT = {\n");
                for (int32_t i = 0; i < 512; i++) {
                    if (blockInfoList[i].hasVariants()) {
                        // we need to get blockdata
                        for (const auto &itbv : blockInfoList[i].variantList) {
                            fprintf(fp, "'%d': { name: '%s', id: %d, blockdata: %d },\n", be32toh(itbv->color),
                                    escapeString(itbv->name, "'").c_str(), i, itbv->blockdata
                            );
                        }
                    } else {
                        if (blockInfoList[i].colorSetFlag) {
                            fprintf(fp, "'%d': { name: '%s', id: %d, blockdata: %d },\n",
                                    be32toh(blockInfoList[i].color), escapeString(blockInfoList[i].name, "'").c_str(),
                                    i, 0
                            );
                        }
                    }
                }
                // last, put the catch-all
                fprintf(fp, "'%d': { name: '*UNKNOWN BLOCK*', id: -1, blockdata: 0 }\n};\n", kColorDefault);

                // write biome color info
                fprintf(fp,
                        "// a lookup table for identifying biomes in the web app\n"
                        "// key is color (decimal), value is biome name\n"
                        "// hacky? it sure is!\n"
                );

                fprintf(fp, "var biomeColorLUT = {\n");
                for (const auto &it : biomeInfoList) {
                    if (it.second->colorSetFlag) {
                        fprintf(fp, "'%d': { name: '%s', id: %d },\n", be32toh(it.second->color),
                                escapeString(it.second->name, "'").c_str(), it.first
                        );
                    }
                }
                // last, put the catch-all
                fprintf(fp, "'%d': { name: '*UNKNOWN BIOME*', id: -1 }\n};\n", kColorDefault);


                fprintf(fp,
                        "// lut for inventory items\n"
                );

                fprintf(fp, "var imageIconLUT = {\n");
                for (const auto &it : imageFileMap) {
                    fprintf(fp, "'%d': '%s',\n", it.second, it.first.c_str());
                }
                fprintf(fp, "'-1': ''};\n");

                fclose(fp);

            } else {
                slogger.msg(kLogInfo1, "ERROR: Failed to open javascript output file (fn=%s error=%s (%d))\n",
                            control.fnJs.c_str(), strerror(errno), errno);
            }

            // copy helper files to destination directory
            std::string dirDest = mydirname(control.fnOutputBase);

            if (dirDest.size() > 0 && dirDest != ".") {
                // todo - how to be sure that this is a diff dir?
                sprintf(tmpstring, "%s/%s", dirDest.c_str(), mybasename(fnJsSrc).c_str());
                std::string fnJsDest = tmpstring;
                copyFile(fnJsSrc, fnJsDest, false);

                sprintf(tmpstring, "%s/%s", dirDest.c_str(), mybasename(fnCssSrc).c_str());
                std::string fnCssDest = tmpstring;
                copyFile(fnCssSrc, fnCssDest, false);

                // copy javascript files
                std::string dirJs = dirDest + "/js";
                local_mkdir(dirJs);
                copyDirToDir(dirExec + "/js", dirJs, false);

                // copy images
                // todobig - could save a good amount of time per run if we detect if dir has already been copied
                // todobig - could save a bit of time by only copying images used (use imageFileMap)
                slogger.msg(kLogInfo1, "Copying icon images\n");
                std::string dirImages = dirDest + "/images";
                local_mkdir(dirImages);
                copyDirToDir(dirExec + "/images", dirImages, true);

            } else {
                // if same dir, don't copy files
            }

            return 0;
        }


        int32_t doOutput_colortest() {
            slogger.msg(kLogInfo1, "Do Output: html colortest\n");

            std::string fnOut = control.fnOutputBase + ".colortest.html";
            FILE *fp = fopen(fnOut.c_str(), "w");

            if (!fp) {
                slogger.msg(kLogInfo1, "ERROR: failed to open output file (%s error=%s (%d))\n", fnOut.c_str(),
                            strerror(errno), errno);
                return -1;
            }

            // put start of html file
            fprintf(fp,
                    "<!doctype html>\n"
                    "<html><head><title>MCPE Viz Color Test</title>\n"
                    "<style>"
                    ".section { width: 100%%; padding: 2em 2em; }"
                    ".colorBlock { width: 100%%; padding: 0.5em 2em; }"
                    ".darkBlock  { color: #ffffff; }"
                    "</style>"
                    "</head>"
                    "<body>"
            );

            // create list of all colors and sort them by HSL
            std::vector<std::unique_ptr<ColorInfo> > webColorList;

            webColorList.clear();
            for (int32_t i = 0; i < 512; i++) {
                if (blockInfoList[i].hasVariants()) {
                    for (const auto &itbv : blockInfoList[i].variantList) {
                        webColorList.push_back(std::unique_ptr<ColorInfo>
                                                       (new ColorInfo(itbv->name, be32toh(itbv->color))));
                    }
                } else {
                    if (blockInfoList[i].colorSetFlag) {
                        webColorList.push_back(std::unique_ptr<ColorInfo>
                                                       (new ColorInfo(blockInfoList[i].name,
                                                                      be32toh(blockInfoList[i].color))));
                    }
                }
            }

            std::sort(webColorList.begin(), webColorList.end(), compareColorInfo);
            fprintf(fp, "<div class=\"section\">Block Colors</div>");
            for (const auto &it : webColorList) {
                fprintf(fp, "%s\n", it->toHtml().c_str());
            }


            webColorList.clear();
            for (const auto &it : biomeInfoList) {
                if (it.second->colorSetFlag) {
                    // webColorList.emplace_back(blockInfoList[i].name, (int32_t)be32toh(blockInfoList[i].color));
                    webColorList.push_back(
                            std::unique_ptr<ColorInfo>(new ColorInfo(it.second->name, be32toh(it.second->color))));
                }
            }

            std::sort(webColorList.begin(), webColorList.end(), compareColorInfo);
            fprintf(fp, "<div class=\"section\">Biome Colors</div>");
            for (const auto &it : webColorList) {
                fprintf(fp, "%s\n", it->toHtml().c_str());
            }

            fprintf(fp, "\n</body></html>\n");
            fclose(fp);
            return 0;
        }


        int32_t doOutput_GeoJSON() {

            if (false) {
#if 0
                                                                                                                                        // todobig - this would be lovely but does not work when run on windows (browser does not like the gzip'ed geojson file)

        // we output gzip'ed data (saves a ton of disk+bandwidth for very little cost)

        gzFile_s* fpGeoJSON = gzopen(control.fnGeoJSON.c_str(), "w");
        if ( ! fpGeoJSON ) {
          slogger.msg(kLogInfo1,"ERROR: Failed to create GeoJSON output file (%s).\n", control.fnGeoJSON.c_str());
          return -1;
        }

        // set params for gzip
        //gzsetparams(fpGeoJSON, Z_BEST_COMPRESSION, Z_DEFAULT_STRATEGY);

        // put the geojson preamble stuff
        if ( ! control.noForceGeoJSONFlag ) {
          gzprintf(fpGeoJSON, "var geojson =\n" );
        }
        gzprintf(fpGeoJSON,
                 "{ \"type\": \"FeatureCollection\",\n"
                 // todo - correct way to specify this?
                 "\"crs\": { \"type\": \"name\", \"properties\": { \"name\": \"mcpe_viz-image\" } },\n"
                 "\"features\": [\n"
                 );

        // put the list with correct commas (silly)
        int32_t i = listGeoJSON.size();
        for ( const auto& it: listGeoJSON ) {
          gzputs(fpGeoJSON, it.c_str());
          if ( --i > 0 ) {
            gzputc(fpGeoJSON,',');
          }
          gzputc(fpGeoJSON,'\n');
        }

        // close out the geojson properly
        if ( control.noForceGeoJSONFlag ) {
          gzprintf(fpGeoJSON,"] }\n");
        } else {
          gzprintf(fpGeoJSON,"] };\n");
        }

        gzclose(fpGeoJSON);
#endif
            } else {

                // plain text file version

                FILE *fpGeoJSON = fopen(control.fnGeoJSON.c_str(), "w");
                if (!fpGeoJSON) {
                    slogger.msg(kLogInfo1, "ERROR: Failed to create GeoJSON output file (%s error=%s (%d)).\n",
                                control.fnGeoJSON.c_str(), strerror(errno), errno);
                    return -1;
                }

                // put the geojson preamble stuff
                if (!control.noForceGeoJSONFlag) {
                    fprintf(fpGeoJSON, "var geojson =\n");
                }
                fprintf(fpGeoJSON,
                        "{ \"type\": \"FeatureCollection\",\n"
                        // todo - correct way to specify this?
                        "\"crs\": { \"type\": \"name\", \"properties\": { \"name\": \"mcpe_viz-image\" } },\n"
                        "\"features\": [\n"
                );

                // put the list with correct commas (silly)
                int32_t i = listGeoJSON.size();
                for (const auto &it: listGeoJSON) {
                    fputs(it.c_str(), fpGeoJSON);
                    if (--i > 0) {
                        fputc(',', fpGeoJSON);
                    }
                    fputc('\n', fpGeoJSON);
                }

                // close out the geojson properly
                if (control.noForceGeoJSONFlag) {
                    fprintf(fpGeoJSON, "] }\n");
                } else {
                    fprintf(fpGeoJSON, "] };\n");
                }

                fclose(fpGeoJSON);

            }
            return 0;
        }


        int32_t doOutput() {
            calcChunkBounds();

            // todonow todobig todostopper -- how to handle worlds that are larger than png dimensional limits (see midgard world file)

            for (int32_t i = 0; i < kDimIdCount; i++) {
                dimDataList[i]->doOutput(db);
            }

            if (control.doHtml) {

                if (control.autoTileFlag) {
                    int32_t xdimId = kDimIdOverworld;
                    const int32_t chunkW = (dimDataList[xdimId]->getMaxChunkX() - dimDataList[xdimId]->getMinChunkX() +
                                            1);
                    const int32_t imageW = chunkW * 16;
                    const int32_t chunkH = (dimDataList[xdimId]->getMaxChunkZ() - dimDataList[xdimId]->getMinChunkZ() +
                                            1);
                    const int32_t imageH = chunkH * 16;

                    // todobig - 5000 a reasonable default max image size before we auto-tile?
                    int32_t maxImageSize = 5000;
                    if (imageW > maxImageSize || imageH > maxImageSize) {
                        slogger.msg(kLogInfo1, "Detected large images and 'auto-tile' is enabled, enabling tiles!\n");
                        control.doTiles = true;
                    }
                }

                doOutput_Tile();
                doOutput_html();
                doOutput_GeoJSON();
            }

            if (control.colorTestFlag) {
                doOutput_colortest();
            }

            return 0;
        }

        void worldPointToImagePoint(int32_t dimId, double wx, double wz, double &ix, double &iy, bool geoJsonFlag) {
            // hack to avoid using wrong dim on pre-0.12 worlds
            if (dimId < 0) { dimId = 0; }

            return dimDataList[dimId]->worldPointToImagePoint(wx, wz, ix, iy, geoJsonFlag);
        }

    };

    std::unique_ptr<MinecraftWorld_LevelDB> world;


    void worldPointToImagePoint(int32_t dimId, double wx, double wz, double &ix, double &iy, bool geoJsonFlag) {
        return world->worldPointToImagePoint(dimId, wx, wz, ix, iy, geoJsonFlag);
    }

    void worldPointToGeoJSONPoint(int32_t dimId, double wx, double wz, double &ix, double &iy) {
        worldPointToImagePoint(dimId, wx, wz, ix, iy, true);
    }


    int32_t doParseConfigFile(const std::string &fn) {
        if (!file_exists(fn.c_str())) {
            return -1;
        }

        // todo - this should use streams

        const char *hdr = "";
        int32_t indent = 1;

        FILE *fp = fopen(fn.c_str(), "r");
        if (!fp) {
            slogger.msg(kLogInfo1, "ERROR: Failed to open file (%s error=%s (%d)\n", fn.c_str(), strerror(errno),
                        errno);
            return 1;
        }

        slogger.msg(kLogInfo1, "Reading config from %s\n", fn.c_str());

        char buf[1025], *p;
        while (!feof(fp)) {
            memset(buf, 0, 1025);
            fgets(buf, 1024, fp);

            // remove comments and newlines
            if ((p = strchr(buf, '#'))) {
                *p = 0;
            }
            if ((p = strchr(buf, '\n'))) {
                *p = 0;
            }
            if ((p = strchr(buf, '\r'))) {
                *p = 0;
            }

            if ((p = strstr(buf, "hide-top:"))) {
                int32_t dimId = -1;
                int32_t blockId = -1;
                int32_t pass = false;
                if (sscanf(&p[9], "%d 0x%x", &dimId, &blockId) == 2) {
                    pass = true;
                } else if (sscanf(&p[9], "%d %d", &dimId, &blockId) == 2) {
                    pass = true;
                }
                // check dimId
                if (dimId < kDimIdOverworld || dimId >= kDimIdCount) {
                    pass = false;
                }
                if (pass) {
                    // add to hide list
                    world->dimDataList[dimId]->blockHideList.push_back(blockId);
                } else {
                    slogger.msg(kLogInfo1, "%sERROR: Failed to parse cfg item 'hide-top': [%s]\n",
                                makeIndent(indent, hdr).c_str(), buf);
                }
            } else if ((p = strstr(buf, "force-top:"))) {
                int32_t dimId = -1;
                int32_t blockId = -1;
                int32_t pass = false;
                if (sscanf(&p[10], "%d 0x%x", &dimId, &blockId) == 2) {
                    pass = true;
                } else if (sscanf(&p[10], "%d %d", &dimId, &blockId) == 2) {
                    pass = true;
                }
                // check dimId
                if (dimId < kDimIdOverworld || dimId >= kDimIdCount) {
                    pass = false;
                }
                if (pass) {
                    // add to hide list
                    world->dimDataList[dimId]->blockForceTopList.push_back(blockId);
                } else {
                    slogger.msg(kLogInfo1, "%sERROR: Failed to parse cfg item 'force-top': [%s]\n",
                                makeIndent(indent, hdr).c_str(), buf);
                }
            } else if ((p = strstr(buf, "geojson-block:"))) {
                int32_t dimId = -1;
                int32_t blockId = -1;
                int32_t pass = false;
                if (sscanf(&p[14], "%d 0x%x", &dimId, &blockId) == 2) {
                    pass = true;
                } else if (sscanf(&p[14], "%d %d", &dimId, &blockId) == 2) {
                    pass = true;
                }
                // check dimId
                if (dimId < kDimIdOverworld || dimId >= kDimIdCount) {
                    pass = false;
                }
                if (pass) {
                    // add to list
                    world->dimDataList[dimId]->blockToGeoJSONList.push_back(blockId);
                } else {
                    slogger.msg(kLogInfo1, "%sERROR: Failed to parse cfg item 'geojson-block': [%s]\n",
                                makeIndent(indent, hdr).c_str(), buf);
                }
            } else if ((p = strstr(buf, "player-id:"))) {
                if (parsePlayerIdToName(&p[strlen("player-id:")]) == 0) {
                    // all good
                } else {
                    slogger.msg(kLogInfo1, "%sWARNING: Unparsed config line: [%s]\n", makeIndent(indent, hdr).c_str(),
                                buf);
                }
            }

        }

        fclose(fp);
        return 0;
    }


    int32_t parseConfigFile() {
        // parse cfg files in this order:
        // -- option specified on command-line
        // -- master dir
        // -- exec dir
        // -- local dir
        std::string fn;

        // same dir as exec - special filename
        fn = dirExec;
        fn += "/mcpe_viz.local.cfg";
        if (doParseConfigFile(fn) == 0) {
            // we keep reading other config files
        }

        // as specified on cmdline
        if (control.fnCfg.size() > 0) {
            if (doParseConfigFile(control.fnCfg) == 0) {
                return 0;
            }
        }

        // default config file
        // todo - how to support on win32? %HOMEPATH%?
        if (getenv("HOME")) {
            std::string fnHome = getenv("HOME");
            fnHome += "/.mcpe_viz/mcpe_viz.cfg";
            if (doParseConfigFile(fnHome) == 0) {
                return 0;
            }
        }

        // same dir as exec
        fn = dirExec;
        fn += "/mcpe_viz.cfg";
        if (doParseConfigFile(fn) == 0) {
            return 0;
        }

        // local dir
        fn = "./mcpe_viz.cfg";
        if (doParseConfigFile(fn) == 0) {
            return 0;
        }

        //slogger.msg(kLogInfo1,"WARNING: Did not find a valid config file\n");
        return -1;
    }


    int32_t parseXml() {
        // parse xml file in this order:
        // -- option specified on command-line
        // -- asset dir(./data/ or /usr/local/share/bedrock-viz/data/, etc)
        std::string fn;
        int32_t ret;
        char tmpstring[256];

        // initialize lists
        // todobig - others?
        for (int32_t i = 0; i < 512; i++) {
            sprintf(tmpstring, "(unknown-id-0x%02x)", i);
            blockInfoList[i].setName(tmpstring);
            blockInfoList[i].valid = false;
        }

        // as specified on cmdline
        if (control.fnXml.length() > 0) {
            ret = doParseXml(control.fnXml);
            if (ret >= 0) {
                return ret;
            }
        }

        // in asset folder
        fn = bedrock_viz::xml_path();
        ret = doParseXml(fn);
        if (ret >= 0) {
            return ret;
        }

        slogger.msg(kLogInfo1, "ERROR: Did not find a valid XML file\n");
        return -1;
    }


    // a developer's function to match MCPE Viz block + items with PNG files from minecraft wiki
    int32_t findImages() {
        std::string dirSrc = control.dirFindImagesIn;
        std::string dirDest = control.dirFindImagesOut;

        // clear block list
        for (int i = 0; i < 512; i++) {
            if (blockInfoList[i].isValid()) {
                if (blockInfoList[i].hasVariants()) {
                    for (const auto &itbv : blockInfoList[i].variantList) {
                        itbv->setUserVar1(0);
                    }
                }
                blockInfoList[i].setUserVar1(0);
            }
        }

        // clear item list
        for (auto &iter : itemInfoList) {
            iter.second->setUserVar1(0);
        }

        // get list of input images
        // they are named like this: blockId.blockData.blockName.png (e.g. "1.0.Stone.png")
        namespace fs = std::filesystem;
//    struct dirent *dp;
//    DIR *dfd = opendir(dirSrc.c_str());

//    if (dfd != NULL) {
//      while ((dp = readdir(dfd)) != NULL) {
        for (auto &p: fs::directory_iterator(dirSrc)) {
//        if ( strcmp(dp->d_name,".") == 0 || strcmp(dp->d_name,"..") == 0 ) {
            // skip
//        } else {
            std::string fnSrc(p.path().filename().generic_string());
            int32_t blockId = -1, blockData = -1;
            char blockName[1025];
            memset(blockName, 0, 1025);
            if (sscanf(fnSrc.c_str(), "%d.%d.%s.png", &blockId, &blockData, blockName) == 3) {

                //slogger.msg(kLogInfo1, "Parsed (%d) (%d) (%s) (%s)\n", blockId, blockData, blockName, fnSrc.c_str());

                // check for mapping mcpc to mcpe
                if (has_key(mcpcToMcpeBlock, blockId)) {
                    blockId = mcpcToMcpeBlock[blockId];
                }
                if (has_key(mcpcToMcpeItem, blockId)) {
                    blockId = mcpcToMcpeItem[blockId];
                }

                bool found = false;

                if (blockId < 512) {
                    // it's a block, look it up

                    if (blockInfoList[blockId].isValid()) {
                        if (blockInfoList[blockId].hasVariants()) {
                            for (const auto &itbv : blockInfoList[blockId].variantList) {
                                if (itbv->blockdata == blockData) {
                                    slogger.msg(kLogInfo1, "FOUND Block: %s (%d) (%d) (%s)\n", itbv->name.c_str(),
                                                blockId, blockData, blockName);
                                    itbv->deltaUserVar1(1);
                                    itbv->setUserString1(dirSrc + "/" + fnSrc);
                                    found = true;
                                }
                            }
                        } else {
                            if (blockData == 0) {
                                slogger.msg(kLogInfo1, "FOUND Block: %s (%d) (%d) (%s)\n",
                                            blockInfoList[blockId].name.c_str(), blockId, blockData, blockName);
                                blockInfoList[blockId].deltaUserVar1(1);
                                blockInfoList[blockId].setUserString1(dirSrc + "/" + fnSrc);
                                found = true;
                            }
                        }
                    }

                } else {
                    if (has_key(itemInfoList, blockId)) {
                        if (itemInfoList[blockId]->hasVariants()) {
                            for (const auto &itbv : itemInfoList[blockId]->variantList) {
                                if (itbv->extraData == blockData) {
                                    slogger.msg(kLogInfo1, "FOUND  Item: %s (%d) (%d) (%s)\n", itbv->name.c_str(),
                                                blockId, blockData, blockName);
                                    itbv->deltaUserVar1(1);
                                    itbv->setUserString1(dirSrc + "/" + fnSrc);
                                    found = true;
                                }
                            }
                        } else {
                            if (blockData == 0) {
                                slogger.msg(kLogInfo1, "FOUND  Item: %s (%d) (%d) (%s)\n",
                                            itemInfoList[blockId]->name.c_str(), blockId, blockData, blockName);
                                itemInfoList[blockId]->deltaUserVar1(1);
                                itemInfoList[blockId]->setUserString1(dirSrc + "/" + fnSrc);
                                found = true;
                            }
                        }
                    }
                }

                if (!found) {
                    slogger.msg(kLogInfo1, "-- WARNING: did not find target for 0x%04x 0x%02x (%s)\n", blockId,
                                blockData, blockName);
                }

            } else {
                slogger.msg(kLogWarning, "** Failed to parse filename (%s)\n", fnSrc.c_str());
            }
//        }
        }
//      closedir(dfd);
//    } else {
//      slogger.msg(kLogError, "Failed to open source dir (%s)\n", dirSrc.c_str());
//      return -1;
//    }

        // check blocks
        slogger.msg(kLogInfo1, "\n** BLOCK SUMMARY **\n");
        for (int i = 0; i < 512; i++) {
            if (blockInfoList[i].isValid()) {
                if (blockInfoList[i].hasVariants()) {
                    for (const auto &itbv : blockInfoList[i].variantList) {
                        if (itbv->getUserVar1() == 0) {
                            slogger.msg(kLogInfo1, "Did NOT find block-variant: %d %d %s\n", i, itbv->blockdata,
                                        itbv->name.c_str());
                        }
                        if (itbv->getUserVar1() > 1) {
                            slogger.msg(kLogInfo1, "Found multiple (%d) for block-variant: %d %d %s\n",
                                        itbv->getUserVar1(), i, itbv->blockdata, itbv->name.c_str());
                        }
                    }
                } else {
                    if (blockInfoList[i].getUserVar1() == 0) {
                        slogger.msg(kLogInfo1, "Did NOT find block: %d %s\n", i, blockInfoList[i].name.c_str());
                    }
                    if (blockInfoList[i].getUserVar1() > 1) {
                        slogger.msg(kLogInfo1, "Found multiple (%d) for block: %d %s\n", blockInfoList[i].getUserVar1(),
                                    i, blockInfoList[i].name.c_str());
                    }
                }
            }
        }

        // clear item list
        slogger.msg(kLogInfo1, "\n** ITEM SUMMARY **\n");
        for (auto &iter : itemInfoList) {
            if (iter.second->hasVariants()) {
                for (const auto &itbv : iter.second->variantList) {
                    if (itbv->getUserVar1() == 0) {
                        slogger.msg(kLogInfo1, "Did NOT find item-variant: %d %d %s\n", iter.first, itbv->extraData,
                                    itbv->name.c_str());
                    }
                    if (itbv->getUserVar1() > 1) {
                        slogger.msg(kLogInfo1, "Found multiple (%d) for item-variant: %d %d %s\n", itbv->getUserVar1(),
                                    iter.first, itbv->extraData, itbv->name.c_str());
                    }
                }
            } else {
                if (iter.second->getUserVar1() == 0) {
                    slogger.msg(kLogInfo1, "Did NOT find item: %d %s\n", iter.first, iter.second->name.c_str());
                }
                if (iter.second->getUserVar1() > 1) {
                    slogger.msg(kLogInfo1, "Found multiple (%d) for item: %d %s\n", iter.second->getUserVar1(),
                                iter.first, iter.second->name.c_str());
                }
            }
        }

        // process found blocks
        // todobig - might be faster/cooler to make a sprite sheet
        char fnDest[1025];
        for (int i = 0; i < 512; i++) {
            if (blockInfoList[i].isValid()) {
                if (blockInfoList[i].hasVariants()) {
                    for (const auto &itbv : blockInfoList[i].variantList) {
                        if (itbv->getUserVar1() > 0) {
                            sprintf(fnDest, "%s/mcpe_viz.block.%d.%d.png", dirDest.c_str(), i, itbv->blockdata);
                            copyFile(itbv->getUserString1(), fnDest, false);
                        }
                    }
                } else {
                    if (blockInfoList[i].getUserVar1() > 0) {
                        sprintf(fnDest, "%s/mcpe_viz.block.%d.%d.png", dirDest.c_str(), i, blockInfoList[i].blockdata);
                        copyFile(blockInfoList[i].getUserString1(), fnDest, false);
                    }
                }
            }
        }

        // process found items
        for (auto &iter : itemInfoList) {
            if (iter.second->hasVariants()) {
                for (const auto &itbv : iter.second->variantList) {
                    if (itbv->getUserVar1() > 0) {
                        sprintf(fnDest, "%s/mcpe_viz.item.%d.%d.png", dirDest.c_str(), iter.first, itbv->extraData);
                        copyFile(itbv->getUserString1(), fnDest, false);
                    }
                }
            } else {
                if (iter.second->getUserVar1() > 0) {
                    sprintf(fnDest, "%s/mcpe_viz.item.%d.%d.png", dirDest.c_str(), iter.first, iter.second->extraData);
                    copyFile(iter.second->getUserString1(), fnDest, false);
                }
            }
        }
        return 0;
    }


    void print_usage(const char *fn) {
        slogger.msg(kLogInfo1, "Usage:\n\n");
        slogger.msg(kLogInfo1, "  %s [required parameters] [options]\n\n", fn);
        slogger.msg(kLogInfo1, "Required Parameters:\n"
                               "  --db dir                 Directory which holds world files (level.dat is in this dir)\n"
                               "  --out fn-part            Filename base for output file(s)\n"
                               "\n"
        );
        slogger.msg(kLogInfo1, "Options:\n"
                               //"  --detail                 Log extensive details about the world to the log file\n"
                               "  --html                   Create html and javascript files to use as a fancy viewer\n"
                               "  --html-most              Create html, javascript, and most image files to use as a fancy viewer\n"
                               "  --html-all               Create html, javascript, and *all* image files to use as a fancy viewer\n"
                               //"  --dir-temp dir           Directory for temp files (useful for --slices, use a fast, local directory)\n"
                               "  --auto-tile              Automatically tile the images if they are very large\n"
                               "  --tiles[=tilew,tileh]    Create tiles in subdirectory tiles/ (useful for LARGE worlds)\n"
                               "\n"
                               "  --hide-top=did,bid       Hide a block from top block (did=dimension id, bid=block id)\n"
                               "  --force-top=did,bid      Force a block to top block (did=dimension id, bid=block id)\n"
                               "  --geojson-block=did,bid  Add block to GeoJSON file for use in web app (did=dimension id, bid=block id)\n"
                               "\n"
                               "  --check-spawn did,x,z,dist  Add spawnable blocks to the geojson file (did=dimension id; checks a circle of radius 'dist' centered on x,z)\n"
                               "  --schematic-get did,x1,y1,z1,x2,y2,z2,fnpart   Create a schematic file (fnpart) from (x1,y1,z1) to (x2,y2,z2) in dimension (did)\n"
                               "\n"
                               "  (note: [=did] is optional dimension-id - if not specified, do all dimensions; 0=Overworld; 1=Nether)\n"
                               "  --grid[=did]             Display chunk grid on top of images\n"
                               "\n"
                               "  --all-image[=did]        Create all image types\n"
                               "  --biome[=did]            Create a biome map image\n"
                               "  --grass[=did]            Create a grass color map image\n"
                               "  --height-col[=did]       Create a height column map image (red is below sea; gray is sea; green is above sea)\n"
                               "  --height-col-gs[=did]    Create a height column map image (grayscale)\n"
                               "  --height-col-alpha[=did] Create a height column map image (alpha)\n"
                               "  --shaded-relief[=did]    Create a shaded relief image\n"
                               "  --blocklight[=did]       Create a block light map image\n"
                               "  --skylight[=did]         Create a sky light map image\n"
                               "  --slime-chunk[=did]      Create a slime chunk map image\n"
                               "\n"
                               "  --slices[=did]           Create slices (one image for each layer)\n"
                               "  --movie[=did]            Create movie of layers\n"
                               "  --movie-dim x,y,w,h      Integers describing the bounds of the movie (UL X, UL Y, WIDTH, HEIGHT)\n"
                               "\n"
                               "  --xml fn                 XML file containing data definitions\n"
                               "  --log fn                 Send log to a file\n"
                               "\n"
                               "  --no-force-geojson       Don't load geojson in html because we are going to use a web server (or Firefox)\n"
                               "\n"
                               "  --verbose                verbose output\n"
                               "  --quiet                  supress normal output, continue to output warning and error messages\n"
                               "  --help                   this info\n"
        );
    }

    int32_t parseDimIdOptArg(const char *arg) {
        int32_t did = kDoOutputAll;
        if (arg) {
            did = atoi(arg);

            // sanity check
            if (did >= kDimIdOverworld && did < kDimIdCount) {
                // all is good
            } else {
                slogger.msg(kLogInfo1, "ERROR: Invalid dimension-id supplied (%d), defaulting to Overworld only\n",
                            did);
                did = kDimIdOverworld;
            }
        } else {
            // if no arg, we want output for all dimensions
        }
        return did;
    }

    int32_t parse_args(int argc, char **argv) {

        static struct option longoptlist[] = {
                {"db",                 required_argument, NULL, 'D'},
                {"out",                required_argument, NULL, 'O'},

                {"xml",                required_argument, NULL, 'X'},
                {"log",                required_argument, NULL, 'L'},

                {"detail",             no_argument,       NULL, '@'},

                {"hide-top",           required_argument, NULL, 'H'},
                {"force-top",          required_argument, NULL, 'F'},
                {"geojson-block",      required_argument, NULL, '+'},

                {"check-spawn",        required_argument, NULL, 'C'},
                {"check-spawnable",    required_argument, NULL, 'C'},

                {"schematic",          required_argument, NULL, 'Z'},
                {"schematic-get",      required_argument, NULL, 'Z'},

                {"all-image",          optional_argument, NULL, 'A'},
                {"biome",              optional_argument, NULL, 'B'},
                {"grass",              optional_argument, NULL, 'g'},
                {"height-col",         optional_argument, NULL, 'd'},
                {"height-col-gs",      optional_argument, NULL, '#'},
                {"height-col-alpha",   optional_argument, NULL, 'a'},
                {"shaded-relief",      optional_argument, NULL, 'S'},
                {"blocklight",         optional_argument, NULL, 'b'},
                {"skylight",           optional_argument, NULL, 's'},
                {"slime-chunk",        optional_argument, NULL, '%'},

                {"slices",             optional_argument, NULL, '('},

                {"movie",              optional_argument, NULL, 'M'},
                {"movie-dim",          required_argument, NULL, '*'},

                {"grid",               optional_argument, NULL, 'G'},

                {"html",               no_argument,       NULL, ')'},
                {"html-most",          no_argument,       NULL, '='},
                {"html-all",           no_argument,       NULL, '_'},
                {"no-force-geojson",   no_argument,       NULL, ':'},

                {"auto-tile",          no_argument,       NULL, ']'},
                {"tiles",              optional_argument, NULL, '['},

                {"shortrun",           no_argument,       NULL, '$'}, // this is just for testing
                {"colortest",          no_argument,       NULL, '!'}, // this is just for testing

                {"flush",              no_argument,       NULL, 'f'},

                {"leveldb-filter",     required_argument, NULL, '<'},
                {"leveldb-block-size", required_argument, NULL, '>'},

                {"find-images",        required_argument, NULL, '"'},

                {"verbose",            no_argument,       NULL, 'v'},
                {"quiet",              no_argument,       NULL, 'q'},
                {"help",               no_argument,       NULL, 'h'},
                {NULL,                 no_argument,       NULL, 0}
        };

        int32_t option_index = 0;
        int32_t optc;
        int32_t errct = 0;

        control.init();

        while ((optc = getopt_long_only(argc, argv, "", longoptlist, &option_index)) != -1) {
            switch (optc) {
                case 'O':
                    control.fnOutputBase = optarg;
                    break;
                case 'X':
                    control.fnXml = optarg;
                    break;
                case 'L':
                    control.fnLog = optarg;
                    break;
                case 'D':
                    control.dirLeveldb = optarg;
                    break;
                case '@':
                    control.doDetailParseFlag = true;
                    break;
                case 'f':
                    slogger.setFlush(true);
                    break;

                case '<':
                    control.leveldbFilter = atoi(optarg);
                    if (control.leveldbFilter < 0) {
                        control.leveldbFilter = 0;
                    }
                    break;
                case '>':
                    control.leveldbBlockSize = atoi(optarg);
                    if (control.leveldbBlockSize < 0) {
                        control.leveldbBlockSize = 4096;
                    }
                    break;

                case '"':
                    control.doFindImages = true;
                    // todobig - specify an in and an out dir
                    control.dirFindImagesIn = optarg;
                    control.dirFindImagesOut = optarg;
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
                            world->dimDataList[dimId]->blockHideList.push_back(blockId);
                        }
                    }

                    if (!pass) {
                        slogger.msg(kLogInfo1, "ERROR: Failed to parse --hide-top %s\n", optarg);
                        errct++;
                    }
                }
                    break;

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
                            world->dimDataList[dimId]->blockForceTopList.push_back(blockId);
                        }
                    }

                    if (!pass) {
                        slogger.msg(kLogInfo1, "ERROR: Failed to parse --force-top %s\n", optarg);
                        errct++;
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
                            world->dimDataList[dimId]->blockToGeoJSONList.push_back(blockId);
                        }
                    }

                    if (!pass) {
                        slogger.msg(kLogInfo1, "ERROR: Failed to parse --geojson-block %s\n", optarg);
                        errct++;
                    }
                }
                    break;

                case 'C': {
                    slogger.msg(kLogInfo1,
                                "ERROR: --spawnable is no longer supported because the new chunk format (circa beta 1.2.x) no longer stores block light info\n");
                    errct++;

                    bool pass = false;
                    int32_t dimId, checkX, checkZ, checkDistance;
                    if (sscanf(optarg, "%d,%d,%d,%d", &dimId, &checkX, &checkZ, &checkDistance) == 4) {
                        pass = true;
                    }

                    if (pass) {
                        // todo - check params

                        if (dimId < kDimIdOverworld || dimId >= kDimIdCount) {
                            pass = false;
                        }

                        if (pass) {
                            world->dimDataList[dimId]->addCheckSpawn(checkX, checkZ, checkDistance);
                        }
                    }

                    if (!pass) {
                        slogger.msg(kLogInfo1, "ERROR: Failed to parse --check-spawn %s\n", optarg);
                        errct++;
                    }
                }
                    break;


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
                            world->dimDataList[dimId]->addSchematic(x1, y1, z1, x2, y2, z2, fnSchematic);
                        }
                    }

                    if (!pass) {
                        slogger.msg(kLogInfo1, "ERROR: Failed to parse --schematic %s\n", optarg);
                        errct++;
                    }
                }
                    break;


                case 'G':
                    control.doGrid = parseDimIdOptArg(optarg);
                    break;

                case ')':
                    control.doHtml = true;
                    break;

                case '[':
                    control.doTiles = true;
                    {
                        int32_t tw, th;
                        if (optarg) {
                            if (sscanf(optarg, "%d,%d", &tw, &th) == 2) {
                                control.tileWidth = tw;
                                control.tileHeight = th;
                                slogger.msg(kLogInfo1, "Overriding tile dimensions: %d x %d\n", control.tileWidth,
                                            control.tileHeight);
                            }
                        }
                    }
                    break;
                case ']':
                    control.autoTileFlag = true;
                    break;

                case '=':
                    // html most
                    control.doHtml = true;
                    control.doImageBiome =
                    control.doImageGrass =
                    control.doImageHeightCol =
                    control.doImageHeightColGrayscale =
                    control.doImageHeightColAlpha =
                    control.doImageShadedRelief =
                    control.doImageLightBlock =
                    control.doImageLightSky =
                    control.doImageSlimeChunks =
                            kDoOutputAll;
                    break;

                case '_':
                    // html all
                    control.doHtml = true;
                    control.doImageBiome =
                    control.doImageGrass =
                    control.doImageHeightCol =
                    control.doImageHeightColGrayscale =
                    control.doImageHeightColAlpha =
                    control.doImageShadedRelief =
                    control.doImageLightBlock =
                    control.doImageLightSky =
                    control.doImageSlimeChunks =
                            kDoOutputAll;
                    control.doSlices = kDoOutputAll;
                    break;

                case ':':
                    control.noForceGeoJSONFlag = true;
                    break;

                case 'B':
                    control.doImageBiome = parseDimIdOptArg(optarg);
                    break;
                case 'g':
                    control.doImageGrass = parseDimIdOptArg(optarg);
                    break;
                case 'd':
                    control.doImageHeightCol = parseDimIdOptArg(optarg);
                    break;
                case '#':
                    control.doImageHeightColGrayscale = parseDimIdOptArg(optarg);
                    break;
                case 'a':
                    control.doImageHeightColAlpha = parseDimIdOptArg(optarg);
                    break;
                case 'S':
                    control.doImageShadedRelief = parseDimIdOptArg(optarg);
                    break;
                case 'b':
                    control.doImageLightBlock = parseDimIdOptArg(optarg);
                    break;
                case 's':
                    control.doImageLightSky = parseDimIdOptArg(optarg);
                    break;
                case '%':
                    control.doImageSlimeChunks = parseDimIdOptArg(optarg);
                    break;

                case 'A':
                    control.doImageBiome =
                    control.doImageGrass =
                    control.doImageHeightCol =
                    control.doImageHeightColGrayscale =
                    control.doImageHeightColAlpha =
                    control.doImageShadedRelief =
                    control.doImageLightBlock =
                    control.doImageLightSky =
                    control.doImageSlimeChunks =
                            parseDimIdOptArg(optarg);
                    break;

                case '(':
                    control.doSlices = parseDimIdOptArg(optarg);
                    break;
                case 'M':
                    control.doMovie = parseDimIdOptArg(optarg);
                    break;
                case '*':
                    // movie dimensions
                    if (sscanf(optarg, "%d,%d,%d,%d", &control.movieX, &control.movieY, &control.movieW,
                               &control.movieH) == 4) {
                        // good
                    } else {
                        slogger.msg(kLogInfo1, "ERROR: Failed to parse --movie-dim\n");
                        errct++;
                    }
                    break;

                case '$':
                    control.shortRunFlag = true;
                    break;
                case '!':
                    control.colorTestFlag = true;
                    break;

                case 'v':
                    control.verboseFlag = true;
                    break;
                case 'q':
                    control.quietFlag = true;
                    logger.setLogLevelMask(kLogQuiet);
                    break;

                    /* Usage */
                default:
                    slogger.msg(kLogInfo1, "ERROR: Unrecognized option: '%c'\n", optc);
                    return -1;

                case 'h':
                    return -1;
            }
        }

        // todobig - be more clever about dirLeveldb -- allow it to be the dir or the level.dat file

        // verify/test args
        if (control.dirLeveldb.length() <= 0) {
            errct++;
            slogger.msg(kLogInfo1, "ERROR: Must specify --db\n");
        }
        if (control.fnOutputBase.length() <= 0) {
            errct++;
            slogger.msg(kLogInfo1, "ERROR: Must specify --out\n");
        }

        // make sure that output directory is NOT world data directory
        std::string fnTest = mydirname(control.fnOutputBase) + "/level.dat";
        if (file_exists(fnTest)) {
            errct++;
            slogger.msg(kLogInfo1, "ERROR: You cannot send mcpe_viz output to a world file data directory\n");
        }

        if (errct <= 0) {
            control.setupOutput();
        }

        return errct;
    }

    int32_t init(int argc, char **argv) {

        int32_t ret;

        dirExec = mydirname(argv[0]);

        world = std::unique_ptr<MinecraftWorld_LevelDB>(new MinecraftWorld_LevelDB());

        slogger.setStdout(stderr);
        slogger.setStderr(stderr);

        ret = parse_args(argc, argv);
        if (ret != 0) {
            print_usage(argv[0]);
            return ret;
        }

        ret = parseXml();
        if (ret != 0) {
            slogger.msg(kLogInfo1, "ERROR: Failed to parse XML file.  Exiting...\n");
            return -1;
        }

        parseConfigFile();

        makePalettes();

        return 0;
    }

}  // namespace mcpe_viz



int main(int argc, char **argv) {
    std::cerr << bedrock_viz::version_full << '\n';

    int32_t ret = mcpe_viz::init(argc, argv);
    if (ret != 0) {
        return -1;
    }

    if (mcpe_viz::control.doFindImages) {
        mcpe_viz::findImages();
        return 0;
    }

    mcpe_viz::world->init();
    mcpe_viz::world->dbOpen(std::string(mcpe_viz::control.dirLeveldb));
    // todobig - we must do this, for now - we could get clever about this later
    // todobig - we could call this deepParseDb() and only do it if the user wanted it
    if (true || mcpe_viz::control.doDetailParseFlag) {
        mcpe_viz::world->dbParse();
        mcpe_viz::world->checkSpawnable();
    }
    mcpe_viz::world->doOutput();
    mcpe_viz::world->dbClose();

    std::cerr << "Done.\n";

    return 0;
}
