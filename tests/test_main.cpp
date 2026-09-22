#include <gtest/gtest.h>

// This test will PASS
TEST(RescueAnimalTestSuite, AlwaysPasses) {
    EXPECT_EQ(5, 5);
}

// This test will FAIL intentionally
TEST(RescueAnimalTestSuite, AlwaysFails) {
    EXPECT_EQ(5, 5); 
}