#ifndef WINDOW_H
#define WINDOW_H

#include "Constants.h"

namespace Graphics
{
    void pushRGBA(sf::RenderWindow& app, const Byte* pixels)
    {
        app.clear();

        sf::Texture buffer;
        buffer.create(GAME_WIDTH, GAME_HEIGHT);
        buffer.update(pixels);

        sf::Sprite temp(buffer);
        temp.setScale(GAME_SCALE,GAME_SCALE);

        app.draw(temp);
    }
};

#endif // WINDOW_H
