#include "./Extras/Constants.h"
#include "./Extras/Window.h"
#include "./Extras/Game.h"
#include "./Extras/LevelBuilder.h"
#include "./Extras/TextTimes.h"

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
    game.loadWorld(START_LEVEL);

    while (app.isOpen())
    {
        Graphics::checkEvents(app);

        game.gameLoop();

        Graphics::pushRGBA(app, game.returnWorldPixels());

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
