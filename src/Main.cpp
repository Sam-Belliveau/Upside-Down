#include "./Headers/Constants.h"
#include "./Headers/Window.h"
#include "./Headers/Game.h"
#include "./Headers/LevelBuilder.h"
#include "./Headers/TextTimes.h"

int main()
{
    // Game Window
    sf::RenderWindow app(sf::VideoMode(GAME_WIDTH*GAME_SCALE, GAME_HEIGHT*GAME_SCALE), "Upside Down");
    app.setFramerateLimit(GAME_FPS);

    // Text class used to display times
    sf::Text leaderboard = GET_DEFAULT_TEXT();
    sf::Text timer = GET_DEFAULT_TEXT();
    timer.setPosition(6, 0);

    Game game;
    bool focus = true;
    game.loadWorld(START_LEVEL);

    while (app.isOpen())
    {
        sf::Event event;
        while (app.pollEvent(event))
        {
            // Close window : exit
            if (event.type == sf::Event::Closed) app.close();
            if (event.type == sf::Event::GainedFocus) focus = true;
            if (event.type == sf::Event::LostFocus) focus = false;
        }

        if(focus) game.gameLoop();

        Graphics::pushRGBA(app, game.returnWorldPixels(focus));

        if(Game::editorCheatKey())
        {
            game.setCheater();
            game.loadWorld(LevelBuilder::Loop(app, game.getLevel(), game.getCameraX()));
            while(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape));
        }

        TextTimes::UpdateLeaderboard(game, leaderboard);
        TextTimes::UpdateTimer(game, timer);
        app.draw(leaderboard);
        app.draw(timer);
        
        app.display();
    }

    return EXIT_SUCCESS;
}
