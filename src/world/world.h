#pragma once

#include <cstdint>
#include <string>
#include <memory>


#include <leveldb/db.h>

#include "dimension_data.h"
#include "common.h"
#include "../control.h"

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
        leveldb::DB* db;
        std::unique_ptr<leveldb::Options> dbOptions;
        int32_t totalRecordCt = 0;
        Control* const control;
        std::unique_ptr<DimensionData_LevelDB> dimDataList[kDimIdCount];
    public:
        explicit MinecraftWorld_LevelDB(Control* ctrl);

        ~MinecraftWorld_LevelDB() {
            delete this->db;
            this->db = nullptr;
        }
    private:
        int32_t parseLevelFile(const std::string& fname);

        int32_t parseLevelName(const std::string& fname);

        int32_t calcChunkBounds();

        int32_t doOutput_Tile_image(const std::string& fn);

        int32_t doOutput_Tile();

        std::string makeTileURL(const std::string& fn);

        int32_t doOutput_html();

        int32_t doOutput_GeoJSON();

        int32_t dbOpen();

    public:
        int32_t init();

        int32_t doOutput();

        // this is where we go through every item in the leveldb, we parse interesting things as we go
        int32_t dbParse();

        void worldPointToImagePoint(int32_t dimId, double wx, double wz, double& ix, double& iy, bool geoJsonFlag) {
            // hack to avoid using wrong dim on pre-0.12 worlds
            if (dimId < 0) { dimId = 0; }

            return dimDataList[dimId]->worldPointToImagePoint(wx, wz, ix, iy, geoJsonFlag);
        }

        // the old way of image generation
        void doProcessV1();

        // the new way
        void doProcessV2();

    };

    extern std::unique_ptr<MinecraftWorld_LevelDB> world;
}