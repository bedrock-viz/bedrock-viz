#include "gtest/gtest.h"

namespace mcpe_viz {
    TEST(Example, Test) {
        ASSERT_EQ("0", "0");
    }
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}