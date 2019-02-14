#include <SFML/Graphics.hpp>
#include "Window.h"
#include "Constants.h"
#include "Game.h"

int main()
{
    sf::RenderWindow app(sf::VideoMode(GAME_WIDTH*GAME_SCALE, GAME_HEIGHT*GAME_SCALE), "Upside Down");
    Game mainGame;

    mainGame.loadWorld(0);

    app.setFramerateLimit(GAME_FPS);
    while (app.isOpen())
    {
        Graphics::checkEvents(app);

        mainGame.gameLoop();

        Graphics::pushRGBA(app, mainGame.returnPixels());

        // Set title to current time
        app.setTitle
            ("Upside Down (" 
            + std::to_string(double(mainGame.frame) / double(GAME_FPS)) 
            + "s)");
    }

    return EXIT_SUCCESS;
}
