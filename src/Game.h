#include "Constants.h"
#include <SFML/Graphics.hpp>
#include <cstddef>
#include <string>

#ifndef GAME_H
#define GAME_H

enum type{ sky, ground, bounce, trap };

class Game
{
public:
    Game();

    void gameLoop();
    void reset();

    IntType level = 0, frame = 0;
    IntType levelFrames[LEVEL_COUNT] = {0};
    IntType playerX = 5, playerY = 18, cX = 0, trapX = TRAP_START;
    void loadWorld(const IntType w);

    unsigned char* returnPixels();

private:
    bool gravity = false; // false = down, true = up;
    bool canJump = true, canBounce = true;

    bool moveCameraLeft();
    bool moveCameraRight();

    IntType randomize(const IntType input);

    unsigned char world[GAME_LENGTH][GAME_HEIGHT];
    unsigned char* buffer = new unsigned char[GAME_HEIGHT*GAME_WIDTH*4];
};

#endif // GAME_H
