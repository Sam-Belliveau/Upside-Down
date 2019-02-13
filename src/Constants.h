#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

#include <string>
#include <cstdint>

using IntType = std::int_fast32_t;

static const IntType GAME_WIDTH = 40;
static const IntType GAME_HEIGHT = 24;
static const IntType PIXEL_COUNT = GAME_WIDTH*GAME_HEIGHT*4;
static const IntType GAME_SCALE = 16;
static const IntType GAME_LENGTH = 256;

static const IntType TRAP_START = -4;
static const IntType TRAP_SMOOTH = 8;
static const double TRAP_SPEED = 2.4;

static const std::string LEVEL_NAMES[]
{
    "Levels/L1.bmp",
    "Levels/L2.bmp",
    "Levels/L3.bmp",
    "Levels/L4.bmp",
    "Levels/L5.bmp",
    "Levels/L6.bmp",
    "Levels/L7.bmp",
    "Levels/L8.bmp",
    "Levels/L9.bmp",
    "Levels/L10.bmp",
    "Levels/L11.bmp",
    "Levels/L12.bmp"
};

static const IntType LEVEL_COUNT = sizeof(LEVEL_NAMES) / sizeof(std::string);

#endif // CONSTANTS_H_INCLUDED
