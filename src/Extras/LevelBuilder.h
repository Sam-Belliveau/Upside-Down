#ifndef LEVEL_BUILDER_H
#define LEVEL_BUILDER_H

#include <SFML/Graphics.hpp>
#include "./Window.h"
#include "./Constants.h"
#include "./Game.h"

namespace LevelBuilder
{
    static bool loadWorld(const IntType inLevel, Game::GameType* world)
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
        } else 
        {
            for(IntType i = 0; i < GAME_HEIGHT*GAME_LENGTH; ++i)
                world[i] = Game::GameType::Sky;
            return true; 
        }

    }

    static void saveWorld(const IntType inLevel, Game::GameType* world)
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

    static void updateBuffer(Byte* buffer, const Game::GameType* world, IntType cameraX, 
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
                    const int brightness = Game::randomize((-((cameraX)/3) - x)*(y+1))%8;
                    R = 0   + brightness;
                    G = 160 + brightness;
                    B = 200 + brightness;
                }

                if(x + cameraX <= START_SIZE) { G += 48; }

                // Mouse Pointer
                if(x + cameraX == mousePos.x && y == mousePos.y)
                { R += 24; G += 24; B += 24; }

                const std::size_t pixel = 4 * (y * GAME_WIDTH + x);  
                buffer[pixel + 0] = static_cast<Byte>(std::max(std::min(R, 255), 0)); // Red
                buffer[pixel + 1] = static_cast<Byte>(std::max(std::min(G, 255), 0)); // Green
                buffer[pixel + 2] = static_cast<Byte>(std::max(std::min(B, 255), 0)); // Blue
                buffer[pixel + 3] = 255; // Alpha
            }
        }
    }

    static IntType Loop(sf::RenderWindow &app, IntType level, IntType cameraX)
    {
        app.setFramerateLimit(36);

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
        SavedIcon.setPosition((GAME_WIDTH-12)*GAME_SCALE,GAME_SCALE*16);

        sf::Text Help = SavedIcon;
        Help.setString( 
            "\n      Escape = Leave Editor"
            "\n    Ctrl + S = Save Level"  
            "\n    Ctrl + Z = Undo changes"
            "\n   Up + Down = Change Block"
            "\n  Left Click = Place Block"
            "\nLeft + Right = Move Camera" 
            "\nCtrl + Arrow = Change Level"    
        );

        sf::Text Block = SavedIcon;
        Block.setCharacterSize(TEXT_SCALE*GAME_SCALE*1.125);

        Game::GameType world[GAME_HEIGHT*GAME_LENGTH] = {};
        Byte buffer[GAME_HEIGHT*GAME_WIDTH*4] = {};
        IntType blinkCounter = 0;
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
        while (app.isOpen() && (!sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) || edits))
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

            // Keyboard switching blocks
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                if(item == 0) item = 3;
                else --item;
                while(sf::Keyboard::isKeyPressed(sf::Keyboard::Up));
            } else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                if(item == 3) item = 0;
                else ++item;
                while(sf::Keyboard::isKeyPressed(sf::Keyboard::Down));
            }

            // Block Text
            switch (item)
            {
                case 0:
                    Block.setString("\n\n\n\n\n      (Sky)");
                    Block.setFillColor(sf::Color(0,160,255));
                    break;
                case 1:
                    Block.setString("\n\n\n\n\n    (Ground)");
                    Block.setFillColor(sf::Color(64,64,64));
                    break;
                case 2:
                    Block.setString("\n\n\n\n\n     (Trap)");
                    Block.setFillColor(sf::Color(220,32,0));
                    break;
                case 3:
                    Block.setString("\n\n\n\n\n    (Bouncer)");
                    Block.setFillColor(sf::Color(64,255,164));
                    break;
                default:
                    break;
            }

            // Reverting
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)
            && sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
            {
                edits = false;
                loadWorld(level, world); 
            }

            // Change Worlds / Moving Camera
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            {
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                {
                    if(!edits && level > 0)
                    { 
                        cameraX = 0; --level;
                        edits = false;
                        loadWorld(level, world); 
                        while(sf::Keyboard::isKeyPressed(sf::Keyboard::Left));
                    }
                } else {
                    if(cameraX > 0)
                    { --cameraX; }
                }
            }

            // Change Worlds / Moving Camera
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                {
                    if(!edits && level < MAX_LEVEL_COUNT-1)
                    { 
                        cameraX = 0; ++level;
                        edits = false;
                        loadWorld(level, world); 
                        while(sf::Keyboard::isKeyPressed(sf::Keyboard::Right));
                    }
                } else {    
                    if(cameraX < GAME_LENGTH - GAME_WIDTH)
                    { ++cameraX; } 
                }
            }


            // Saving 
            if(!edits)
            {
                if(level == 0) SavedIcon.setString("      (End Level Saved!)");
                else SavedIcon.setString("      (Level " + std::to_string(level) + " Saved!)");
                SavedIcon.setFillColor(sf::Color::Green);
            } else {
                if(level == 0) SavedIcon.setString("      (End Level Not Saved!)");
                else SavedIcon.setString("      (Level " + std::to_string(level) + " Not Saved!)");
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
            
            if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                if(mouseCoords.x - cameraX >= 0 && mouseCoords.x - cameraX < GAME_WIDTH)
                {
                    if(mouseCoords.y >= 0 && mouseCoords.y < GAME_HEIGHT)
                    {
                        edits |= world[mouseCoords.x*GAME_HEIGHT + mouseCoords.y] != peices[item];
                        world[mouseCoords.x*GAME_HEIGHT + mouseCoords.y] = peices[item];
                    }
                }
            } 

            // Draw World
            updateBuffer(buffer, world, cameraX, peices[item], mouseCoords);
            Graphics::pushRGBA(app, buffer);

            // Draw Text
            app.draw(SavedIcon);
            app.draw(Help);
            app.draw(Block);

            // Update Title
            if(level == 0) app.setTitle("Upside Down Level Editor (End Level)");
            else app.setTitle("Upside Down Level Editor (Level " + std::to_string(level) + ")");

            // Show To User
            app.display();
        }

        app.setFramerateLimit(GAME_FPS);
        return level;
    }
}

#endif