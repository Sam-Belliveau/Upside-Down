#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

#include <SFML/Graphics.hpp>

#include <fstream> // Loading and saving levels
#include <stack> // Undo in level editor
#include <string> // File Names, Times 
#include <cstdint> // Fixed Width Types
#include <sstream> // Building leaderboard
#include <iomanip> // Building leaderboard
#include <cmath> // Misc. Math

// Game Types
using Byte = std::uint8_t;
using IntType = std::int_fast32_t;
using UIntType = std::uint_fast32_t;

// Game Size / Pixel Measurements
static const IntType GAME_WIDTH = 42;
static const UIntType GAME_HEIGHT = 24;
static const UIntType GAME_LENGTH = 256;

static const IntType GAME_SCALE = 24;
static const IntType GAME_FPS = 24;
static const IntType START_SIZE = 9;

// Game Peices / File Loading
static const UIntType MAGIC_NUMBER = 0x01020304;
static constexpr IntType GameTypeCount = 9;
enum GameType : Byte 
{ 
    // Here are the IDs for each block
    Sky        = 0x00, 
    Ground     = 0x01, 
    Trap       = 0x02,
    Bounce     = 0x03,
    Mud        = 0x04,
    Water      = 0x05,
    Smog       = 0x06,
    LowGravity = 0x07,
    Goal       = 0x08,
};

// Game Text
static const double TEXT_SCALE = 4;

// Game Camera Measurements
static const IntType RIGHT_CAMERA_BOARDER = double(GAME_WIDTH) / (std::sqrt(2) + 1);
static const IntType LEFT_CAMERA_BOARDER = double(GAME_WIDTH) / 4;

// Controls 
static const IntType DEFAULT_JOYSTICK_PORT = 0;
static const IntType JUMP_BUTTONS[] = {0, 1};
static const IntType RESET_BUTTON = 8;
static const float Y_JOYSTICK_DEAD_ZONE = 80;
static const float X_JOYSTICK_DEAD_ZONE = 40;

// Smog Measurements
static const double SMOG_SIZE = 4;

// Left Wall Trap
static const IntType TRAP_START = -8;
static const double TRAP_SMOOTH = 8;
static const double TRAP_SPEED = 4;
static const IntType TRAP_LEAD = IntType(GAME_WIDTH*TRAP_SPEED);

// Level Data / Image Processing
static const IntType START_LEVEL = 1;
static const IntType MAX_LEVEL_COUNT = 100;
static const std::uint8_t COLOR_THRESHOLD = 128;

// Text
static const IntType TEXT_X = 1;
static const IntType TEXT_Y = 9;
static const sf::Color GOOD_COLOR = sf::Color(0xE0,0xFF,0xE0);
static const sf::Color BAD_COLOR = sf::Color(0xFF,0x80,0x80);
static sf::Font DEFAULT_GAME_FONT;

static sf::Text GET_DEFAULT_TEXT()
{
    const std::string ttfFile = "./GameFont.ttf";
    if(!DEFAULT_GAME_FONT.loadFromFile(ttfFile)) { return sf::Text(); }

    sf::Text defaultText;
    defaultText.setFont(DEFAULT_GAME_FONT);
    defaultText.setFillColor(sf::Color::White);
    defaultText.setOutlineColor(sf::Color::Black);
    defaultText.setOutlineThickness(2.5*TEXT_SCALE);
    defaultText.setCharacterSize(TEXT_SCALE*GAME_SCALE/1.5);
    defaultText.setScale(sf::Vector2f(1/TEXT_SCALE,1/TEXT_SCALE));
    return defaultText;
}

// Level Editor
static const IntType EDITOR_CAMERA_SPEED = 2;

// Random Number Generation
static const IntType BBS_RNG_P = 4091;
static const IntType BBS_RNG_Q = 4093;
static const IntType BBS_RNG_M = BBS_RNG_P*BBS_RNG_Q;
static const IntType BBS_RNG_SALT[16] = 
{
    0x248d, 0x41c5, 0xa061, 0x342b,
    0x5142, 0x75cf, 0x95c2, 0xce40, 
    0x0e96, 0x3f2f, 0x1a69, 0xd765, 
    0x40eb, 0x5b6b, 0x64c2, 0x6b50 
};

// Level Names
static const std::string LEVEL_FOLDER = "./Levels/";
static const std::string LEVEL_PREFIX = "L";
static const std::string LEVEL_EXTENTION = ".udl"; // u(pside)d(own)l(evel)

#endif // CONSTANTS_H_INCLUDED
