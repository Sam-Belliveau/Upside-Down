#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

#include <SFML/Graphics.hpp>

#include <string> // File Names, Times 
#include <cstdint> // Fixed Width Types
#include <sstream> // Building leaderboard
#include <iomanip> // Building leaderboard
#include <limits> // Cheating prevention
#include <cmath> // Misc. Math

// Game Types
using Byte = std::uint8_t;
using IntType = std::int_fast32_t;

// Game Text
static const double TEXT_SCALE = 8;

// Game Size / Pixel Measurements
static const IntType GAME_WIDTH = 40;
static const IntType GAME_HEIGHT = 24;
static const IntType GAME_LENGTH = 256;
static const IntType PIXEL_COUNT = GAME_WIDTH*GAME_HEIGHT*4;

static const IntType GAME_SCALE = 24;
static const IntType GAME_FPS = 24;
static const IntType CHEAT_SPEED = 2;
static const IntType START_SIZE = 9;

// Game Camera Measurements
static const IntType RIGHT_CAMERA_BOARDER = double(GAME_WIDTH) / (std::sqrt(2) + 1);
static const IntType LEFT_CAMERA_BOARDER = double(GAME_WIDTH) / 4;

// Controls 
static const IntType DEFAULT_JOYSTICK_PORT = 0;
static const IntType JUMP_BUTTONS[] = {0, 1, 2, 3};
static const float Y_JOYSTICK_DEAD_ZONE = 40;
static const float X_JOYSTICK_DEAD_ZONE = 40;

// Left Wall Trap
static const IntType TRAP_START = -8;
static const double TRAP_SMOOTH = 8;
static const double TRAP_SPEED = 3;
static const IntType TRAP_LEAD = IntType(48*TRAP_SPEED);

// Level Data / Image Processing
static const IntType START_LEVEL = 1;
static const IntType MAX_LEVEL_COUNT = 100;
static const std::uint8_t COLOR_THRESHOLD = 128;

// Random Number Generation
static const IntType p = 4091;
static const IntType q = 4093;
static const IntType M = p*q;

#endif // CONSTANTS_H_INCLUDED
