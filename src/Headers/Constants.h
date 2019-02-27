#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

#include <SFML/Graphics.hpp>

#include <fstream> // Loading and saving levels
#include <cstdint> // Fixed Width Types
#include <sstream> // Building leaderboard
#include <iomanip> // Building leaderboard
#include <chrono> // Global frame count
#include <string> // File Names, Times 
#include <stack> // Undo in level editor
#include <cmath> // Misc. Math

// Game Types
using Byte = std::uint8_t;
using IntType = std::int32_t;
using RawIntType = std::uint32_t;

// Game FPS
static const IntType GAME_FPS = 24;

// Game Version
static const std::string GAME_VERSION = "v1.5";

// Game Size / Pixel Measurements
static const IntType GAME_WIDTH = 42;
static const RawIntType GAME_HEIGHT = 24;
static const RawIntType GAME_LENGTH = 256;
static const IntType START_SIZE = 9;

static const IntType GAME_START_X = START_SIZE/2;
static const IntType GAME_START_Y = 18;

static const IntType GAME_SCALE = 32;
static const double LOST_FOCUS_COLOR = 1.5;
static const sf::Color PLAYER_COLOR = sf::Color(196, 255, 196);

// Global frames
using CHRONO_CLOCK = std::chrono::steady_clock;
using CHRONO_UNIT = std::chrono::nanoseconds;
static const std::uintmax_t CHONO_UNIT_PER_SEC = 1000000000;
static const auto START_DURATION = CHRONO_CLOCK::now();
static IntType GET_GLOBAL_FRAME()
{
    auto now = CHRONO_CLOCK::now();
    auto t = std::chrono::duration_cast<CHRONO_UNIT>(now - START_DURATION).count();
    t *= GAME_FPS;
    t += CHONO_UNIT_PER_SEC/2; // Rounding
    t /= CHONO_UNIT_PER_SEC;
    return IntType(t);
}

// Game Peices / File Loading
static const RawIntType MAGIC_NUMBER = 0x53616d42; // "SamB"
static constexpr IntType GameTypeCount = 12;
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
    Goal       = 0xff
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
static const IntType TRAP_START = -240;
static const double TRAP_SMOOTH = 8;
static const double TRAP_SPEED = 4;
static const IntType TRAP_LEAD = IntType(GAME_WIDTH*TRAP_SPEED);

// Level Data / Image Processing
static const IntType START_LEVEL = 1;
static const IntType MAX_LEVEL_COUNT = 100;

// Text
static const IntType TEXT_X = 1;
static const IntType TEXT_Y = 9;
static const sf::Color GOOD_COLOR = sf::Color(0xE0,0xFF,0xE0);
static const sf::Color BAD_COLOR = sf::Color(0xFF,0x80,0x80);
static const std::string ttfFile = "./GameFiles/GameFont.ttf";
static sf::Font DEFAULT_GAME_FONT;

static sf::Text GET_DEFAULT_TEXT(double size)
{
    if(!DEFAULT_GAME_FONT.loadFromFile(ttfFile)) { return sf::Text(); }

    sf::Text defaultText;
    defaultText.setFont(DEFAULT_GAME_FONT);
    defaultText.setFillColor(sf::Color::White);
    defaultText.setOutlineColor(sf::Color::Black);
    defaultText.setOutlineThickness(size*2*TEXT_SCALE);
    defaultText.setCharacterSize(size*TEXT_SCALE*GAME_SCALE/1.5);
    defaultText.setScale(sf::Vector2f(1.0/TEXT_SCALE,1.0/TEXT_SCALE));
    return defaultText;
}

// Level Editor
static const IntType EDITOR_CAMERA_SPEED = 2;
static const IntType BLOCK_LIST_SIZE = 8;

// Sorting Blocks By Brightness
static const IntType R_LUMINANCE = 2126;
static const IntType G_LUMINANCE = 7152;
static const IntType B_LUMINANCE = 722;
static IntType GetLuminance(sf::Color in)
{
    IntType r = in.r * in.r * R_LUMINANCE;
    IntType g = in.g * in.g * G_LUMINANCE;
    IntType b = in.b * in.b * B_LUMINANCE;
    return r + g + b;
}

// Random Number Generation
using HashType = std::uint64_t;
static HashType ROTATE(HashType in, IntType rot) 
{
    return (in << rot) | (in >> (sizeof(in)*8 - rot)); 
}

static const HashType BBS_RNG_ROUNDS = 23;
static const HashType BBS_RNG_P = 59747; // cousin primes 
static const HashType BBS_RNG_Q = 59751;
static const HashType BBS_RNG_M = BBS_RNG_P*BBS_RNG_Q;
static const HashType BBS_RNG_SALT[19] = 
{
    0x5A04, 0xABFC, 0x800B, 0xCADC,
    0x9E44, 0x7A2E, 0xC345, 0x3484,
    0xFDD5, 0x6705, 0x0E1E, 0x9EC9, 
    0xDB73, 0xDBD3, 0x1055, 0x88CD,
    0x675F, 0xDA79, 0xE367
};

template<class T>
T RANDOMIZE(T number)
{
    const IntType offset = number%T(19);
    HashType n = static_cast<HashType>(number);

    // Modified Blum Blum Shub
    for(IntType i = 0; i < BBS_RNG_ROUNDS; ++i)
    {
        n = n*n % BBS_RNG_M;

        // Salt is used to fix issue around 0
        n += BBS_RNG_SALT[(i + n)%19];
    }
    
    return static_cast<T>(n);
}

// Level Names
static const std::string LEVEL_FOLDER = "./Levels/";
static const std::string LEVEL_PREFIX = "L";
static const std::string LEVEL_EXTENTION = ".lvl";

#endif // CONSTANTS_H_INCLUDED
