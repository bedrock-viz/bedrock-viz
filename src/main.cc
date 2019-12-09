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
#include <algorithm>
#include <cmath>
#include <random>
#include <filesystem>
#include <iostream>
#include <ctime>
#include <string>
#include <cstdint>

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
#include "args.h"
#include "global.h"
#include "control.h"

#include "minecraft/block_info.h"
#include "minecraft/item_info.h"
#include "minecraft/biome_info.h"
#include "minecraft/conversion.h"
#include "utils/block_recorder.h"

#include "world/common.h"
#include "world/dimension_data.h"

#include "world/palettes.h"

namespace mcpe_viz {

    // suggestion from mcpe_sample_setup.cpp
    class NullLogger : public leveldb::Logger {
    public:
        void Logv(const char*, va_list) override {
        }
    };


    // note: this is an attempt to remove "bad" chunks as seen in "nyan.zip" world
    inline bool legalChunkPos(int32_t chunkX, int32_t chunkZ) {
        if ((uint32_t)chunkX == 0x80000000 && (uint32_t)chunkZ == 0x80000000) {
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

        void setWorldName(const std::string& s) { worldName = s; }

        const std::string& getWorldName() const { return worldName; }

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
        leveldb::DB* db;
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

        int32_t parseLevelFile(const std::string& fname) {
            FILE* fp = fopen(fname.c_str(), "rb");
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
                char* buf = new char[bufLen];
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
            }
            else {
                fclose(fp);
            }

            return ret;
        }

        int32_t parseLevelName(const std::string& fname) {
            FILE* fp = fopen(fname.c_str(), "r");
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

        int32_t dbOpen(const std::string& dirDb) {
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
            leveldb::Iterator* iter = db->NewIterator(levelDbReadOptions);
            leveldb::Slice skey;
            int32_t key_size;
            const char* key;
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
                }
                else if (key_size == 13) {
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
                }
                else if (key_size == 14) {
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
                    for (const auto& iter : dimDataList[dimId]->blockHideList) {
                        blockId = iter;
                        slogger.msg(kLogInfo1, "  'hide-top' block: %s - %s (dimId=%d blockId=%d (0x%02x))\n",
                            dimDataList[dimId]->getName().c_str(), blockInfoList[blockId].name.c_str(), dimId,
                            blockId, blockId);
                        itemCt++;
                    }

                    for (const auto& iter : dimDataList[dimId]->blockForceTopList) {
                        blockId = iter;
                        slogger.msg(kLogInfo1, "  'force-top' block: %s - %s (dimId=%d blockId=%d (0x%02x))\n",
                            dimDataList[dimId]->getName().c_str(), blockInfoList[blockId].name.c_str(), dimId,
                            blockId, blockId);
                        itemCt++;
                    }

                    for (const auto& iter : dimDataList[dimId]->blockToGeoJSONList) {
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
            const char* key;
            const char* cdata;
            std::string dimName, chunkstr;

            leveldb::Iterator* iter = db->NewIterator(levelDbReadOptions);
            for (iter->SeekToFirst(); iter->Valid(); iter->Next()) {

                // note: we get the raw buffer early to avoid overhead (maybe?)
                skey = iter->key();
                key_size = (int)skey.size();
                key = skey.data();

                svalue = iter->value();
                cdata_size = svalue.size();
                cdata = svalue.data();

                ++recordCt;
                if (control.shortRunFlag && recordCt > 1000) {
                    break;
                }
                if ((recordCt % 10000) == 0) {
                    double pct = (double)recordCt / (double)totalRecordCt;
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
                }
                else if (strncmp(key, "Overworld", key_size) == 0) {
                    logger.msg(kLogInfo1, "Overworld value:\n");
                    parseNbt("Overworld: ", cdata, cdata_size, tagList);
                    // todo - parse tagList? a list of "LimboEntities"
                }
                else if (strncmp(key, "~local_player", key_size) == 0) {
                    logger.msg(kLogInfo1, "Local Player value:\n");
                    ret = parseNbt("Local Player: ", cdata, cdata_size, tagList);
                    if (ret == 0) {
                        parseNbt_entity(-1, "", tagList, true, false, "Local Player", "");
                    }
                }
                else if ((key_size >= 7) && (strncmp(key, "player_", 7) == 0)) {
                    // note: key contains player id (e.g. "player_-1234")
                    std::string playerRemoteId = std::string(&key[strlen("player_")], key_size - strlen("player_"));

                    logger.msg(kLogInfo1, "Remote Player (id=%s) value:\n", playerRemoteId.c_str());

                    ret = parseNbt("Remote Player: ", cdata, cdata_size, tagList);
                    if (ret == 0) {
                        parseNbt_entity(-1, "", tagList, false, true, "Remote Player", playerRemoteId);
                    }
                }
                else if (strncmp(key, "villages", key_size) == 0) {
                    logger.msg(kLogInfo1, "Villages value:\n");
                    parseNbt("villages: ", cdata, cdata_size, tagList);
                    // todo - parse tagList? usually empty, unless player is in range of village; test that!
                }
                else if (strncmp(key, "mVillages", key_size) == 0) {
                    // todobig -- new for 0.13? what is it?
                    logger.msg(kLogInfo1, "mVillages value:\n");
                    ret = parseNbt("mVillages: ", cdata, cdata_size, tagList);
                    if (ret == 0) {
                        parseNbt_mVillages(tagList);
                    }
                }
                else if (strncmp(key, "game_flatworldlayers", key_size) == 0) {
                    // todobig -- what is it?
                    // example data (standard flat): 5b 37 2c 33 2c 33 2c 32 5d
                    logger.msg(kLogInfo1, "game_flatworldlayers value: (todo)\n");
                    // parseNbt("game_flatworldlayers: ", cdata, cdata_size, tagList);
                    // todo - parse tagList?
                }
                else if (strncmp(key, "idcounts", key_size) == 0) {
                    // todobig -- new for 0.13? what is it? is it a 4-byte int?
                    logger.msg(kLogInfo1, "idcounts value:\n");
                    parseNbt("idcounts: ", cdata, cdata_size, tagList);
                }
                else if (strncmp(key, "Nether", key_size) == 0) {
                    logger.msg(kLogInfo1, "Nether value:\n");
                    parseNbt("Nether: ", cdata, cdata_size, tagList);
                    // todo - parse tagList?  list of LimboEntities
                }
                else if (strncmp(key, "portals", key_size) == 0) {
                    logger.msg(kLogInfo1, "portals value:\n");
                    ret = parseNbt("portals: ", cdata, cdata_size, tagList);
                    if (ret == 0) {
                        parseNbt_portals(tagList);
                    }
                }
                else if (strncmp(key, "AutonomousEntities", key_size) == 0) {
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
                }
                else if (key_size == 9 || key_size == 10 || key_size == 13 || key_size == 14) {

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
                    }
                    else if (key_size == 10) {
                        // overworld chunk
                        chunkX = myParseInt32(key, 0);
                        chunkZ = myParseInt32(key, 4);
                        chunkDimId = kDimIdOverworld;
                        chunkType = myParseInt8(key, 8);
                        chunkTypeSub = myParseInt8(key, 9); // todonow - rename
                        dimName = "overworld";
                        chunkFormatVersion = 3; //todonow - get properly
                    }
                    else if (key_size == 13) {
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
                    }
                    else if (key_size == 14) {
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
                        chunkTypeSub, (int32_t)cdata_size);
                    chunkstr += tmpstring;
                    if (true) {
                        // show approximate image coordinates for chunk
                        double tix, tiy;
                        dimDataList[chunkDimId]->worldPointToImagePoint(chunkX * 16, chunkZ * 16, tix, tiy, false);
                        int32_t imageX = int32_t(tix);
                        int32_t imageZ = int32_t(tiy);
                        sprintf(tmpstring, " (image %d %d)", (int32_t)imageX, (int32_t)imageZ);
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
                                    dimName.c_str(), (int)(cdata[0]));
                            }
                            else {
                                logger.msg(kLogInfo1, "%s 0x76 chunk (world format version): v=%d\n",
                                    dimName.c_str(), (int)(cdata[0]));
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
                        }
                        else {
                            if (cdata_size != 6145 && cdata_size != 10241) {
                                logger.msg(kLogInfo1, "WARNING: UNKNOWN cdata_size=%d of 0x2f chunk\n",
                                    (int)cdata_size);
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
                            (int32_t)key_size, chunkType, (int32_t)cdata_size);
                        printKeyValue(key, key_size, cdata, cdata_size, true);

                        if (false) {
                            if (cdata_size > 10) {
                                parseNbt("UNK: ", cdata, cdata_size, tagList);
                            }
                        }
                        break;
                    }
                }
                else {
                    logger.msg(kLogInfo1, "WARNING: Unknown chunk - key_size=%d cdata_size=%d\n", (int32_t)key_size,
                        (int32_t)cdata_size);
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

        int32_t doOutput_Tile_image(const std::string& fn) {
            if (fn.size() <= 0) {
                return -1;
            }

            std::string dirOut = mydirname(control.fnOutputBase) + "/tiles";
            local_mkdir(dirOut.c_str());

            slogger.msg(kLogInfo1, "Creating tiles for %s...\n", mybasename(fn).c_str());
            PngTiler pngTiler(fn, control.tileWidth, control.tileHeight, dirOut);
            if (pngTiler.doTile() == 0) {
                // all is good
            }
            else {
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


        std::string makeTileURL(const std::string& fn) {
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
            }
            else {
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
            FILE* fp = fopen(control.fnJs.c_str(), "w");
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
                char* p = strchr(timebuf, '\n');
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
                    }
                    else {
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
                    for (const auto& it : dimDataList[did]->blockToGeoJSONList) {
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
                    }
                    else {
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
                        for (const auto& itbv : blockInfoList[i].variantList) {
                            fprintf(fp, "'%d': { name: '%s', id: %d, blockdata: %d },\n", be32toh(itbv->color),
                                escapeString(itbv->name, "'").c_str(), i, itbv->blockdata
                            );
                        }
                    }
                    else {
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
                for (const auto& it : biomeInfoList) {
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
                for (const auto& it : imageFileMap) {
                    fprintf(fp, "'%d': '%s',\n", it.second, it.first.c_str());
                }
                fprintf(fp, "'-1': ''};\n");

                fclose(fp);

            }
            else {
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

            }
            else {
                // if same dir, don't copy files
            }

            return 0;
        }


        int32_t doOutput_colortest() {
            slogger.msg(kLogInfo1, "Do Output: html colortest\n");

            std::string fnOut = control.fnOutputBase + ".colortest.html";
            FILE* fp = fopen(fnOut.c_str(), "w");

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
                    for (const auto& itbv : blockInfoList[i].variantList) {
                        webColorList.push_back(std::unique_ptr<ColorInfo>
                            (new ColorInfo(itbv->name, be32toh(itbv->color))));
                    }
                }
                else {
                    if (blockInfoList[i].colorSetFlag) {
                        webColorList.push_back(std::unique_ptr<ColorInfo>
                            (new ColorInfo(blockInfoList[i].name,
                                be32toh(blockInfoList[i].color))));
                    }
                }
            }

            std::sort(webColorList.begin(), webColorList.end(), compareColorInfo);
            fprintf(fp, "<div class=\"section\">Block Colors</div>");
            for (const auto& it : webColorList) {
                fprintf(fp, "%s\n", it->toHtml().c_str());
            }


            webColorList.clear();
            for (const auto& it : biomeInfoList) {
                if (it.second->colorSetFlag) {
                    // webColorList.emplace_back(blockInfoList[i].name, (int32_t)be32toh(blockInfoList[i].color));
                    webColorList.push_back(
                        std::unique_ptr<ColorInfo>(new ColorInfo(it.second->name, be32toh(it.second->color))));
                }
            }

            std::sort(webColorList.begin(), webColorList.end(), compareColorInfo);
            fprintf(fp, "<div class=\"section\">Biome Colors</div>");
            for (const auto& it : webColorList) {
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
                if (!fpGeoJSON) {
                    slogger.msg(kLogInfo1, "ERROR: Failed to create GeoJSON output file (%s).\n", control.fnGeoJSON.c_str());
                    return -1;
                }

                // set params for gzip
                //gzsetparams(fpGeoJSON, Z_BEST_COMPRESSION, Z_DEFAULT_STRATEGY);

                // put the geojson preamble stuff
                if (!control.noForceGeoJSONFlag) {
                    gzprintf(fpGeoJSON, "var geojson =\n");
                }
                gzprintf(fpGeoJSON,
                    "{ \"type\": \"FeatureCollection\",\n"
                    // todo - correct way to specify this?
                    "\"crs\": { \"type\": \"name\", \"properties\": { \"name\": \"mcpe_viz-image\" } },\n"
                    "\"features\": [\n"
                );

                // put the list with correct commas (silly)
                int32_t i = listGeoJSON.size();
                for (const auto& it : listGeoJSON) {
                    gzputs(fpGeoJSON, it.c_str());
                    if (--i > 0) {
                        gzputc(fpGeoJSON, ',');
                    }
                    gzputc(fpGeoJSON, '\n');
                }

                // close out the geojson properly
                if (control.noForceGeoJSONFlag) {
                    gzprintf(fpGeoJSON, "] }\n");
                }
                else {
                    gzprintf(fpGeoJSON, "] };\n");
                }

                gzclose(fpGeoJSON);
#endif
            }
            else {

                // plain text file version

                FILE* fpGeoJSON = fopen(control.fnGeoJSON.c_str(), "w");
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
                int32_t i = int32_t(listGeoJSON.size());
                for (const auto& it : listGeoJSON) {
                    fputs(it.c_str(), fpGeoJSON);
                    if (--i > 0) {
                        fputc(',', fpGeoJSON);
                    }
                    fputc('\n', fpGeoJSON);
                }

                // close out the geojson properly
                if (control.noForceGeoJSONFlag) {
                    fprintf(fpGeoJSON, "] }\n");
                }
                else {
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

        void worldPointToImagePoint(int32_t dimId, double wx, double wz, double& ix, double& iy, bool geoJsonFlag) {
            // hack to avoid using wrong dim on pre-0.12 worlds
            if (dimId < 0) { dimId = 0; }

            return dimDataList[dimId]->worldPointToImagePoint(wx, wz, ix, iy, geoJsonFlag);
        }

    };

    std::unique_ptr<MinecraftWorld_LevelDB> world;


    void worldPointToImagePoint(int32_t dimId, double wx, double wz, double& ix, double& iy, bool geoJsonFlag) {
        return world->worldPointToImagePoint(dimId, wx, wz, ix, iy, geoJsonFlag);
    }

    void worldPointToGeoJSONPoint(int32_t dimId, double wx, double wz, double& ix, double& iy) {
        worldPointToImagePoint(dimId, wx, wz, ix, iy, true);
    }


    int32_t doParseConfigFile(const std::string& fn) {
        if (!file_exists(fn.c_str())) {
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

    int32_t init(int argc, char** argv) {

        int32_t ret;

        dirExec = mydirname(argv[0]);

        world = std::unique_ptr<MinecraftWorld_LevelDB>(new MinecraftWorld_LevelDB());

        slogger.setStdout(stderr);
        slogger.setStderr(stderr);

        ret = parse_args(argc, argv);
        if (ret != 0) {
            mcpe_viz::print_usage();
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



int main(int argc, char** argv) {
    using namespace mcpe_viz;

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

    // print missing block information
    const auto& recorder = mcpe_viz::BlockRecorder::instance();
    if (!recorder.getUnknownBlockVariant().empty()) {
        for (auto& i : recorder.getUnknownBlockVariant()) {
            const auto& blockId = i.first.first;
            const auto& blockData = i.first.second;
            const auto& blockName = i.second;
            mcpe_viz::slogger.msg(kLogInfo1,
                "WARNING: Did not find block variant for block (id=%d (0x%x) '%s') with blockdata=%d (0x%x) MSG3\n",
                blockId, blockId,
                blockName.c_str(), blockData,
                blockData
            );
        }

    }

    std::cout << "Done.\n";

    return 0;
}
