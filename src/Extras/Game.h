#ifndef GAME_H
#define GAME_H

#include "Constants.h"

class Game
{
public: // Static methods and enums
    // Enums and static methods
    static IntType randomize(IntType n);

    static constexpr IntType GameTypeCount = 8;
    enum GameType : Byte 
    { 
        ////////     0bRGBA
        Sky        = 0b0001, 
        Ground     = 0b0101, 
        Trap       = 0b1001,
        Bounce     = 0b0011,
        Mud        = 0b1011,
        Water      = 0b0111,
        Smog       = 0b1101,
        LowGravity = 0b1111
    };

    struct GameTypeData
    {
        const char* name;
        const sf::Color color;
        const IntType randomness, cameraSpeed;
        const bool solid;  // Stops player from passing through
        const bool liquid; // Slows gravity to 1/2
        const bool trap;   // Kills Player
        const bool jump;   // Player can jump off of
        const bool bounce; // Player bounces off of
        const bool smog;   // Hinders Visibility
        IntType randomize(IntType cx, IntType x, IntType y) const;
    };

    struct GameTypeLink { GameType type; GameTypeData data; };
    static const GameTypeLink GameTypeList[GameTypeCount];
    static const GameTypeData GetGameTypeData(GameType input);
        
    enum GravityType : IntType { Up = -1, Down = 1 };

public:
    Game();

    // Controls
    static bool isJoystickConnected();
    static float joyXAxis();
    static float joyYAxis();

    static bool upKey();
    static bool downKey();
    static bool leftKey();
    static bool rightKey();
    static bool jumpKey();

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
    IntType getFrame() const;
    IntType getLevelFrame(IntType) const;
    bool getWinner() const;
    bool getCheater() const;
    void setCheater();

private: // Member Variables
    IntType finalLevel;
    IntType level, frame;
    IntType levelFrames[MAX_LEVEL_COUNT];

    sf::Vector2<IntType> player; 
    IntType cameraX, trapX;
    
    GravityType gravity = GravityType::Down; 
    bool canJump = true, canBounce = true;
    bool hasCheated = false;

    GameType world[GAME_LENGTH][GAME_HEIGHT];
    Byte buffer[GAME_HEIGHT][GAME_WIDTH][4];
};

#endif // GAME_H
