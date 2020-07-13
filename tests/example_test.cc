#include "nbt.h"

#include <gtest/gtest.h>

namespace mcpe_viz {
    TEST(Example, Test) {
        ASSERT_EQ("0", "0");
    }
    
    TEST(Another, Test) {
        std::string geojson_header = makeGeojsonHeader(0,0,false);
        ASSERT_EQ(true,true);
    }
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
