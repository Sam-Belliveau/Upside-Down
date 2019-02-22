#ifndef LEVEL_BUILDER_H
#define LEVEL_BUILDER_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include "./Window.h"
#include "./Constants.h"
#include "./Game.h"

namespace LevelBuilder
{
    static bool loadWorld(const IntType inLevel, Game::GameType world[][GAME_HEIGHT])
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
                    if(pixel.g > COLOR_THRESHOLD) { out |= 0b010; }
                    if(pixel.b > COLOR_THRESHOLD) { out |= 0b001; }
                    world[x][y] = static_cast<Game::GameType>(out);
                }
            }
            return false;
        } else 
        {
            for(IntType x = 0; x < GAME_LENGTH; ++x)
                for(IntType y = 0; y < GAME_HEIGHT; ++y)
                    if(y < GAME_HEIGHT-3) world[x][y] = Game::GameType::Sky;
                    else world[x][y] = Game::GameType::Sky;
            return true; 
        }

    }

    static void saveWorld(const IntType inLevel, Game::GameType world[][GAME_HEIGHT])
    {
        sf::Image img;
        img.create(GAME_LENGTH, GAME_HEIGHT);
        for(IntType x = 0; x < GAME_LENGTH; x++)
        {
            for(IntType y = 0; y < GAME_HEIGHT; y++)
            {
                sf::Color out = sf::Color(0,0,0,255);
                if(IntType(world[x][y]) & 0b100) { out.r = 255; }
                if(IntType(world[x][y]) & 0b010) { out.g = 255; }
                if(IntType(world[x][y]) & 0b001) { out.b = 255; }
                img.setPixel(x, y, out);
            }
        }

        img.saveToFile("./Levels/L" + std::to_string(inLevel) + ".bmp");
    }

    static void updateBuffer(Byte buffer[][GAME_WIDTH][4], const Game::GameType world[][GAME_HEIGHT], 
        const IntType cameraX, const Game::GameType userItem, const sf::Vector2i mousePos)
    {
        for(IntType y = 0; y < GAME_HEIGHT; y++)
        {
            for(IntType x = 0; x < GAME_WIDTH; x++)
            {
                // Current Pixel
                Game::GameType gamePixel = world[cameraX + x][y];

                // Mouse Pointer
                if(x + cameraX == mousePos.x && y == mousePos.y)
                { gamePixel = userItem; }

                // Current Pixel
                const Game::GameTypeData pixelData = Game::GetGameTypeData(gamePixel);

                // Values to feed into buffer
                IntType R = pixelData.color.r; 
                IntType G = pixelData.color.g; 
                IntType B = pixelData.color.b;
                
                // Randomize Color
                IntType random = pixelData.randomize(cameraX, x, y);
                R += random; G += random; B += random;

                // Start Area
                if(x + cameraX <= START_SIZE) G += 48;
                
                // Mouse Pointer Highlight
                if(x + cameraX == mousePos.x && y == mousePos.y)
                { R += 32; G += 32; B += 32; }

                // Cap RGB Values
                buffer[y][x][0] = static_cast<Byte>(std::max(std::min(R, IntType(255)), IntType(0))); // Red
                buffer[y][x][1] = static_cast<Byte>(std::max(std::min(G, IntType(255)), IntType(0))); // Green
                buffer[y][x][2] = static_cast<Byte>(std::max(std::min(B, IntType(255)), IntType(0))); // Blue
                buffer[y][x][3] = 255; // Alpha
            }
        }
    }

    static IntType Loop(sf::RenderWindow &app, IntType level, IntType cameraX)
    {
        app.setFramerateLimit(36);
        sf::Text SavedIcon = GET_DEFAULT_TEXT();
        SavedIcon.setPosition((GAME_WIDTH-12)*GAME_SCALE,GAME_SCALE*16);

        sf::Text Help = SavedIcon;
        Help.setString( 
            "\n      Escape = Goto Game"
            "\n    Ctrl + S = Save Level"  
            "\n    Ctrl + Z = Undo changes"
            "\n   Up + Down = Change Block"
            "\n  Left Click = Place Block"
            "\nLeft + Right = Move Camera" 
            "\nCtrl + L + R = Change Level"    
        );

        sf::Text Block = SavedIcon;
        Block.setPosition((GAME_WIDTH-8)*GAME_SCALE,GAME_SCALE*22.25);
        Block.setScale(sf::Vector2f(1.5/TEXT_SCALE, 1.5/TEXT_SCALE));

        Game::GameType world[GAME_LENGTH][GAME_HEIGHT] = {};
        Byte buffer[GAME_HEIGHT][GAME_WIDTH][4] = {};
        IntType item = 0;
        bool edits = false;

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

                // Scroll switching blocks
                else if(event.type == sf::Event::MouseWheelScrolled)
                {
                    if(event.mouseWheelScroll.delta > 0) --item;
                    else ++item;
                } 
                
                // Scroll switching blocks with up and down
                else if(event.type == sf::Event::KeyPressed)
                {            
                    if(Game::upKey()) --item;
                    else if(Game::downKey() && !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) 
                        ++item;
                    
                }
            }

            // Exiting
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) && !edits)
            { break; }

            // Loop Items
            item += Game::GameTypeCount;
            item %= Game::GameTypeCount;

            // Indicate Item
            Block.setString(Game::GameTypeList[item].data.name);
            Block.setFillColor(Game::GameTypeList[item].data.color);

            // Reverting
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)
            && sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
            {
                edits = false;
                loadWorld(level, world); 
            }

            // Change Worlds / Moving Camera
            if(Game::leftKey())
            {
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                {
                    if(!edits && level > 0)
                    { 
                        cameraX = 0; --level;
                        edits = false;
                        loadWorld(level, world); 
                        while(Game::leftKey());
                    }
                } else {
                    if(cameraX > 0) --cameraX; 
                }
            }

            // Change Worlds / Moving Camera
            if(Game::rightKey())
            {
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                {
                    if(!edits && level < MAX_LEVEL_COUNT-1)
                    { 
                        cameraX = 0; ++level;
                        edits = false;
                        loadWorld(level, world); 
                        while(Game::rightKey());
                    }
                } else {    
                    if(cameraX < GAME_LENGTH - GAME_WIDTH) ++cameraX; 
                }
            }

            // Saving 
            if(!edits)
            {
                if(level == 0) SavedIcon.setString("      (End Level Saved)");
                else SavedIcon.setString("      (Level " + std::to_string(level) + " Saved)");
                SavedIcon.setFillColor(sf::Color::Green);
            } else {
                if(level == 0) SavedIcon.setString("      (End Level Not Saved)");
                else SavedIcon.setString("      (Level " + std::to_string(level) + " Not Saved)");
                SavedIcon.setFillColor(sf::Color::Red);

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                    {
                        saveWorld(level, world);
                        edits = false;
                    }
            }

            // Calculate mouse pixel
            mouseCoords = sf::Mouse::getPosition(app);
            mouseCoords.x /= app.getSize().x/double(GAME_WIDTH);
            mouseCoords.y /= app.getSize().y/double(GAME_HEIGHT);
            mouseCoords.x += cameraX;

            // Mouse and Updating screen
            if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                if(sf::Mouse::getPosition(app).x >= 0 
                && sf::Mouse::getPosition(app).x < app.getSize().x)
                {
                    if(sf::Mouse::getPosition(app).y >= 0 
                    && sf::Mouse::getPosition(app).y < app.getSize().y)
                    {
                        edits |= world[mouseCoords.x][mouseCoords.y] != Game::GameTypeList[item].type;
                        world[mouseCoords.x][mouseCoords.y] = Game::GameTypeList[item].type;
                    }
                }
            } 

            // Draw World
            updateBuffer(buffer, world, cameraX, Game::GameTypeList[item].type, mouseCoords);
            Graphics::pushRGBA(app, reinterpret_cast<const Byte*>(buffer));

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

        app.setTitle("Upside Down");
        app.setFramerateLimit(GAME_FPS);
        return level;
    }
}

#endif