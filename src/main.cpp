#include <iostream>
#include <sstream>
#include <iomanip>
#include <SFML/Graphics.hpp>
#include "Window.h"
#include "Constants.h"
#include "Game.h"

int main()
{
    sf::RenderWindow app(sf::VideoMode(GAME_WIDTH*GAME_SCALE, GAME_HEIGHT*GAME_SCALE), "Upside Down");
    app.setFramerateLimit(GAME_FPS);

    // Font used for times
    sf::Font consolas;
    const std::string ttfFile = "./src/Consolas.ttf";
    if(!consolas.loadFromFile(ttfFile))
    {
        std::cout << "Error loading \"" << ttfFile << "\", please double check on file and try again!\n";
    }

    // Text class used to display times
    sf::Text times;
    times.setFont(consolas);
    times.setCharacterSize(GAME_SCALE / 1.2);
    times.setFillColor(sf::Color(255, 255, 255, 255));
    times.setOutlineColor(sf::Color(0, 0, 0, 255));
    times.setOutlineThickness(GAME_SCALE/12.0);
    times.setPosition(0, app.getSize().y/2);

    Game mainGame;
    mainGame.loadWorld(0);

    while (app.isOpen())
    {
        Graphics::checkEvents(app);

        mainGame.gameLoop();

        Graphics::pushRGBA(app, mainGame.returnPixels());

        std::ostringstream timesStream;
        timesStream << std::setprecision(3) << std::fixed;
        for(std::size_t i = 0; i < LEVEL_COUNT; ++i)
        {
            timesStream << " Level " << i+1 << ": ";
            if(i < 9) { timesStream << " "; }
            timesStream << double(mainGame.levelFrames[i]) / double(GAME_FPS) << "s\n";
        }
        times.setString(timesStream.str());
        app.draw(times);

        app.display();
    }

    return EXIT_SUCCESS;
}
