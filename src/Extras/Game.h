#ifndef GAME_H
#define GAME_H

#include "Constants.h"

class Game
{
public: // Static methods and enums
    // Enums and static methods
    static IntType randomize(IntType n)
    {
        // 12 Rounds of Blum Blum Shub with a 
        // 24 bit M setup. Should be pretty random
        n = n*n % M; n = n*n % M; n = n*n % M;
        n = n*n % M; n = n*n % M; n = n*n % M;
        n = n*n % M; n = n*n % M; n = n*n % M;
        n = n*n % M; n = n*n % M; n = n*n % M;

        // Blum Blum Shub only produces positive numbers, 
        // but I use std::abs incase the RNG changes
        return std::abs(n);
    }

    enum GameType : Byte 
    { 
        //////// 0bRGB
        Sky    = 0b000, 
        Ground = 0b010, 
        Trap   = 0b100,
        Bounce = 0b001
    };

    enum GravityType : IntType 
    { 
        Up   = -1, 
        Down = 1
    };

public:
    Game();

    void gameLoop();

    IntType loadWorld(const IntType);
    Byte* returnWorldPixels();

    IntType getCameraX() const;
    IntType getLevel() const;
    IntType getFrame() const;
    IntType getLevelFrame(IntType) const;
    bool getWinner() const;
    bool getCheater() const;

private:
    // Game Loop
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

    // Controls
    bool isJoystickConnected();
    float joyXAxis();
    float joyYAxis();

    bool upKey();
    bool downKey();
    bool leftKey();
    bool rightKey();
    bool jumpKey();

    bool cheatKey();
    bool flyCheatKey();
    bool levelCheatKey();

    void reset();

    IntType level = START_LEVEL, frame = 0;
    IntType levelFrames[MAX_LEVEL_COUNT] = {0};

    sf::Vector2<IntType> player = sf::Vector2<IntType>(5, 18); 
    IntType cameraX = 0, trapX = TRAP_START;
    
    GravityType gravity = GravityType::Down; 
    bool canJump = true, canBounce = true;
    bool hasCheated = false;

    GameType world[GAME_LENGTH][GAME_HEIGHT];
    Byte buffer[GAME_HEIGHT*GAME_WIDTH*4];
    
};

#endif // GAME_H
