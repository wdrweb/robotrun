#include <gtest/gtest.h>
#include "drive_distance.h"

// Stub encoder: simulates progress. Every read adds `step` counts
// so the while(true) loop in driveDistance terminates.
class StubEncoders : public IEncoders {
public:
    int16_t left = 0;
    int16_t right = 0;
    int16_t step = 200;

    int16_t getCountsLeft() override { left += step; return left; }
    int16_t getCountsRight() override { right += step; return right; }
    int16_t getCountsAndResetLeft() override { left = 0; return 0; }
    int16_t getCountsAndResetRight() override { right = 0; return 0; }
};

// Stub motors: records what was commanded.
class StubMotors : public IMotors {
public:
    int16_t lastLeft = 999;
    int16_t lastRight = 999;
    int16_t firstLeft = 999;
    int16_t firstRight = 999;
    bool sawFirst = false;

    void setSpeeds(int16_t left, int16_t right) override {
        if (!sawFirst) { firstLeft = left; firstRight = right; sawFirst = true; }
        lastLeft = left;
        lastRight = right;
    }
    void setLeftSpeed(int16_t speed) override { lastLeft = speed; }
    void setRightSpeed(int16_t speed) override { lastRight = speed; }
};

TEST(DriveDistance, StopsAtTarget) {
    StubEncoders enc;
    StubMotors mot;
    driveDistance(100, enc, mot);
    EXPECT_EQ(mot.lastLeft, 0);
    EXPECT_EQ(mot.lastRight, 0);
}

TEST(DriveDistance, StartsForward) {
    StubEncoders enc;
    StubMotors mot;
    driveDistance(100, enc, mot);
    EXPECT_GT(mot.firstLeft, 0);
    EXPECT_GT(mot.firstRight, 0);
}

TEST(DriveDistance, NegativeDistanceDrivesBackward) {
    StubEncoders enc;
    StubMotors mot;
    driveDistance(-100, enc, mot);
    EXPECT_LT(mot.firstLeft, 0);
    EXPECT_LT(mot.firstRight, 0);
    EXPECT_EQ(mot.lastLeft, 0);
    EXPECT_EQ(mot.lastRight, 0);
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
