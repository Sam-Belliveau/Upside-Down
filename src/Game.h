#include "Constants.h"
#include <SFML/Graphics.hpp>

#ifndef GAME_H
#define GAME_H

class Game
{
private: // Static Objects
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

private:
    bool moveCameraLeft();
    bool moveCameraRight();
    void flipGravity();

    void reset();

    IntType level = 0, frame = 0;
    IntType levelFrames[MAX_LEVEL_COUNT] = {0};

    sf::Vector2<IntType> player = sf::Vector2<IntType>(5, 18); 
    IntType cameraX = 0, trapX = TRAP_START;
    
    GravityType gravity = GravityType::Down; 
    bool canJump = true, canBounce = true;

    GameType world[GAME_LENGTH][GAME_HEIGHT];
    Byte buffer[GAME_HEIGHT*GAME_WIDTH*4];
};

#endif // GAME_H
