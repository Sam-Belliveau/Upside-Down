#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <fstream> // Loading and saving levels
#include <cstdint> // Fixed Width Types
#include <sstream> // Building leaderboard
#include <iomanip> // Building leaderboard
#include <chrono> // Global frame count
#include <string> // File Names, Times 
#include <stack> // Undo in level editor
#include <cmath> // Misc. Math
#include "NumberLookup.h"

// Game Types
using Byte = std::uint8_t;
using IntType = std::int32_t;
using RawIntType = std::uint32_t;
using HashType = std::uint64_t;
using TypePropsType = std::uint64_t;

// Game FPS
static constexpr IntType GAME_FPS = 25;

// Game Version
static const std::string GAME_VERSION = "v1.0";

// Game Size / Pixel Measurements
static constexpr IntType GAME_WIDTH = 42;
static constexpr RawIntType GAME_HEIGHT = 24;
static constexpr RawIntType GAME_LENGTH = 256;
static constexpr IntType START_SIZE = 9;

static constexpr IntType GAME_START_X = START_SIZE/2;
static constexpr IntType GAME_START_Y = 18;

static constexpr IntType GAME_SCALE = 32;
static constexpr double LOST_FOCUS_COLOR = 1.5;
static const sf::Color PLAYER_COLOR = sf::Color(0, 0, 0);

// Sounds
static const std::string SOUND_DIRECTORY = "./GameFiles/";
static const std::string SOUND_EXTENTIONS[] = {".wav", ".ogg", ".flac"};

static constexpr double COIN_PITCH = 1;
static constexpr double COIN_VOL = 100;

static constexpr double JUMP_PITCH = 1;
static constexpr double JUMP_VOL = 50;

static constexpr double BOUNCE_PITCH = 1;
static constexpr double BOUNCE_VOL = 60;

static constexpr double DEATH_PITCH = 1;
static constexpr double DEATH_VOL = 75;

static constexpr double WIN_PITCH = 1;
static constexpr double WIN_VOL = 100;

static constexpr double OVERWORLD_PITCH = 1;
static constexpr double OVERWORLD_VOL = 30;

static constexpr double LOWGRAVITY_PITCH = 1.5;

// Global frames
using CHRONO_CLOCK = std::chrono::steady_clock;
static constexpr std::uintmax_t CHONO_UNIT_PER_SEC = 1000000000;
static IntType GET_GLOBAL_FRAME()
{
    auto now = CHRONO_CLOCK::now();
    auto t = now.time_since_epoch().count();
    t *= GAME_FPS;
    t += CHONO_UNIT_PER_SEC/2; // Rounding
    t /= CHONO_UNIT_PER_SEC;
    return IntType(t);
}

// Game Peices / File Loading
static constexpr RawIntType MAGIC_NUMBER = 0x53616d42; // "SamB"
static constexpr IntType GameTypeCount = 13;
enum GameType : Byte 
{ 
    // Here are the IDs for each block
    Sky        = 0, 
    Ground     = 1, 
    Trap       = 2,
    Bounce     = 3,
    Mud        = 4,
    Water      = 5,
    Smog       = 6,
    LowGravity = 7,
    MoveRight  = 8,
    MoveLeft   = 9,
    Honey      = 10,
    Coin       = 11,
    Goal       = 0xff
};

static constexpr TypePropsType TypePropsBit(IntType bit) 
{ 
    return TypePropsType(0x1) << bit;
}

// Game Text
static constexpr double TEXT_SCALE = 4;

// Game Camera Measurements
static constexpr IntType RIGHT_CAMERA_BOARDER = GAME_WIDTH / 2;
static constexpr IntType LEFT_CAMERA_BOARDER = GAME_WIDTH / 3;

// Controls 
static constexpr IntType DEFAULT_JOYSTICK_PORT = 0;
static constexpr IntType JUMP_BUTTONS[] = {0, 1};
static constexpr IntType RESET_BUTTON = 8;
static constexpr float Y_JOYSTICK_DEAD_ZONE = 80;
static constexpr float X_JOYSTICK_DEAD_ZONE = 40;

// Smog Measurements
static constexpr double SMOG_SIZE = 4;

// Left Wall Trap
static constexpr double TRAP_SMOOTH = 8;
static constexpr double TRAP_SPEED = 5;
static constexpr IntType TRAP_LEAD = IntType(GAME_WIDTH*TRAP_SPEED*1.5);
static constexpr IntType TRAP_START = -TRAP_LEAD;

// Level Data / Image Processing
static constexpr IntType START_LEVEL = 1;
static constexpr IntType MAX_LEVEL_COUNT = 100;

// Text
static constexpr IntType TEXT_X = 1;
static constexpr IntType TEXT_Y = 9;
static const sf::Color GOOD_COLOR = sf::Color(196,255,196);
static const sf::Color BAD_COLOR = sf::Color(255,196,196);
static const std::string ttfFile = "./GameFiles/GameFont.ttf";
static sf::Font DEFAULT_GAME_FONT;

static sf::Text GET_DEFAULT_TEXT(double size)
{
    if(!DEFAULT_GAME_FONT.loadFromFile(ttfFile)) { return sf::Text(); }

    sf::Text defaultText;
    defaultText.setFont(DEFAULT_GAME_FONT);
    defaultText.setFillColor(sf::Color::White);
    defaultText.setOutlineColor(sf::Color::Black);
    defaultText.setOutlineThickness(size*2.5*TEXT_SCALE);
    defaultText.setCharacterSize(size*TEXT_SCALE*GAME_SCALE/1.5);
    defaultText.setScale(sf::Vector2f(1.0/TEXT_SCALE,1.0/TEXT_SCALE));
    return defaultText;
}

// Level Editor
static constexpr IntType EDITOR_CAMERA_SPEED = 2;
static constexpr IntType BLOCK_LIST_SIZE = 8;

// Sorting Blocks By Brightness
static constexpr IntType R_LUMINANCE = 2126;
static constexpr IntType G_LUMINANCE = 7152;
static constexpr IntType B_LUMINANCE = 722;
static IntType GetLuminance(sf::Color in)
{
    IntType r = in.r * in.r * R_LUMINANCE;
    IntType g = in.g * in.g * G_LUMINANCE;
    IntType b = in.b * in.b * B_LUMINANCE;
    return r + g + b;
}

// Random Number Generation
static constexpr IntType BBS_RNG_P = 43; // cousin primes 
static constexpr IntType BBS_RNG_Q = 47;
static constexpr IntType BBS_RNG_M = BBS_RNG_P*BBS_RNG_Q;

static IntType RANDOMIZE(IntType n)
{
    const Byte startIndex = n & 0xff;
    n %= BBS_RNG_M;
    IntType pool = n;

    // 13 Rounds of Modified Blum Blum Shub
    for(IntType i = 0; i < 13; ++i)
    {
        n *= n;
        n += LookUp::PiTable[0xff & (startIndex + n)];
        n %= BBS_RNG_M;
        pool += n;
    }

    return pool % BBS_RNG_M;
}

// Level Names
static const std::string LEVEL_FOLDER = "./Levels/";
static const std::string LEVEL_PREFIX = "L";
static const std::string LEVEL_EXTENTION = ".lvl";
static const IntType LEVEL_HASH_TIME = GAME_FPS*4;

#endif // CONSTANTS_H_INCLUDED
