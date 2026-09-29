#pragma once
#include "robot_interface.h"

// Testable core: runs on injected encoder/motor interfaces.
// No Arduino / Pololu includes here so it compiles natively.
void driveDistance(int distance_mm, IEncoders &enc, IMotors &mot);
