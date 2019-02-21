#include <SFML/Graphics.hpp>
#include "../Extras/Window.h"
#include "../Extras/Constants.h"
#include "../Extras/Game.h"

using WorldArray = std::array<Game::GameType, GAME_HEIGHT*GAME_LENGTH>;
using BufferArray = std::array<Byte, GAME_HEIGHT*GAME_WIDTH*4>;

bool loadWorld(const IntType inLevel, WorldArray& world)
{
    sf::Image img;
    if(img.loadFromFile("./Levels/L" + std::to_string(inLevel) + ".bmp"))
    {
        for(IntType x = 0; x < GAME_LENGTH; x++)
        {
            for(IntType y = 0; y < GAME_HEIGHT; y++)
            {
                // Detect different game elements
                const sf::Color pixel = img.getPixel(x, y);
                
                Byte out = 0;
                     if(pixel.r > COLOR_THRESHOLD) { out |= 0b100; }
                else if(pixel.g > COLOR_THRESHOLD) { out |= 0b010; }
                else if(pixel.b > COLOR_THRESHOLD) { out |= 0b001; }
                world[x*GAME_HEIGHT + y] = static_cast<Game::GameType>(out);
            }
        }
        return false;
    } else { world = {}; return true; }

}

void saveWorld(const IntType inLevel, WorldArray& world)
{
    sf::Image img;
    img.create(GAME_LENGTH, GAME_HEIGHT);
    for(IntType x = 0; x < GAME_LENGTH; x++)
    {
        for(IntType y = 0; y < GAME_HEIGHT; y++)
        {
            // Detect different game elements
            const Game::GameType pixel = world[x*GAME_HEIGHT + y];
            
            sf::Color out = sf::Color(0,0,0,255);
            if(IntType(pixel) & 0b100) { out.r = 255; }
            if(IntType(pixel) & 0b010) { out.g = 255; }
            if(IntType(pixel) & 0b001) { out.b = 255; }
            img.setPixel(x, y, out);
        }
    }

    img.saveToFile("./Levels/L" + std::to_string(inLevel) + ".bmp");
}

void updateBuffer(BufferArray& buffer, const WorldArray& world, IntType cameraX, 
                  Game::GameType userItem, sf::Vector2i mousePos)
{
    for(IntType y = 0; y < GAME_HEIGHT; y++)
    {
        for(IntType x = 0; x < GAME_WIDTH; x++)
        {
            // Current Pixel
            Game::GameType gamePixel = world[(cameraX + x)*GAME_HEIGHT + y];

            // Mouse Pointer
            if(x + cameraX == mousePos.x && y == mousePos.y)
            { gamePixel = userItem; }

            // Values to feed into buffer
            int R, G, B;
            if(gamePixel == Game::GameType::Ground) // Ground
            {
                const int brightness = Game::randomize((cameraX + x) * (y + 1)) % 24 + 52;
                R = brightness; G = brightness; B = brightness;
            } else if(gamePixel == Game::GameType::Bounce) // Bounces
            {
                R = 64; 
                G = 255; 
                B = 164;
            } else if(gamePixel == Game::GameType::Trap) // Traps
            {
                const int brightness = Game::randomize((x+1)*(y+1))%32 - 16;
                R = 220 + brightness;
                G = 32 + brightness;
                B = 0;
            } else // Sky 
            {
                const IntType rVal[3] = // Random Values to Average
                {
                    Game::randomize((-((cameraX + 2)/3) - x)*(y+1))%8,
                    Game::randomize((-((cameraX + 1)/3) - x)*(y+1))%8,
                    Game::randomize((-((cameraX + 0)/3) - x)*(y+1))%8
                };
                const int brightness = (rVal[0] + rVal[1] + rVal[2])/3;
                R = 0   + brightness;
                G = 160 + brightness;
                B = 200 + brightness;
            }

            if(x + cameraX <= START_SIZE) { G += 64; }

            const std::size_t pixel = 4 * (y * GAME_WIDTH + x);  
            buffer[pixel + 0] = static_cast<Byte>(std::max(std::min(R, 255), 0)); // Red
            buffer[pixel + 1] = static_cast<Byte>(std::max(std::min(G, 255), 0)); // Green
            buffer[pixel + 2] = static_cast<Byte>(std::max(std::min(B, 255), 0)); // Blue
            buffer[pixel + 3] = 255; // Alpha
        }
    }
}

