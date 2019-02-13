#include <string>
#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

static const unsigned int GAME_WIDTH = 40;
static const unsigned int GAME_HEIGHT = 24;
static const unsigned int PIXEL_COUNT = GAME_WIDTH*GAME_HEIGHT*4;
static const unsigned int GAME_SCALE = 16;
static const unsigned int GAME_LENGTH = 256;

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

static const unsigned int LEVEL_COUNT = sizeof(LEVEL_NAMES) / sizeof(std::string);

#endif // CONSTANTS_H_INCLUDED
