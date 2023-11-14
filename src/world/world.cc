#include "world/world.h"
#include "control.h"
#include "nbt.h"
#include "global.h"
#include "minecraft/conversion.h"
#include "utils/fs.h"
#include "asset.h"
#include "minecraft/v2/biome.h"
#include "minecraft/v2/block.h"

#include <ctime>
#include <leveldb/filter_policy.h>
#include <leveldb/cache.h>
#include <leveldb/env.h>

#include <iostream>


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

    bool limitedChunkPos(int32_t did, int32_t chunkX, int32_t chunkZ)
    {
        if (!legalChunkPos(chunkX, chunkZ)) {
            return false;
        }
        if (mcpe_viz::control.limitX[did] && chunkX < mcpe_viz::control.limitXMin[did]) {
            return false;
        }
        if (mcpe_viz::control.limitX[did] && chunkX > mcpe_viz::control.limitXMax[did]) {
            return false;
        }
        if (mcpe_viz::control.limitZ[did] && chunkZ < mcpe_viz::control.limitZMin[did]) {
            return false;
        }
        if (mcpe_viz::control.limitZ[did] && chunkZ > mcpe_viz::control.limitZMax[did]) {
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

        //create a 40 mb cache (we use this on ~1gb devices)
        dbOptions->block_cache = leveldb::NewLRUCache(40 * 1024 * 1024);

        //create a 4mb write buffer, to improve compression and touch the disk less
        dbOptions->write_buffer_size = 4 * 1024 * 1024;
        dbOptions->info_log = new NullLogger();
        dbOptions->compression = leveldb::kZlibRawCompression;

        for (int32_t i = 0; i < kDimIdCount; i++) {
            dimDataList.push_back(std::make_unique<DimensionData_LevelDB>());
            dimDataList[i]->setDimId(i);
            dimDataList[i]->unsetChunkBoundsValid();
            dimDataList[i]->setName(kDimIdNames[i]);
        }
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
        leveldb::Status openstatus = leveldb::DB::Open(*dbOptions, std::string(dirDb + "/db"), &db);
        log::info("DB Open Status: {} (block_size={} bloom_filter_bits={})", openstatus.ToString(), control.leveldbBlockSize, control.leveldbFilter);
        fflush(stderr);
        if (!openstatus.ok()) {
            log::error("LevelDB operation returned status={}", openstatus.ToString());
            
            if (control.tryDbRepair)
            {
                log::info("Attempting leveldb repair due to failed open");
                leveldb::Options options_;
                leveldb::Status repairstatus = leveldb::RepairDB(std::string(dirDb + "/db"), options_);

                if (repairstatus.ok())
                    log::info("LevelDB repair completed");
                else {
                    log::error("LevelDB repair failed");
                    exit(-2);
                }
            }

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
                    if (limitedChunkPos(0, chunkX, chunkZ)) {
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
                    if (limitedChunkPos(0, chunkX, chunkZ)) {
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
                    if (limitedChunkPos(chunkDimId, chunkX, chunkZ)) {
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
                    if (limitedChunkPos(chunkDimId, chunkX, chunkZ)) {
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
                        Block::queryName(blockId),
                        dimId, blockId, blockId);
                    itemCt++;
                }

                for (const auto& iter : dimDataList[dimId]->blockForceTopList) {
                    blockId = iter;
                    log::info("  'force-top' block: {} - {} (dimId={} blockId={} (0x{:x}))",
                        dimDataList[dimId]->getName(),
                              Block::queryName(blockId),
                              dimId, blockId, blockId);
                    itemCt++;
                }

                for (const auto& iter : dimDataList[dimId]->blockToGeoJSONList) {
                    blockId = iter;
                    log::info("  'geojson' block: {} - {} (dimId={} blockId={} (0x{:x}))",
                        dimDataList[dimId]->getName(),
                              Block::queryName(blockId),
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

        MyNbtPlayerMap playerMap;
        std::vector<std::string> villages;

        std::vector<uint64_t> actorIds;

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
                    std::shared_ptr<PlayerInfo> player_info(new PlayerInfo());
                    parseNbt_entity(-1, "", tagList, true, false, "Local Player", "", player_info);
                    playerMap[player_info->uniqueId] = player_info;
                }
            }
            else if ((key_size >= 7) && (strncmp(key, "player_", 7) == 0)) {
                // note: key contains player id (e.g. "player_-1234")
                std::string playerRemoteId = std::string(&key[strlen("player_")], key_size - strlen("player_"));

                log::trace("Remote Player (id={}) value:", playerRemoteId);

                ret = parseNbt("Remote Player: ", cdata, int32_t(cdata_size), tagList);
                if (ret == 0) {
                    std::shared_ptr<PlayerInfo> player_info(new PlayerInfo());
                    parseNbt_entity(-1, "", tagList, false, true, "Remote Player", playerRemoteId, player_info);
                    playerMap[player_info->uniqueId] = player_info;
                }
            }
            else if (strncmp(key, "villages", key_size) == 0) {
                log::trace("Villages value:");
                parseNbt("villages: ", cdata, int32_t(cdata_size), tagList);
                // todo - parse tagList? usually empty, unless player is in range of village; test that!
            }
            else if (strncmp(key, "mVillages", key_size) == 0) {
                // todobig -- new for 0.13? what is it?
                log::trace("mVillages value:");
                ret = parseNbt("mVillages: ", cdata, int32_t(cdata_size), tagList);
                if (ret == 0) {
                    parseNbt_mVillages(tagList);
                }
            }
            else if (strncmp(key, "VILLAGE_", 8) == 0) {
                // Extract the unique 36 VID from the key in the form off:
                // VILLAGE_07315855-d0e6-4fac-8b20-0c07cfad3d29_POI
                // VILLAGE_Overworld_4faa34eb-af26-4a63-9251-851069a53573_DWELLERS\001\330b\a\006
                std::string keyStr(key, key_size);
                size_t last = keyStr.rfind("_");
                size_t secondLast = std::string::npos;
                if (last != std::string::npos) {
                    secondLast = keyStr.rfind("_", last - 1);
                }
                if (last - secondLast == 37) {
                    char vid[37];
                    strncpy(vid, keyStr.data() + secondLast + 1, 36);
                    vid[36] = '\0';
                    if (strncmp(key + last + 1, "INFO", 4) == 0) {
                        villages.push_back(vid);
                    }
                }
            }
            else if (strncmp(key, "game_flatworldlayers", key_size) == 0) {
                // todobig -- what is it?
                // example data (standard flat): 5b 37 2c 33 2c 33 2c 32 5d
                log::trace("game_flatworldlayers value: (todo)");
                // parseNbt("game_flatworldlayers: ", cdata, cdata_size, tagList);
                // todo - parse tagList?
            }
            else if (strncmp(key, "idcounts", key_size) == 0) {
                // todobig -- new for 0.13? what is it? is it a 4-byte int?
                log::trace("idcounts value:");
                parseNbt("idcounts: ", cdata, int32_t(cdata_size), tagList);
            }
            else if (strncmp(key, "Nether", key_size) == 0) {
                log::trace("Nether value:");
                parseNbt("Nether: ", cdata, int32_t(cdata_size), tagList);
                // todo - parse tagList?  list of LimboEntities
            }
            else if (strncmp(key, "portals", key_size) == 0) {
                log::trace("portals value:");
                ret = parseNbt("portals: ", cdata, int32_t(cdata_size), tagList);
                if (ret == 0) {
                    parseNbt_portals(tagList);
                }
            }
            else if (strncmp(key, "AutonomousEntities", key_size) == 0) {
                log::trace("AutonomousEntities value:");
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
                log::debug("Dimension chunk -- key: ({}) value:", keyString);
                ret = parseNbt("Dimension: ", cdata, int32_t(cdata_size), tagList);
                // todostopper - what to do with this info?
                //          if ( ret == 0 ) {
                //            parseNbt_portals(tagList);
                //          }
            }
            else if (strncmp(key, "digp", 4) == 0) {
                // Actor Digest
                if (key_size == 12) {
                    // overworld chunk
                    chunkX = myParseInt32(key, 4);
                    chunkZ = myParseInt32(key, 8);
                    chunkDimId = kDimIdOverworld;
                } else if (key_size == 16) {
                    // non-overworld chunk
                    chunkX = myParseInt32(key, 4);
                    chunkZ = myParseInt32(key, 8);
                    chunkDimId = myParseInt32(key, 12);
                }

                // we check for corrupt chunks
                if (!legalChunkPos(chunkX, chunkZ)) {
                    log::warn("Found a chunk with invalid chunk coordinates cx={} cz={}", chunkX, chunkZ);
                    continue;
                }

                if (!limitedChunkPos(chunkDimId, chunkX, chunkZ)) {
                    continue;
                }

                for (uint32_t i = 0; i < cdata_size; i += 8) {
                    actorIds.push_back(*(uint64_t *)(cdata + i));
                }
            }
            else if (strncmp(key, "actorprefix", 11) == 0) {
                continue;
            }
	    else if(strncmp(key, "LevelChunkMetaDataDictionary", 28) == 0) {
                log::debug("Found LevelChunkMetaDataDictionary");
                if (control.verboseFlag) {
                    printKeyValue(key, int32_t(key_size), cdata, int32_t(cdata_size), false);
                }
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

                if (!limitedChunkPos(chunkDimId, chunkX, chunkZ)) {
                    continue;
                }

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
                log::trace("{}", chunkstr);

                // see what kind of chunk we have
                // tommo posted useful info about the various record types here (around 0.17 beta):
                //   https://www.reddit.com/r/MCPE/comments/5cw2tm/level_format_changes_in_mcpe_0171_100/
                switch (chunkType) {
                case 0x30:
                    // "LegacyTerrain"
                    log::trace("{} 0x30 chunk (LegacyTerrain):", dimName);
                    // chunk block data
                    // we do the parsing in the destination object to save memcpy's
                    // todonow - would be better to get the version # from the proper chunk record (0x76)
                    dimDataList[chunkDimId]->addChunk(2, chunkX, 0, chunkZ, cdata, cdata_size);
                    break;

                case 0x31:
                    // "BlockEntity"
                    // tile entity record (e.g. a chest)
                    log::trace("{} 0x31 chunk (BlockEntity):", dimName);
                    ret = parseNbt("0x31-te: ", cdata, int32_t(cdata_size), tagList);
                    if (ret == 0) {
                        parseNbt_tileEntity(chunkDimId, dimName + "-", tagList);
                    }
                    break;

                case 0x32:
                    // "Entity"
                    // entity record (e.g. a mob)
                    log::trace("{} 0x32 chunk (Entity):", dimName);
                    ret = parseNbt("0x32-e: ", cdata, int32_t(cdata_size), tagList);
                    if (ret == 0) {
                        parseNbt_entity(chunkDimId, dimName + "-", tagList, false, false, "", "");
                    }
                    break;

                case 0x33:
                    // "PendingTicks"
                    // todo - this appears to be info on blocks that can move: water + lava + fire + sand + gravel
                    // todo - new nether has slowed things down quite a bit
                    log::trace("{} 0x33 chunk (PendingTicks):", dimName);
                    //parseNbt("0x33-tick: ", cdata, int32_t(cdata_size), tagList);
                    // todo - parse tagList?
                    // todobig - could show location of active fires
                    break;

                case 0x34:
                    // "LegacyBlockExtraData"
                    log::trace("{} 0x34 chunk (LegacyBlockExtraData)", dimName);
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
                    log::trace("{} 0x35 chunk (BiomeState)", dimName);
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
		    // "FinalizedState"
                    log::trace("{} 0x36 chunk (FinalizedState)", dimName);
                    if (control.verboseFlag) {
                        printKeyValue(key, int32_t(key_size), cdata, int32_t(cdata_size), false);
                    }
		    // From https://cran.r-project.org/web/packages/rbedrock/rbedrock.pdf
		    // 0 NeedsInstaticking Chunk needs to be ticked
                    // 1 NeedsPopulation Chunk needs to be populated with mobs
                    // 2 Done Chunk generation is fully complete
                    break;
                case 0x37:
                    log::trace("{} 0x37 chunk (ConversionData)", dimName);
		    break;
                case 0x38:
                    log::trace("{} 0x38 chunk (BorderBlocks)", dimName);
		    break;
                case 0x39:
		    // "HardCodedSpawnAreas"
                    // Bounding boxes for structure spawns stored in binary format
                    log::trace("{} 0x39 chunk (HardCodedSpawnAreas)", dimName);
		    // Last byte of the cdata maps to the following
		    // 1: "Fortress", 3: "Monument", 5: "Villager Outpost", 2: "Witch Hut"
		    switch (*(cdata + cdata_size - 1)) {
			    case 1:
                                log::info("{} Found Fortress", dimName);
				break;
			    case 2:
                                log::info("{} Found Witch Hut", dimName);
				break;
			    case 3:
                                log::info("{} Found Ocean Monument", dimName);
				break;
			    case 5:
                                log::info("{} Found Pillager Outpost", dimName);
				break;
			    default:
				log::error("{} Found unknown structure", dimName);
		    }
                    if (control.verboseFlag) {
                        printKeyValue(key, int32_t(key_size), cdata, int32_t(cdata_size), false);
                    }
                    break;
                case 0x3a:
                    // Tick counters - not used
                    log::trace("{} 0x3a chunk (RandomTicks)", dimName);
                    break;
                case 0x3b:
                    // Appears to be a list of checksums for chunk data. Upcoming in 1.16
		    // Appears to be no longer written as of 1.18 -- https://minecraft.fandom.com/wiki/Bedrock_Edition_level_format#Chunk_key_format
                    log::trace("{} 0x3b chunk (CheckSums)", dimName);
                    if (control.verboseFlag) {
                        printKeyValue(key, int32_t(key_size), cdata, int32_t(cdata_size), false);
                    }
                    // todo - what is this?
                    break;
                case 0x3c:
                    log::trace("{} 0x3c chunk (GenerationSeed)", dimName);
		    break;
                case 0x3d:
                    log::trace("{} 0x3d chunk (GeneratedPreCavesAndCliffsBlending)", dimName);
		    break;
                case 0x3e:
                    log::trace("{} 0x3c chunk (BlendingBiomeHeight)", dimName);
		    break;
		case 0x3f:
		    // MetaDataHash
		    log::trace("{} 0x3f chunk (MetaDataHash)", dimName);
		    // cdata contains a key to the NBT data that's in the global LevelChunkMetaDataDictionary
                    if (control.verboseFlag) {
                        printKeyValue(key, int32_t(key_size), cdata, int32_t(cdata_size), false);
                    }
		case 0x40:
                    log::trace("{} 0x40 chunk (BlendingData)", dimName);
                    if (control.verboseFlag) {
                        printKeyValue(key, int32_t(key_size), cdata, int32_t(cdata_size), false);
                    }
		    break;
		case 0x41:
                    log::trace("{} 0x41 chunk (ActorDigestVersion)", dimName);
                    if (control.verboseFlag) {
                        printKeyValue(key, int32_t(key_size), cdata, int32_t(cdata_size), false);
                    }
		    break;
                case 0x2C:
                case 0x76:
                    // "Version"
                {
                    // this record is not very interesting, we usually hide it
                    // note: it would be interesting if this is not == 2 (as of MCPE 0.12.x it is always 2)
                    
                    // chunk versions have changed many times since this was originally included. 
                    // it seems unnecessary to keep track of this for anything other than trace information
                    log::trace("{} 0x{:x} chunk (world format version): v={}"
                        ,dimName
                        ,chunkType
                        ,int(cdata[0]));
                    /*
                    if (control.verboseFlag || ((cdata[0] != 2) && (cdata[0] != 3) && (cdata[0] != 9))) {
                        if (cdata[0] != 2 && cdata[0] != 9) {
                            log::debug("UNKNOWN CHUNK VERSION!  {} 0x76 chunk (world format version): v={}",
                                dimName, int(cdata[0]));
                        }
                        else {
                            log::debug("{} 0x76 chunk (world format version): v={}",
                                dimName, int(cdata[0]));
                        }
                    }
                    */
                }
                break;

                case 0x2f:
                    // "SubchunkPrefix"
                    // chunk block data - 10241 bytes
                    // todonow -- but have also seen 6145 on v1.1?
                    // we do the parsing in the destination object to save memcpy's
                    // todonow - would be better to get the version # from the proper chunk record (0x2C)
                {
                    int32_t chunkY = chunkTypeSub;
                    // check the first byte to see if anything interesting is in it
                    if (cdata[0] != 0) {
                        //logger.msg(kLogInfo1, "WARNING: UNKNOWN Byte 0 of 0x2f chunk: b0=[%d 0x%02x]\n", (int)cdata[0], (int)cdata[0]);
                        dimDataList[chunkDimId]->addChunk(7, chunkX, chunkY, chunkZ, cdata, cdata_size);
                    }
                    else {
                        if (cdata_size != 6145 && cdata_size != 10241) {
                            log::warn("UNKNOWN cdata_size={} of 0x2f chunk", cdata_size);
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
                    log::trace("{} 0x2d chunk (Data2D)", dimName);
                    dimDataList[chunkDimId]->addChunkColumnData(3, chunkX, chunkZ, cdata, int32_t(cdata_size));
                }
                break;

                case 0x2b:
		    // "Data3D"
                    // 1.18 3D biome data
                    // 512 bytes -> heightmap
                    // Paletted biome data per subcunk (16x16x16) from bottom up
                    //   1 byte header - 0xff means non-existant
                    //     bit 0 - always 1?
                    //     bits 7..1: bits per value for array (can be 0)
                    //   n byte array (n = 4096/8 * bits per value)
                    //   int32 palette length
                    //   palette entries (int32)
                {
                    log::trace("{} 0x2b chunk (Data3D)", dimName);
                    dimDataList[chunkDimId]->addChunkColumnData(4, chunkX, chunkZ, cdata, int32_t(cdata_size));
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

        for (auto actorId : actorIds) {
            std::string data;
            char key[19] = "actorprefix";
            memcpy(key+11, &actorId, 8);
            MyNbtTagList actor_tags;
            db->Get(levelDbReadOptions, leveldb::Slice(key, 19), &data);
            ret = parseNbt("actor: ", data.data(), data.size(), actor_tags);

            if (ret == 0) {
                parseNbt_entity(kDimIdOverworld, "actor", actor_tags, false, false, "", "");
            }
        }
        actorIds.clear();

        for (auto vid : villages) {
            std::string data;
            MyNbtTagList info_tags, player_tags, dweller_tags, poi_tags;
            if (db->Get(levelDbReadOptions, ("VILLAGE_" + vid + "_INFO"), &data).ok() ||
                db->Get(levelDbReadOptions, ("VILLAGE_Overworld_" + vid + "_INFO"), &data).ok() ) {
                ret = parseNbt("village_info: ", data.data(), data.size(), info_tags);
                if (ret != 0) continue;
            }
            if (db->Get(levelDbReadOptions, ("VILLAGE_" + vid + "_PLAYERS"), &data).ok() ||
                db->Get(levelDbReadOptions, ("VILLAGE_Overworld_" + vid + "_PLAYERS"), &data).ok()) {
                ret = parseNbt("village_players: ", data.data(), data.size(), player_tags);
                if (ret != 0) continue;
            }
            if (db->Get(levelDbReadOptions, ("VILLAGE_" + vid + "_DWELLERS"), &data).ok() ||
                db->Get(levelDbReadOptions, ("VILLAGE_Overworld_" + vid + "_DWELLERS"), &data).ok()) {
                ret = parseNbt("village_dwellers: ", data.data(), data.size(), dweller_tags);
                if (ret != 0) continue;
            }
            if (db->Get(levelDbReadOptions, ("VILLAGE_" + vid + "_POI"), &data).ok() ||
                db->Get(levelDbReadOptions, ("VILLAGE_Overworld_" + vid + "_POI"), &data).ok()) {
                ret = parseNbt("village_poi: ", data.data(), data.size(), poi_tags);
                if (ret != 0) continue;
            }

            parseNbt_village(info_tags, player_tags, dweller_tags, poi_tags, playerMap);
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
        if (control.verboseFlag) {
            log::info("  Creating tiles for {}...", mybasename(fn));
        } else {
            log::trace("Creating tiles for {}...", mybasename(fn));
        }
        PngTiler pngTiler(fn, control.tileWidth, control.tileHeight, dirOut);
        pngTiler.doTile();

        return 0;
    }


    int32_t MinecraftWorld_LevelDB::doOutput_Tile()
    {
        if (!control.doTiles) {
            return 0;
        }

        for (int32_t dimid = 0; dimid < kDimIdCount; dimid++) {
            log::info("Splitting images to tiles: Dimension '{}' ({})...", dimDataList[dimid]->getName(), dimid);
            doOutput_Tile_image(control.fnLayerTop[dimid]);
            doOutput_Tile_image(control.fnLayerBiome[dimid]);
            doOutput_Tile_image(control.fnLayerHeight[dimid]);
            doOutput_Tile_image(control.fnLayerHeightGrayscale[dimid]);
            doOutput_Tile_image(control.fnLayerHeightAlpha[dimid]);
            doOutput_Tile_image(control.fnLayerSlimeChunks[dimid]);
            doOutput_Tile_image(control.fnLayerShadedRelief[dimid]);
            int dimensionBottomY = control.dimYBottom[dimid];
            int dimensionTopY = control.dimYTop[dimid];
            for (int32_t cy = dimensionBottomY; cy <= dimensionTopY; cy++) {
                if (cy % 32 == 0) {
                    log::info("  Layer {} in {}..{}", cy, dimensionBottomY, dimensionTopY);
                }
                doOutput_Tile_image(control.fnLayerRaw[dimid][cy - dimensionBottomY]);
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
        char tmpstring[1025];

        log::info("Do Output: html viewer");

        //sprintf(tmpstring, "%s/bedrock_viz.html.template", dirExec.c_str());
        const std::string fnHtmlSrc = static_path("bedrock_viz.html.template").generic_string();

        //sprintf(tmpstring, "%s/bedrock_viz.js", dirExec.c_str());
        const std::string fnJsSrc = static_path("bedrock_viz.js").generic_string();

        //sprintf(tmpstring, "%s/bedrock_viz.css", dirExec.c_str());
        const std::string fnCssSrc = static_path("bedrock_viz.css").generic_string();

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
        } else {
            // remove the placeholder it would have gone to
            replaceStrings.push_back(
                std::make_pair(
                    std::string("%JSFILE%"),
                    ""
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
                "// mcpe_viz javascript helper file -- created by bedrock_viz program\n"
                "var worldName = '%s';\n"
                "var worldSeed = %lld;\n"
                "var creationTime = '%s';\n"
                "var creationBedrockVizVersion = '%s';\n"
                "var loadGeoJSONFlag = %s;\n"
                "var fnGeoJSON = '%s';\n"
                "var useTilesFlag = %s;\n"
                "var tileW = %d;\n"
                "var tileH = %d;\n"
                "var dimensionInfo = {\n", escapeString(getWorldName().c_str(), "'").c_str(),
                (long long int) getWorldSeed(), escapeString(timebuf, "'").c_str(),
                version, control.noForceGeoJSONFlag ? "true" : "false",
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

                /* 
                tomnolan: I'm of the opinion that we should be setting the default position to 0,0 
                          for any dimensions the player is not located. If the player is in the nether, 
                          we shouldn't be setting the map position to x/8,z/8 in the overworld because 
                          this position has no relevance. They may have never traveled to that position
                          in the overworld ever and a user will see a mass of white space on their screen
                          when they load the map by default. This is very confusing.

                          I'm removing this for now and setting the default to 0,0 for dimensions the
                          player is not present.
                */
                // set default position to 0
                if (did != playerPositionDimensionId)
                {
                    px = 0;
                    py = 0;
                }
                /*
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
                */
                fprintf(fp, "  playerPosX: %lf,\n", px);
                fprintf(fp, "  playerPosY: %lf,\n", py);

                // list of blocks that were added to geojson
                fprintf(fp, "  geojsonBlocks: [ ");
                int32_t llen = int32_t(dimDataList[did]->blockToGeoJSONList.size());
                for (const auto& it : dimDataList[did]->blockToGeoJSONList) {
                    fprintf(fp, "'%s'", Block::queryName(it).c_str());
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
                fprintf(fp, "  fnLayerSlimeChunks: '%s',\n", makeTileURL(control.fnLayerSlimeChunks[did]).c_str());

                int dimensionBottomY = control.dimYBottom[did];
                int dimensionTopY = control.dimYTop[did];

                fprintf(fp, "  bottomLayer: %d,\n", dimensionBottomY);
                fprintf(fp, "  topLayer: %d,\n", dimensionTopY);
                fprintf(fp, "  listLayers: [\n");
                for (int32_t y = dimensionBottomY; y <= dimensionTopY; y++) {
                    fprintf(fp, "    '%s',\n", makeTileURL(control.fnLayerRaw[did][y - dimensionBottomY]).c_str());
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
            for(auto& i: Block::list()) {
                if (i->hasVariants()) {
                    for(auto& v: i->getVariants()) {
                        fprintf(fp, "'%d': { name: '%s', id: %d, blockdata: %d },\n",
                                local_be32toh(v.second->color()),
                                escapeString(v.second->name, "'").c_str(),
                                i->id, v.second->data
                        );
                    }
                }
                else {
                    if (i->is_color_set()) {
                        fprintf(fp, "'%d': { name: '%s', id: %d, blockdata: %d },\n",
                                local_be32toh(i->color()), escapeString(i->name, "'").c_str(),
                                i->id, 0
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
            for(auto& i : Biome::list()) {
                if (i->is_color_set()) {
                    fprintf(fp, "'%d': { name: '%s', id: %d },\n", local_be32toh(i->color()),
                            escapeString(i->name, "'").c_str(), i->id
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
            "\"crs\": { \"type\": \"name\", \"properties\": { \"name\": \"bedrock_viz-image\" } },\n"
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
            
            // TODO: Remove this whole block?
            // if (control.autoTileFlag) {
            //     int32_t xdimId = kDimIdOverworld;
            //     const int32_t chunkW = (dimDataList[xdimId]->getMaxChunkX() - dimDataList[xdimId]->getMinChunkX() +
            //         1);
            //     const int32_t imageW = chunkW * 16;
            //     const int32_t chunkH = (dimDataList[xdimId]->getMaxChunkZ() - dimDataList[xdimId]->getMinChunkZ() +
            //         1);
            //     const int32_t imageH = chunkH * 16;

            //     // todobig - 2048 a reasonable default max image size before we auto-tile?
            //     int32_t maxImageSize = 2048;
            //     if (imageW > maxImageSize || imageH > maxImageSize) {
            //         log::info("Detected large images and 'auto-tile' is enabled, enabling tiles");
            //         control.doTiles = true;
            //     }
            // }

            doOutput_Tile();
            doOutput_html();
            doOutput_GeoJSON();
        }

        return 0;
    }

    std::unique_ptr<MinecraftWorld_LevelDB> world;
}