int main()
{
    sf::RenderWindow app(sf::VideoMode(GAME_WIDTH*GAME_SCALE, GAME_HEIGHT*GAME_SCALE), "Upside Down Level Editor");
    app.setFramerateLimit(48);

    // Font used for times
    sf::Font GameFont;
    const std::string ttfFile = "./src/Extras/GameFont.ttf";
    if(!GameFont.loadFromFile(ttfFile)) { return 0; }

    sf::Text SavedIcon;
    SavedIcon.setFont(GameFont);
    SavedIcon.setFillColor(sf::Color::White);
    SavedIcon.setOutlineColor(sf::Color::Black);
    SavedIcon.setOutlineThickness(2*TEXT_SCALE);
    SavedIcon.setCharacterSize(TEXT_SCALE*GAME_SCALE/1.5);
    SavedIcon.setScale(sf::Vector2f(1/TEXT_SCALE,1/TEXT_SCALE));
    SavedIcon.setPosition(6,GAME_SCALE*9);

    sf::Text Help = SavedIcon;
    Help.setString( 
        "\nA + D = Move Camera" 
        "\nCtrl+S = Save"  
        "\nEscape = Revert to last save"
        "\nArrows = Change Level" 
        "\nScroll = Change Block"  
        "\nLeft Click = Place Block"
    );

    WorldArray world;
    BufferArray buffer;
    IntType blinkCounter = 0;
    IntType level = 0;
    IntType cameraX = 0;
    IntType item = 0;
    bool edits = false;

    const Game::GameType peices[4] = 
    {
        Game::GameType::Sky, 
        Game::GameType::Ground, 
        Game::GameType::Trap, 
        Game::GameType::Bounce 
    };

    loadWorld(level, world);

    sf::Vector2i mouseCoords(0,0);
    while (app.isOpen())
    {
        // Game Events
        sf::Event event;
        while (app.pollEvent(event))
        {
            // Close window : exit
            if (event.type == sf::Event::Closed)
                app.close();
            else if(event.type == sf::Event::MouseWheelScrolled)
            {
                IntType scroll = event.mouseWheelScroll.y;
                if(scroll > 0) 
                {
                    if(item == 0) item = 3;
                    else --item;
                } else {
                    if(item == 3) item = 0;
                    else ++item;
                }
            }
        }

        // Moving Camera
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            if(cameraX > 0)
            { --cameraX; }
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        { 
            if(cameraX < GAME_LENGTH - GAME_WIDTH)
            { ++cameraX; } 
        } 

        // Reverting
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        { 
            edits = loadWorld(level, world); 
        }

        // Change Worlds
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && level > 0)
        { 
            cameraX = 0;
            --level;
            edits = loadWorld(level, world); 
            while(sf::Keyboard::isKeyPressed(sf::Keyboard::Left));
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && level < MAX_LEVEL_COUNT - 1)
        { 
            cameraX = 0;
            ++level;
            edits = loadWorld(level, world); 
            while(sf::Keyboard::isKeyPressed(sf::Keyboard::Right));
        }

        // Saving 
        if(!edits)
        {
            SavedIcon.setString("(Level " + std::to_string(level) + " Saved!)");
            SavedIcon.setFillColor(sf::Color::Green);
        } else {
            SavedIcon.setString("(Level " + std::to_string(level) + " Not Saved!)");
            SavedIcon.setFillColor(sf::Color::Red);

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
            {
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                {
                    saveWorld(level, world);
                    edits = false;
                }
            }
        }

        mouseCoords = sf::Mouse::getPosition(app);
        mouseCoords.x /= app.getSize().x/double(GAME_WIDTH);
        mouseCoords.y /= app.getSize().y/double(GAME_HEIGHT);
        mouseCoords.x += cameraX;

        // Mouse and Updating screen
        if(sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            edits |= world[mouseCoords.x*GAME_HEIGHT + mouseCoords.y] != Game::GameType::Sky;
            world[mouseCoords.x*GAME_HEIGHT + mouseCoords.y] = Game::GameType::Sky;
            updateBuffer(buffer, world, cameraX, Game::GameType::Sky, mouseCoords);
        } 
        else if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            edits |= world[mouseCoords.x*GAME_HEIGHT + mouseCoords.y] != peices[item];
            world[mouseCoords.x*GAME_HEIGHT + mouseCoords.y] = peices[item];
            updateBuffer(buffer, world, cameraX, peices[item], mouseCoords);
        } else {
            updateBuffer(buffer, world, cameraX, peices[item], mouseCoords);
        }

        // Draw World
        Graphics::pushRGBA(app, buffer.data());

        // Draw Text
        app.draw(SavedIcon);
        app.draw(Help);

        // Update Title
        if(level == 0) app.setTitle("Upside Down Level Editor (End Level)");
        else app.setTitle("Upside Down Level Editor (Level " + std::to_string(level) + ")");

        // Show To User
        app.display();
    }
}