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

#include <cstdio>
#include <map>
#include <vector>
#include <cmath>
#include <random>
#include <filesystem>
#include <iostream>
#include <string>
#include <cstdint>

#include <leveldb/db.h>
#include <leveldb/cache.h>

#include <pugixml.hpp>

#ifndef _MSC_VER

#include <getopt.h>

#else
#include "getopt/getopt.h"
#endif

#include "define.h"
#include "util.h"
#include "nbt.h"
#include "xml.h"
#include "asset.h"
#include "args.h"
#include "global.h"
#include "control.h"

#include "minecraft/block_info.h"
#include "minecraft/item_info.h"
#include "minecraft/conversion.h"
#include "utils/block_recorder.h"

#include "world/common.h"
#include "world/dimension_data.h"

#include "world/world.h"

#include "utils/fs.h"
#include "asset.h"

namespace mcpe_viz {

    int32_t doParseConfigFile(const std::string& fn) {
        if (!file_exists(fn)) {
            return -1;
        }

        // todo - this should use streams

        const char* hdr = "";
        int32_t indent = 1;

        FILE* fp = fopen(fn.c_str(), "r");
        if (!fp) {
            slogger.msg(kLogInfo1, "ERROR: Failed to open file (%s error=%s (%d)\n", fn.c_str(), strerror(errno),
                errno);
            return 1;
        }

        slogger.msg(kLogInfo1, "Reading config from %s\n", fn.c_str());

        char buf[1025], * p;
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
                }
                else if (sscanf(&p[9], "%d %d", &dimId, &blockId) == 2) {
                    pass = true;
                }
                // check dimId
                if (dimId < kDimIdOverworld || dimId >= kDimIdCount) {
                    pass = false;
                }
                if (pass) {
                    // add to hide list
                    world->dimDataList[dimId]->blockHideList.push_back(blockId);
                }
                else {
                    slogger.msg(kLogInfo1, "%sERROR: Failed to parse cfg item 'hide-top': [%s]\n",
                        makeIndent(indent, hdr).c_str(), buf);
                }
            }
            else if ((p = strstr(buf, "force-top:"))) {
                int32_t dimId = -1;
                int32_t blockId = -1;
                int32_t pass = false;
                if (sscanf(&p[10], "%d 0x%x", &dimId, &blockId) == 2) {
                    pass = true;
                }
                else if (sscanf(&p[10], "%d %d", &dimId, &blockId) == 2) {
                    pass = true;
                }
                // check dimId
                if (dimId < kDimIdOverworld || dimId >= kDimIdCount) {
                    pass = false;
                }
                if (pass) {
                    // add to hide list
                    world->dimDataList[dimId]->blockForceTopList.push_back(blockId);
                }
                else {
                    slogger.msg(kLogInfo1, "%sERROR: Failed to parse cfg item 'force-top': [%s]\n",
                        makeIndent(indent, hdr).c_str(), buf);
                }
            }
            else if ((p = strstr(buf, "geojson-block:"))) {
                int32_t dimId = -1;
                int32_t blockId = -1;
                int32_t pass = false;
                if (sscanf(&p[14], "%d 0x%x", &dimId, &blockId) == 2) {
                    pass = true;
                }
                else if (sscanf(&p[14], "%d %d", &dimId, &blockId) == 2) {
                    pass = true;
                }
                // check dimId
                if (dimId < kDimIdOverworld || dimId >= kDimIdCount) {
                    pass = false;
                }
                if (pass) {
                    // add to list
                    world->dimDataList[dimId]->blockToGeoJSONList.push_back(blockId);
                }
                else {
                    slogger.msg(kLogInfo1, "%sERROR: Failed to parse cfg item 'geojson-block': [%s]\n",
                        makeIndent(indent, hdr).c_str(), buf);
                }
            }
            else if ((p = strstr(buf, "player-id:"))) {
                if (parsePlayerIdToName(&p[strlen("player-id:")]) == 0) {
                    // all good
                }
                else {
                    slogger.msg(kLogInfo1, "%sWARNING: Unparsed config line: [%s]\n", makeIndent(indent, hdr).c_str(),
                        buf);
                }
            }

        }

        fclose(fp);
        return 0;
    }


    int32_t loadConfigFile() {
        // parse cfg files in this order:
        // -- option specified on command-line
        // -- home dir
        // -- local dir

        // as specified on cmdline
        if (!control.fnCfg.empty()) {
            if (doParseConfigFile(control.fnCfg) == 0) {
                return 0;
            }
        }

        // default config file
        // todo - how to support on win32? %HOMEPATH%?
        if (getenv("HOME")) {
            std::string fnHome = getenv("HOME");
            fnHome += "/.mcpe_viz.cfg";
            if (doParseConfigFile(fnHome) == 0) {
                return 0;
            }
        }

        // local dir
        if (doParseConfigFile(std::string("mcpe_viz.cfg")) == 0) {
            return 0;
        }

        //slogger.msg(kLogInfo1,"WARNING: Did not find a valid config file\n");
        return -1;
    }


    int32_t loadXml() {
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
        fn = bedrock_viz::xml_path().generic_string();
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
                    for (const auto& itbv : blockInfoList[i].variantList) {
                        itbv->setUserVar1(0);
                    }
                }
                blockInfoList[i].setUserVar1(0);
            }
        }

        // clear item list
        for (auto& iter : itemInfoList) {
            iter.second->setUserVar1(0);
        }

        // get list of input images
        // they are named like this: blockId.blockData.blockName.png (e.g. "1.0.Stone.png")
        namespace fs = std::filesystem;
        //    struct dirent *dp;
        //    DIR *dfd = opendir(dirSrc.c_str());

        //    if (dfd != NULL) {
        //      while ((dp = readdir(dfd)) != NULL) {
        for (auto& p : fs::directory_iterator(dirSrc)) {
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
                            for (const auto& itbv : blockInfoList[blockId].variantList) {
                                if (itbv->blockdata == blockData) {
                                    slogger.msg(kLogInfo1, "FOUND Block: %s (%d) (%d) (%s)\n", itbv->name.c_str(),
                                        blockId, blockData, blockName);
                                    itbv->deltaUserVar1(1);
                                    itbv->setUserString1(dirSrc + "/" + fnSrc);
                                    found = true;
                                }
                            }
                        }
                        else {
                            if (blockData == 0) {
                                slogger.msg(kLogInfo1, "FOUND Block: %s (%d) (%d) (%s)\n",
                                    blockInfoList[blockId].name.c_str(), blockId, blockData, blockName);
                                blockInfoList[blockId].deltaUserVar1(1);
                                blockInfoList[blockId].setUserString1(dirSrc + "/" + fnSrc);
                                found = true;
                            }
                        }
                    }

                }
                else {
                    if (has_key(itemInfoList, blockId)) {
                        if (itemInfoList[blockId]->hasVariants()) {
                            for (const auto& itbv : itemInfoList[blockId]->variantList) {
                                if (itbv->extraData == blockData) {
                                    slogger.msg(kLogInfo1, "FOUND  Item: %s (%d) (%d) (%s)\n", itbv->name.c_str(),
                                        blockId, blockData, blockName);
                                    itbv->deltaUserVar1(1);
                                    itbv->setUserString1(dirSrc + "/" + fnSrc);
                                    found = true;
                                }
                            }
                        }
                        else {
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

            }
            else {
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
                    for (const auto& itbv : blockInfoList[i].variantList) {
                        if (itbv->getUserVar1() == 0) {
                            slogger.msg(kLogInfo1, "Did NOT find block-variant: %d %d %s\n", i, itbv->blockdata,
                                itbv->name.c_str());
                        }
                        if (itbv->getUserVar1() > 1) {
                            slogger.msg(kLogInfo1, "Found multiple (%d) for block-variant: %d %d %s\n",
                                itbv->getUserVar1(), i, itbv->blockdata, itbv->name.c_str());
                        }
                    }
                }
                else {
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
        for (auto& iter : itemInfoList) {
            if (iter.second->hasVariants()) {
                for (const auto& itbv : iter.second->variantList) {
                    if (itbv->getUserVar1() == 0) {
                        slogger.msg(kLogInfo1, "Did NOT find item-variant: %d %d %s\n", iter.first, itbv->extraData,
                            itbv->name.c_str());
                    }
                    if (itbv->getUserVar1() > 1) {
                        slogger.msg(kLogInfo1, "Found multiple (%d) for item-variant: %d %d %s\n", itbv->getUserVar1(),
                            iter.first, itbv->extraData, itbv->name.c_str());
                    }
                }
            }
            else {
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
                    for (const auto& itbv : blockInfoList[i].variantList) {
                        if (itbv->getUserVar1() > 0) {
                            sprintf(fnDest, "%s/mcpe_viz.block.%d.%d.png", dirDest.c_str(), i, itbv->blockdata);
                            copyFile(itbv->getUserString1(), fnDest, false);
                        }
                    }
                }
                else {
                    if (blockInfoList[i].getUserVar1() > 0) {
                        sprintf(fnDest, "%s/mcpe_viz.block.%d.%d.png", dirDest.c_str(), i, blockInfoList[i].blockdata);
                        copyFile(blockInfoList[i].getUserString1(), fnDest, false);
                    }
                }
            }
        }

        // process found items
        for (auto& iter : itemInfoList) {
            if (iter.second->hasVariants()) {
                for (const auto& itbv : iter.second->variantList) {
                    if (itbv->getUserVar1() > 0) {
                        sprintf(fnDest, "%s/mcpe_viz.item.%d.%d.png", dirDest.c_str(), iter.first, itbv->extraData);
                        copyFile(itbv->getUserString1(), fnDest, false);
                    }
                }
            }
            else {
                if (iter.second->getUserVar1() > 0) {
                    sprintf(fnDest, "%s/mcpe_viz.item.%d.%d.png", dirDest.c_str(), iter.first, iter.second->extraData);
                    copyFile(iter.second->getUserString1(), fnDest, false);
                }
            }
        }
        return 0;
    }

    int32_t parseDimIdOptArg(const char* arg) {
        int32_t did = kDoOutputAll;
        if (arg) {
            did = atoi(arg);

            // sanity check
            if (did >= kDimIdOverworld && did < kDimIdCount) {
                // all is good
            }
            else {
                slogger.msg(kLogInfo1, "ERROR: Invalid dimension-id supplied (%d), defaulting to Overworld only\n",
                    did);
                did = kDimIdOverworld;
            }
        }
        else {
            // if no arg, we want output for all dimensions
        }
        return did;
    }

    int32_t parse_args(int argc, char** argv) {

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
                }
                else if (sscanf(optarg, "%d,%d", &dimId, &blockId) == 2) {
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
                }
                else if (sscanf(optarg, "%d,%d", &dimId, &blockId) == 2) {
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
                }
                else if (sscanf(optarg, "%d,%d", &dimId, &blockId) == 2) {
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
                }
                else {
                    slogger.msg(kLogInfo1, "ERROR: Failed to parse --movie-dim\n");
                    errct++;
                }
                break;

            case '$':
                control.shortRunFlag = true;
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

}  // namespace mcpe_viz

