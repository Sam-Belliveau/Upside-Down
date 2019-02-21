#include "./Extras/Window.h"
#include "./Extras/Constants.h"
#include "./Extras/Game.h"

int main()
{
    sf::RenderWindow app(sf::VideoMode(GAME_WIDTH*GAME_SCALE, GAME_HEIGHT*GAME_SCALE), "Upside Down");
    app.setFramerateLimit(GAME_FPS);

    // Font used for times
    sf::Font GameFont;
    const std::string ttfFile = "./src/Extras/GameFont.ttf";
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
    mainGame.loadWorld(START_LEVEL);

    IntType lastLevel = 0;
    while (app.isOpen())
    {
        Graphics::checkEvents(app);

        mainGame.gameLoop();

        Graphics::pushRGBA(app, mainGame.returnWorldPixels());

        // Leader Board
        if(mainGame.getCameraX() < START_SIZE 
        && mainGame.getLevel() != lastLevel 
        && mainGame.getLevel() != START_LEVEL)
        {
            // Setup string buffer
            std::ostringstream timesStream;
            timesStream << std::setprecision(2) << std::fixed;
            
            if(mainGame.getCheater())
            { timesStream << "Cheated Times:\n"; } 
            else { timesStream << "Times:\n"; }

            if(mainGame.getWinner())
            {
                for(std::size_t i = std::max(lastLevel - 10, IntType(1)); i <= lastLevel; ++i)
                {
                    // Balance Level Sign
                    timesStream << "Level " << i << ": ";
                    if(i < 10) { timesStream << " "; }
                    
                    // Calculate Time
                    timesStream << double(mainGame.getLevelFrame(i)) / double(GAME_FPS) << "s\n";
                }
            } else 
            {
                for(std::size_t i = std::max(mainGame.getLevel() - 10, IntType(1)); 
                    i < mainGame.getLevel(); ++i)
                {
                    // Balance Level Sign
                    timesStream << "Level " << i << ": ";
                    if(i < 10) { timesStream << " "; }
                    
                    // Calculate Time
                    timesStream << double(mainGame.getLevelFrame(i)) / double(GAME_FPS) << "s\n";
                }
            }

            // Optimization, don't build the string over and over again
            lastLevel = mainGame.getLevel();
            
            // Print times to leader board
            times.setString(timesStream.str());
        } 

        // Leaderboard for times
        if(mainGame.getWinner())
        {
            times.setScale(sf::Vector2f(1.25/TEXT_SCALE,1.25/TEXT_SCALE));
            times.setPosition(GAME_SCALE * (GAME_WIDTH/2 - mainGame.getCameraX()), GAME_SCALE * 4);
            app.draw(times);
        } else 
        {
            times.setScale(sf::Vector2f(1/TEXT_SCALE,1/TEXT_SCALE));
            times.setPosition(GAME_SCALE * (1 - mainGame.getCameraX()), GAME_SCALE * 9);
            app.draw(times);
        }

        // Draw current time in top left corner
        // Cheating Indicator
        if(mainGame.getCheater())
        { 
            times.setFillColor(sf::Color::Red);
            currentTime.setFillColor(sf::Color::Red);
            currentTime.setString(
                "(CHEATS USED) " + 
                (std::to_string(double(mainGame.getFrame()) / double(GAME_FPS)).substr(0,7)) +
                "s"
            );
        } else {
            times.setFillColor(sf::Color::White);
            currentTime.setFillColor(sf::Color::White);
            currentTime.setString(
                std::to_string(double(mainGame.getFrame()) / double(GAME_FPS)).substr(0,7) + 
                "s"
            );
        }
        app.draw(currentTime);
        
        app.display();
    }

    return EXIT_SUCCESS;
}
