#pragma once

#include <cstdint>
#include <string>
#include <memory>


#include <leveldb/db.h>

#include "dimension_data.h"
#include "common.h"

namespace mcpe_viz {

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

        MinecraftWorld_LevelDB();

        ~MinecraftWorld_LevelDB() {
            dbClose();
        }

        int32_t parseLevelFile(const std::string& fname);

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

        int32_t init();

        int32_t dbOpen(const std::string& dirDb);

        int32_t dbClose() {
            if (db != nullptr) {
                delete db;
                db = nullptr;
            }
            return 0;
        }

        int32_t calcChunkBounds();

        // this is where we go through every item in the leveldb, we parse interesting things as we go
        int32_t dbParse();

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

        int32_t doOutput_Tile_image(const std::string& fn);

        int32_t doOutput_Tile();


        std::string makeTileURL(const std::string& fn);


        int32_t doOutput_html();


        int32_t doOutput_colortest() const;


        int32_t doOutput_GeoJSON();


        int32_t doOutput();

        void worldPointToImagePoint(int32_t dimId, double wx, double wz, double& ix, double& iy, bool geoJsonFlag) {
            // hack to avoid using wrong dim on pre-0.12 worlds
            if (dimId < 0) { dimId = 0; }

            return dimDataList[dimId]->worldPointToImagePoint(wx, wz, ix, iy, geoJsonFlag);
        }

    };

    extern std::unique_ptr<MinecraftWorld_LevelDB> world;
}