int main(int argc, char** argv) {
    using namespace mcpe_viz;
    using namespace bedrock_viz;
    set_argv_0(argv[0]);
    // steps:
    // 1. load args from argv
    // 2. check args, exit and print usage if any error found
    // 3. if doFindImages, run findImages, exit
    // 4. load xml and cfg
    // 5. create then world
    // 6. parse world
    // 7. generate output
    // 8. print warning of unknown blocks/items

    world = std::make_unique<MinecraftWorld_LevelDB>();

    slogger.setStdout(stderr);
    slogger.setStderr(stderr);

    if (parse_args(argc, argv) != 0) {
        mcpe_viz::print_usage();
        return -1;
    }

    if (loadXml() != 0) {
        slogger.msg(kLogInfo1, "ERROR: Failed to parse XML file.\n");
        return -1;
    }

    loadConfigFile();

    if (mcpe_viz::control.doFindImages) {
        mcpe_viz::findImages();
    }
    else {
        world->init();
        world->dbOpen(std::string(mcpe_viz::control.dirLeveldb));
        // todobig - we must do this, for now - we could get clever about this later
        // todobig - we could call this deepParseDb() and only do it if the user wanted it
        if (true || mcpe_viz::control.doDetailParseFlag) {
            world->dbParse();
            world->checkSpawnable();
        }
        world->doOutput();
        world->dbClose();

        print_unknown_block_warnings();

        std::cout << "Done.\n";
    }
    return 0;
}
