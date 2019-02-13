#include <SFML/Graphics.hpp>
#include "Window.h"
#include "Constants.h"
#include "Game.h"

int main()
{
    sf::RenderWindow app(sf::VideoMode(GAME_WIDTH*GAME_SCALE, GAME_HEIGHT*GAME_SCALE), "Upside Down");
    Game mainGame;

    unsigned long long frame = 0;

    mainGame.loadWorld(0);

    app.setFramerateLimit(30);
    while (app.isOpen())
    {
        Graphics::checkEvents(app);

        mainGame.gameLoop();

        Graphics::pushRGBA(app, mainGame.returnPixels());

        frame++;
    }

    return EXIT_SUCCESS;
}
