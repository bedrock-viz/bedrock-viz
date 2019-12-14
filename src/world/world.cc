#include "world.h"

#include <ctime>

#include <leveldb/filter_policy.h>
#include <leveldb/cache.h>
#include <leveldb/env.h>

#include "../control.h"
#include "../nbt.h"
#include "../global.h"
#include "../minecraft/block_info.h"
#include "../minecraft/biome_info.h"
#include "../minecraft/conversion.h"

#include "../utils/fs.h"

#include "../asset.h"

namespace
{
    // suggestion from mcpe_sample_setup.cpp
    class NullLogger : public leveldb::Logger {
    public:
        void Logv(const char*, va_list) override
        {
        }
    };

    // note: this is an attempt to remove "bad" chunks as seen in "nyan.zip" world
    bool legalChunkPos(int32_t chunkX, int32_t chunkZ)
    {
        if ((uint32_t)chunkX == 0x80000000 && (uint32_t)chunkZ == 0x80000000) {
            return false;
        }
        return true;
    }
}

namespace mcpe_viz
{
    MinecraftWorld_LevelDB::MinecraftWorld_LevelDB()
    {
        db = nullptr;

        levelDbReadOptions.fill_cache = false;
        // suggestion from leveldb/mcpe_sample_setup.cpp
        levelDbReadOptions.decompress_allocator = new leveldb::DecompressAllocator();


        dbOptions = std::make_unique<leveldb::Options>();
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
            dimDataList[i] = std::make_unique<DimensionData_LevelDB>();
            dimDataList[i]->setDimId(i);
            dimDataList[i]->unsetChunkBoundsValid();
        }
        dimDataList[kDimIdOverworld]->setName("overworld");
        dimDataList[kDimIdNether]->setName("nether");
        dimDataList[kDimIdTheEnd]->setName("the-end");
    }

    int32_t MinecraftWorld_LevelDB::parseLevelFile(const std::string& fname)
    {
        FILE* fp = fopen(fname.c_str(), "rb");
        if (!fp) {
            log::error("Failed to open input file (fn={} error={} ({}))", fname, strerror(errno), errno);
            return -1;
        }

        int32_t fVersion;
        int32_t bufLen;
        fread(&fVersion, sizeof(int32_t), 1, fp);
        fread(&bufLen, sizeof(int32_t), 1, fp);

        log::info("parseLevelFile: name={} version={} len={}", fname, fVersion, bufLen);

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
                log::info("  Found World Spawn: x={} y={} z={}",
                    getWorldSpawnX(),
                    getWorldSpawnY(),
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

    int32_t MinecraftWorld_LevelDB::init()
    {
        int32_t ret = parseLevelFile(std::string(control.dirLeveldb + "/level.dat"));
        if (ret != 0) {
            log::error("Failed to parse level.dat file.  Exiting...");
            log::error("** Hint: --db must point to the dir which contains level.dat");
            return -1;
        }

        ret = parseLevelName(std::string(control.dirLeveldb + "/levelname.txt"));
        if (ret != 0) {
            log::warn("WARNING: Failed to parse levelname.txt file.");
            log::warn("** Hint: --db must point to the dir which contains levelname.txt");
        }

        // update dimension data
        for (int32_t i = 0; i < kDimIdCount; i++) {
            dimDataList[i]->setWorldInfo(getWorldName(), getWorldSpawnX(), getWorldSpawnZ(), getWorldSeed());
        }

        return 0;
    }

    int32_t MinecraftWorld_LevelDB::dbOpen(const std::string& dirDb)
    {
        // todobig - leveldb read-only? snapshot?
        // note: seems impossible, see <https://github.com/google/leveldb/issues/182>
        log::info("DB Open: dir={}", dirDb);
        leveldb::Status dstatus = leveldb::DB::Open(*dbOptions, std::string(dirDb + "/db"), &db);
        log::info("DB Open Status: {} (block_size={} bloom_filter_bits={})", dstatus.ToString(), control.leveldbBlockSize, control.leveldbFilter);
        fflush(stderr);
        if (!dstatus.ok()) {
            log::error("LevelDB operation returned status={}", dstatus.ToString());
            exit(-2);
        }
        return 0;
    }


    int32_t MinecraftWorld_LevelDB::calcChunkBounds()
    {
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

        log::info("Scan keys to get world boundaries");
        int32_t recordCt = 0;

        // todobig - is there a faster way to enumerate the keys?
        leveldb::Iterator* iter = db->NewIterator(levelDbReadOptions);
        leveldb::Slice skey;
        int32_t key_size;
        const char* key;
        for (iter->SeekToFirst(); iter->Valid(); iter->Next()) {
            skey = iter->key();
            key_size = int32_t(skey.size());
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
            log::warn("LevelDB operation returned status={}", iter->status().ToString());
        }
        delete iter;

        // mark bounds valid
        for (int32_t i = 0; i < kDimIdCount; i++) {
            dimDataList[i]->setChunkBoundsValid();
            dimDataList[i]->reportChunkBounds();
        }

        log::info("  {} records", recordCt);
        totalRecordCt = recordCt;

        return 0;
    }

    int32_t MinecraftWorld_LevelDB::dbParse()
    {
        char tmpstring[256];

        int32_t chunkX = -1, chunkZ = -1, chunkDimId = -1, chunkType = -1, chunkTypeSub = -1;
        int32_t chunkFormatVersion = 2; //todonow - get properly


        // we make sure that we know the chunk bounds before we start so that we can translate world coords to image coords
        calcChunkBounds();

        // report hide and force lists
        {
            log::info("Active 'hide-top', 'force-top', and 'geojson-block':");
            int32_t itemCt = 0;
            int32_t blockId;
            for (int32_t dimId = 0; dimId < kDimIdCount; dimId++) {
                dimDataList[dimId]->updateFastLists();
                for (const auto& iter : dimDataList[dimId]->blockHideList) {
                    blockId = iter;
                    log::info("  'hide-top' block: {} - {} (dimId={} blockId={} (0x{:x}))",
                        dimDataList[dimId]->getName(),
                        blockInfoList[blockId].name,
                        dimId, blockId, blockId);
                    itemCt++;
                }

                for (const auto& iter : dimDataList[dimId]->blockForceTopList) {
                    blockId = iter;
                    log::info("  'force-top' block: {} - {} (dimId={} blockId={} (0x{:x}))",
                        dimDataList[dimId]->getName(),
                        blockInfoList[blockId].name,
                        dimId, blockId, blockId);
                    itemCt++;
                }

                for (const auto& iter : dimDataList[dimId]->blockToGeoJSONList) {
                    blockId = iter;
                    log::info("  'genjson' block: {} - {} (dimId={} blockId={} (0x{:x}))",
                        dimDataList[dimId]->getName(),
                        blockInfoList[blockId].name,
                        dimId, blockId, blockId);
                    itemCt++;
                }
            }
            if (itemCt == 0) {
                log::info("  None");
            }
        }

        log::info("Parse all leveldb records");

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
                log::info("  Processing records: {} / {} ({:.1f}%)", recordCt, totalRecordCt, pct * 100.0);
            }

            // we look at the key to determine what we have, some records have text keys

            if (strncmp(key, "BiomeData", key_size) == 0) {
                // 0x61 +"BiomeData" -- snow accum? -- overworld only?
                log::trace("BiomeData value:");
                parseNbt("BiomeData: ", cdata, int32_t(cdata_size), tagList);
                // todo - parse tagList? snow accumulation amounts
            }
            else if (strncmp(key, "Overworld", key_size) == 0) {
                log::trace("Overworld value:");
                parseNbt("Overworld: ", cdata, int32_t(cdata_size), tagList);
                // todo - parse tagList? a list of "LimboEntities"
            }
            else if (strncmp(key, "~local_player", key_size) == 0) {
                log::trace("Local Player value:");
                ret = parseNbt("Local Player: ", cdata, int32_t(cdata_size), tagList);
                if (ret == 0) {
                    parseNbt_entity(-1, "", tagList, true, false, "Local Player", "");
                }
            }
            else if ((key_size >= 7) && (strncmp(key, "player_", 7) == 0)) {
                // note: key contains player id (e.g. "player_-1234")
                std::string playerRemoteId = std::string(&key[strlen("player_")], key_size - strlen("player_"));

                log::trace("Remote Player (id={}) value:", playerRemoteId);

                ret = parseNbt("Remote Player: ", cdata, int32_t(cdata_size), tagList);
                if (ret == 0) {
                    parseNbt_entity(-1, "", tagList, false, true, "Remote Player", playerRemoteId);
                }
            }
            else if (strncmp(key, "villages", key_size) == 0) {
                logger.msg(kLogInfo1, "Villages value:\n");
                parseNbt("villages: ", cdata, int32_t(cdata_size), tagList);
                // todo - parse tagList? usually empty, unless player is in range of village; test that!
            }
            else if (strncmp(key, "mVillages", key_size) == 0) {
                // todobig -- new for 0.13? what is it?
                logger.msg(kLogInfo1, "mVillages value:\n");
                ret = parseNbt("mVillages: ", cdata, int32_t(cdata_size), tagList);
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
                parseNbt("idcounts: ", cdata, int32_t(cdata_size), tagList);
            }
            else if (strncmp(key, "Nether", key_size) == 0) {
                logger.msg(kLogInfo1, "Nether value:\n");
                parseNbt("Nether: ", cdata, int32_t(cdata_size), tagList);
                // todo - parse tagList?  list of LimboEntities
            }
            else if (strncmp(key, "portals", key_size) == 0) {
                logger.msg(kLogInfo1, "portals value:\n");
                ret = parseNbt("portals: ", cdata, int32_t(cdata_size), tagList);
                if (ret == 0) {
                    parseNbt_portals(tagList);
                }
            }
            else if (strncmp(key, "AutonomousEntities", key_size) == 0) {
                logger.msg(kLogInfo1, "AutonomousEntities value:\n");
                ret = parseNbt("AutonomousEntities: ", cdata, int32_t(cdata_size), tagList);
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
                ret = parseNbt("Dimension: ", cdata, int32_t(cdata_size), tagList);
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
                        log::warn("UNKNOWN -- Found new chunkDimId=0x{:x} -- we are not prepared for that -- skipping chunk", chunkDimId);
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
                        log::warn("UNKNOWN -- Found new chunkDimId=0x{:x} -- we are not prepared for that -- skipping chunk", chunkDimId);
                        continue;
                    }
                }

                // we check for corrupt chunks
                if (!legalChunkPos(chunkX, chunkZ)) {
                    log::warn("Found a chunk with invalid chunk coordinates cx={} cz={}", chunkX, chunkZ);
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
                    ret = parseNbt("0x31-te: ", cdata, int32_t(cdata_size), tagList);
                    if (ret == 0) {
                        parseNbt_tileEntity(chunkDimId, dimName + "-", tagList);
                    }
                    break;

                case 0x32:
                    // "Entity"
                    // entity record (e.g. a mob)
                    logger.msg(kLogInfo1, "%s 0x32 chunk (entity data):\n", dimName.c_str());
                    ret = parseNbt("0x32-e: ", cdata, int32_t(cdata_size), tagList);
                    if (ret == 0) {
                        parseNbt_entity(chunkDimId, dimName + "-", tagList, false, false, "", "");
                    }
                    break;

                case 0x33:
                    // "PendingTicks"
                    // todo - this appears to be info on blocks that can move: water + lava + fire + sand + gravel
                    logger.msg(kLogInfo1, "%s 0x33 chunk (tick-list):\n", dimName.c_str());
                    parseNbt("0x33-tick: ", cdata, int32_t(cdata_size), tagList);
                    // todo - parse tagList?
                    // todobig - could show location of active fires
                    break;

                case 0x34:
                    // "BlockExtraData"
                    logger.msg(kLogInfo1, "%s 0x34 chunk (TODO - MYSTERY RECORD - BlockExtraData)\n",
                        dimName.c_str());
                    if (control.verboseFlag) {
                        printKeyValue(key, int32_t(key_size), cdata, int32_t(cdata_size), false);
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
                        printKeyValue(key, int32_t(key_size), cdata, int32_t(cdata_size), false);
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
                    log::debug("{} 0x36 chunk (TODO - MYSTERY RECORD - TBD)", dimName);
                    if (control.verboseFlag) {
                        printKeyValue(key, int32_t(key_size), cdata, int32_t(cdata_size), false);
                    }
                    // todo - what is this?
                    // appears to be a single 4-byte integer?
                    break;

                case 0x39:
                    // new for v1.2?
                    log::debug("{} 0x39 chunk (TODO - MYSTERY RECORD - TBD)", dimName);
                    if (control.verboseFlag) {
                        printKeyValue(key, int32_t(key_size), cdata, int32_t(cdata_size), false);
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
                    dimDataList[chunkDimId]->addChunkColumnData(3, chunkX, chunkZ, cdata, int32_t(cdata_size));
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
                    log::debug("{} unknown chunk - key_size={} type=0x{:x} length={}", dimName, key_size, chunkType, cdata_size);
                    printKeyValue(key, int32_t(key_size), cdata, int32_t(cdata_size), true);
                    break;
                }
            }
            else {
                log::debug("Unknown chunk - key_size={} cdata_size={}", key_size, cdata_size);
                printKeyValue(key, int32_t(key_size), cdata, int32_t(cdata_size), true);
            }
        }
        log::info("Read {} records", recordCt);
        log::info("Status: {}", iter->status().ToString());

        if (!iter->status().ok()) {
            log::warn("LevelDB operation returned status={}", iter->status().ToString());
        }
        delete iter;

        return 0;
    }

    int32_t MinecraftWorld_LevelDB::doOutput_Tile_image(const std::string& fn)
    {
        if (fn.size() <= 0) {
            return -1;
        }

        std::string dirOut = (control.outputDir / "tiles").generic_string();
        local_mkdir(dirOut);
        log::info("Creating tiles for {}...", mybasename(fn));
        PngTiler pngTiler(fn, control.tileWidth, control.tileHeight, dirOut);
        if (pngTiler.doTile() == 0) {
            // all is good
        }
        else {
            // todobig - error
        }

        return 0;
    }


    int32_t MinecraftWorld_LevelDB::doOutput_Tile()
    {
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

    std::string MinecraftWorld_LevelDB::makeTileURL(const std::string& fn)
    {
        std::string ret = mybasename(fn);
        if (!control.doTiles) {
            return "images/" + ret;
        }
        if (ret.size() > 1) {
            return "tiles/" + ret + ".{y}.{x}.png";
        }
        return "";
    }

    int32_t MinecraftWorld_LevelDB::doOutput_html()
    {
        using namespace bedrock_viz;
        char tmpstring[1025];

        log::info("Do Output: html viewer");

        //sprintf(tmpstring, "%s/mcpe_viz.html.template", dirExec.c_str());
        const std::string fnHtmlSrc = static_path("mcpe_viz.html.template").generic_string();

        //sprintf(tmpstring, "%s/mcpe_viz.js", dirExec.c_str());
        const std::string fnJsSrc = static_path("mcpe_viz.js").generic_string();

        //sprintf(tmpstring, "%s/mcpe_viz.css", dirExec.c_str());
        const std::string fnCssSrc = static_path("mcpe_viz.css").generic_string();

        // create html file -- need to substitute one variable (extra js file)
        StringReplacementList replaceStrings;

        if (!control.noForceGeoJSONFlag) {
            // we do include the geojson file
            replaceStrings.push_back(std::make_pair(std::string("%JSFILE%"),
                "<script src=\"" +
                std::string(mybasename(control.fnGeoJSON().generic_string().c_str())) +
                "\"></script>"
            )
            );
        }
        copyFileWithStringReplacement(fnHtmlSrc, control.fnHtml().generic_string(), replaceStrings);

        // create javascript file w/ filenames etc
        FILE* fp = fopen(control.fnJs().generic_string().c_str(), "w");
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
                mybasename(control.fnGeoJSON().generic_string()).c_str(),
                control.doTiles ? "true" : "false", control.tileWidth,
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
                int32_t llen = int32_t(dimDataList[did]->blockToGeoJSONList.size());
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
                        fprintf(fp, "'%d': { name: '%s', id: %d, blockdata: %d },\n", local_be32toh(itbv->color),
                            escapeString(itbv->name, "'").c_str(), i, itbv->blockdata
                        );
                    }
                }
                else {
                    if (blockInfoList[i].colorSetFlag) {
                        fprintf(fp, "'%d': { name: '%s', id: %d, blockdata: %d },\n",
                            local_be32toh(blockInfoList[i].color), escapeString(blockInfoList[i].name, "'").c_str(),
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
                    fprintf(fp, "'%d': { name: '%s', id: %d },\n", local_be32toh(it.second->color),
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
            log::error("Failed to open javascript output file (fn={} error={} ({}))",
                control.fnJs().generic_string(), strerror(errno), errno);
        }

        // copy helper files to destination directory
        std::string dirDest = control.outputDir.generic_string();

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
            //copyDirToDir(dirExec + "/js", dirJs, false);
            copyDirToDir(static_path("js").generic_string(), dirJs, false);

            // copy images
            // todobig - could save a good amount of time per run if we detect if dir has already been copied
            // todobig - could save a bit of time by only copying images used (use imageFileMap)
            log::info("Copying icon images");
            std::string dirImages = dirDest + "/images";
            local_mkdir(dirImages);
            //copyDirToDir(dirExec + "/images", dirImages, true);
            copyDirToDir(static_path("images").generic_string(), dirImages, true);

        }
        else {
            // if same dir, don't copy files
        }

        return 0;
    }

    int32_t MinecraftWorld_LevelDB::doOutput_GeoJSON()
    {
        // plain text file version

        FILE* fpGeoJSON = fopen(control.fnGeoJSON().generic_string().c_str(), "w");
        if (!fpGeoJSON) {
            log::error("Failed to create GeoJSON output file ({} error={} ({}))",
                control.fnGeoJSON().generic_string(), strerror(errno), errno);
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
        return 0;
    }

    int32_t MinecraftWorld_LevelDB::doOutput()
    {
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
                    log::info("Detected large images and 'auto-tile' is enabled, enabling tiles");
                    control.doTiles = true;
                }
            }

            doOutput_Tile();
            doOutput_html();
            doOutput_GeoJSON();
        }

        return 0;
    }

    std::unique_ptr<MinecraftWorld_LevelDB> world;
}