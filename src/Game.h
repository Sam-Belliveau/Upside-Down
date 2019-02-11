#include "Constants.h"
#include <string>
#include <SFML/Graphics.hpp>

#ifndef GAME_H
#define GAME_H

enum type{ sky, ground, comment, trap};

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
        int playerX = 5;
        int playerY = 18;
        unsigned int cX = 0;
        bool gravity = false; // false = down, true = up;

        bool canJump = true;

        bool glow = true;
        unsigned char redValue = 224;

        bool moveCameraLeft();
        bool moveCameraRight();

        unsigned char world[GAME_LENGTH][GAME_HEIGHT];
        unsigned char* buffer = new unsigned char[GAME_HEIGHT*GAME_WIDTH*4];
};

#endif // GAME_H
