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

    unsigned int level = 0;
    void loadWorld(const unsigned int w);

    unsigned char* returnPixels();

private:
    std::int_fast32_t playerX = 5, playerY = 18, cX = 0;
    bool gravity = false; // false = down, true = up;
    bool canJump = true, canBounce = true;

    bool moveCameraLeft();
    bool moveCameraRight();

    unsigned char world[GAME_LENGTH][GAME_HEIGHT];
    unsigned char* buffer = new unsigned char[GAME_HEIGHT*GAME_WIDTH*4];
};

#endif // GAME_H
