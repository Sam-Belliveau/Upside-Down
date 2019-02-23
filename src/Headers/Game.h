#ifndef GAME_H
#define GAME_H

// File Loader uses this part of the game file
#include "Constants.h"
#include "FileLoader.h"

class Game
{
public: // Static methods and enums
    // Enums and static methods
    static IntType randomize(IntType n);

    enum TypeProps : RawIntType 
    { 
        None = 0x00, // Blank
        Solid     = RawIntType(1) << 0, // Stops player from passing through
        Liquid    = RawIntType(1) << 1, // Slows gravity to 1/2
        Trap      = RawIntType(1) << 2, // Kills Player
        Jumpable  = RawIntType(1) << 3, // Player can jump off of
        Bounce    = RawIntType(1) << 4, // Player bounces off of
        Smog      = RawIntType(1) << 5, // Hinders Visibility
        StopStorm = RawIntType(1) << 6, // Moves the storm
        MoveRight = RawIntType(1) << 7, // Moves player right
        MoveLeft  = RawIntType(1) << 8, // Moves player left
        Goal      = RawIntType(1) << 30 // Goes To Next Level
    };

    struct GameTypeData
    {
        const char* name; // block name
        sf::Color color; // block color
        IntType randomness; // randomness of block color
        double cameraSpeed; // paralax
        double textureSpeed; // how fast block color moves
        IntType propertys; // game propertys

        bool getProp(RawIntType) const;
        IntType randomize(IntType, IntType, IntType) const;
    };

    struct GameTypeLink { GameType type; GameTypeData data; };
    static const GameTypeLink GameTypeList[GameTypeCount];
    static const GameTypeData GetTypeData(GameType);

    enum GravityType : IntType { Up = -1, Down = 1 };

public:
    Game();

    // Controls
    static bool isJoystickConnected();
    static float joyXAxis();
    static float joyYAxis();

    static bool resetKey();

    static bool upKey();
    static bool downKey();
    static bool leftKey();
    static bool rightKey();
    static bool jumpKey(GravityType);

    static bool cheatKey();
    static bool flyCheatKey();
    static bool levelCheatKey();
    static bool editorCheatKey();

public: // Game Loop
    void gameLoop();

private: // Subunits of Game Loop
    void resetKeyLoop();
    void frameTimeLoop();
    void goalLoop();
    bool cheatLoop();
    void trapLoop();
    void jumpLoop();
    void bounceLoop();
    void movementLoop();
    void cameraLoop();
    void gravityLoop();
    void reset();

public: // World/Rendering
    IntType loadWorld(const IntType);
    const Byte* returnWorldPixels();

public: // Getters
    IntType getCameraX() const;
    IntType getFinalLevel() const;
    IntType getLevel() const;
    IntType getDeaths() const;
    IntType getFrame() const;
    IntType getLevelFrame(IntType) const;
    bool getWinner() const;
    bool getCheater() const;
    void setCheater();

private: // Member Variables
    IntType finalLevel;
    IntType level, frame, deaths;
    IntType levelFrames[MAX_LEVEL_COUNT];

    sf::Vector2<IntType> player; 
    IntType cameraX, trapX;
    
    GravityType gravity = GravityType::Down; 
    bool canJump = true, canBounce = true;
    bool hasCheated = false;

    GameTypeData playerTypeData;
    GameType world[GAME_LENGTH][GAME_HEIGHT];
    Byte buffer[GAME_HEIGHT][GAME_WIDTH][4];
};

#endif // GAME_H
