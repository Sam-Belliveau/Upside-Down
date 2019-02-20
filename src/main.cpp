#include <sstream>
#include <iomanip>
#include <cmath>
#include <SFML/Graphics.hpp>
#include "Window.h"
#include "Constants.h"
#include "Game.h"

int main()
{
    sf::RenderWindow app(sf::VideoMode(GAME_WIDTH*GAME_SCALE, GAME_HEIGHT*GAME_SCALE), "Upside Down");
    app.setFramerateLimit(GAME_FPS);

    // Font used for times
    sf::Font GameFont;
    const std::string ttfFile = "./src/GameFont.ttf";
    if(!GameFont.loadFromFile(ttfFile)) { return 0; }

    // Text class used to display times
    sf::Text times;
    times.setFont(GameFont);
    times.setCharacterSize(GAME_SCALE/1.25);
    times.setFillColor(sf::Color::White);
    times.setOutlineColor(sf::Color::Black);
    times.setOutlineThickness(3);
    sf::Text currentTime = times;

    Game mainGame;
    mainGame.loadWorld(0);

    while (app.isOpen())
    {
        Graphics::checkEvents(app);

        mainGame.gameLoop();

        Graphics::pushRGBA(app, mainGame.returnPixels());

        if(mainGame.cX < START_SIZE)
        {
            // Setup string buffer
            std::ostringstream timesStream;
            timesStream << std::setprecision(2) << std::fixed;

            for(std::size_t i = 0; i < mainGame.level; ++i)
            {
                // Balance Level Sign
                timesStream << " Level " << i+1 << ": ";
                if(i < 9) { timesStream << " "; }
                
                // Calculate Time
                timesStream << double(mainGame.levelFrames[i]) / double(GAME_FPS) << "s\n";
            }

            // Print times to leader board
            times.setString(timesStream.str());
            times.setPosition(GAME_SCALE * (1.25 - mainGame.cX), GAME_SCALE * 9);
            app.draw(times);
        } 

        // Draw current time in top left corner
        currentTime.setString(" " + std::to_string(double(mainGame.frame) / double(GAME_FPS)) + "s");
        app.draw(currentTime);
        

        app.display();
    }

    return EXIT_SUCCESS;
}
