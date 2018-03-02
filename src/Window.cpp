#include "Window.h"

void Graphics::checkEvents(sf::RenderWindow& app)
{
    sf::Event event;
    while (app.pollEvent(event))
    {
        // Close window : exit
        if (event.type == sf::Event::Closed)
            app.close();
    }
}

void Graphics::pushRGBA(sf::RenderWindow& app, const unsigned char* pixels)
{
    app.clear();

    sf::Texture buffer;
    buffer.create(GAME_WIDTH, GAME_HEIGHT);
    buffer.update(pixels);

    sf::Sprite temp(buffer);
    temp.setScale(GAME_SCALE,GAME_SCALE);

    app.draw(temp);
    app.display();
}
