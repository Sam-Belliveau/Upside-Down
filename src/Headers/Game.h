#ifndef GAME_H
#define GAME_H

// File Loader uses this part of the game file
#include "NumberLookup.h"
#include "Constants.h"
#include "FileLoader.h"

class Game
{
public: // Static methods and enums
    // Enums and static methods
    template<class T>
    static T randomize(T n);

    enum TypeProps : TypePropsType 
    { 
        None = 0x00, // Blank
        Solid      = TypePropsBit(0x00), // Stops player from passing through
        LowGravity = TypePropsBit(0x01), // Slows gravity to 1/2
        Trap       = TypePropsBit(0x02), // Kills Player
        Jumpable   = TypePropsBit(0x03), // Player can jump off of
        Bounce     = TypePropsBit(0x04), // Player bounces off of
        Slow       = TypePropsBit(0x05), // Slows Down User
        Smog       = TypePropsBit(0x06), // Hinders Visibility
        StopStorm  = TypePropsBit(0x07), // Moves the storm
        MoveRight  = TypePropsBit(0x08), // Moves player right
        MoveLeft   = TypePropsBit(0x09), // Moves player left
        Coin       = TypePropsBit(0x0A), // Collectable Coin
        Goal       = TypePropsBit(0x0B)  // Goes To Next Level
    };

    struct GameTypeData
    {
        const char* name; // block name
        sf::Color color; // block color
        IntType randomness; // randomness of block color
        double cameraSpeed; // paralax
        double textureSpeed; // how fast block color moves
        RawIntType propertys; // game properties

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
    void coinLoop();
    void reset();

public: // World/Rendering
    IntType loadWorld(const IntType);
    const Byte* returnWorldPixels(bool);
    HashType updateLevelHash();

public: // Getters
    IntType getCameraX() const;
    IntType getFinalLevel() const;
    IntType getLevel() const;
    IntType getDeaths() const;
    IntType getCoins() const;
    IntType getMaxCoins() const;
    IntType getLevelCoins(IntType) const;
    IntType getLevelMaxCoins(IntType) const;
    IntType getFrame() const;
    IntType getLevelFrame(IntType) const;
    HashType getLevelHash() const;
    bool getWinner() const;
    bool getCheater() const;
    void setCheater();

private: // Member Variables
    HashType hash, maxCoins;
    RawIntType rawFrame; // Used for game mechanics, always ticks
    IntType finalLevel; // Used to count number of levels
    IntType level, frame, deaths, coins;
    IntType levelFrames[MAX_LEVEL_COUNT];
    IntType levelCoins[MAX_LEVEL_COUNT];
    IntType levelMaxCoins[MAX_LEVEL_COUNT];

    sf::Vector2<IntType> player; 
    IntType cameraX, trapX;
    
    GravityType gravity = GravityType::Down; 
    bool canJump = true, canBounce = true;
    bool hasCheated = false;

    GameTypeData playerBlockData, groundBlockData;
    GameType world[GAME_LENGTH][GAME_HEIGHT];
    Byte buffer[GAME_HEIGHT][GAME_WIDTH][4];
};

#endif // GAME_H
