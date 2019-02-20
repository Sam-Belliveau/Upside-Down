#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

#include <string> // File Names, Times 
#include <cstdint> // Fixed Width Types
#include <sstream> // Building leaderboard
#include <iomanip> // Building leaderboard
#include <cmath> // Misc. Math

// Game Types
using Byte = std::uint8_t;
using IntType = std::int_fast32_t;

// Game Text
static constexpr double TEXT_SCALE = 8;

// Game Size / Pixel Measurements
static constexpr IntType GAME_WIDTH = 40;
static constexpr IntType GAME_HEIGHT = 24;
static constexpr IntType GAME_LENGTH = 256;
static constexpr IntType PIXEL_COUNT = GAME_WIDTH*GAME_HEIGHT*4;

static constexpr IntType GAME_SCALE = 24;
static constexpr IntType GAME_FPS = 24;
static constexpr IntType START_SIZE = 9;

// Left Wall Trap
static constexpr IntType TRAP_START = -8;
static constexpr IntType TRAP_SMOOTH = 8;
static constexpr IntType TRAP_SPEED = 3;
static constexpr IntType TRAP_LEAD = IntType(48*TRAP_SPEED);

// Level Data / Image Processing
static constexpr IntType MAX_LEVEL_COUNT = 100;
static constexpr std::uint8_t COLOR_THRESHOLD = 128;

// Random Number Generation
static constexpr IntType p = 4091;
static constexpr IntType q = 4093;
static constexpr IntType M = p*q;

#endif // CONSTANTS_H_INCLUDED
