#ifndef WINDOW_H
#define WINDOW_H

#include "Constants.h"

namespace Graphics
{
    // Pass render window and have it display events
    void checkEvents(sf::RenderWindow& app)
    {
        sf::Event event;
        while (app.pollEvent(event))
        {
            // Close window : exit
            if (event.type == sf::Event::Closed)
                app.close();
        }
    }

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
