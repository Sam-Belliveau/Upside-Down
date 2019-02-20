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
    times.setFillColor(sf::Color::White);
    times.setOutlineColor(sf::Color::Black);
    times.setCharacterSize(TEXT_SCALE*GAME_SCALE/1.25);
    times.setScale(sf::Vector2f(1/TEXT_SCALE,1/TEXT_SCALE));
    times.setOutlineThickness(TEXT_SCALE * 3);

    sf::Text currentTime = times;
    currentTime.setPosition(6, 0);

    Game mainGame;
    mainGame.loadWorld(0);

    IntType lastLevel = 0;
    while (app.isOpen())
    {
        Graphics::checkEvents(app);

        mainGame.gameLoop();

        Graphics::pushRGBA(app, mainGame.returnWorldPixels());

        // Leader Board
        if(mainGame.getCameraX() < START_SIZE && mainGame.getLevel() != lastLevel)
        {
            // Optimization, don't remake the string over and over again
            lastLevel = mainGame.getLevel();

            // Setup string buffer
            std::ostringstream timesStream;
            timesStream << std::setprecision(2) << std::fixed;

            for(std::size_t i = std::max(mainGame.getLevel() - 10, IntType(1)); 
                i < mainGame.getLevel(); ++i)
            {
                // Balance Level Sign
                timesStream << "Level " << i+1 << ": ";
                if(i < 9) { timesStream << " "; }
                
                // Calculate Time
                timesStream << double(mainGame.getLevelFrame(i)) / double(GAME_FPS) << "s\n";
            }

            // Print times to leader board
            times.setString(timesStream.str());
        } 

        // Leaderboard for times
        times.setPosition(IntType(GAME_SCALE * (1.25 - mainGame.getCameraX())), IntType(GAME_SCALE * 9));
        app.draw(times);

        // Draw current time in top left corner
        currentTime.setString(std::to_string(double(mainGame.getFrame()) / double(GAME_FPS)) + "s");
        app.draw(currentTime);
        
        app.display();
    }

    return EXIT_SUCCESS;
}
