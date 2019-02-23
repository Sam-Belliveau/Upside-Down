#ifndef LEVEL_BUILDER_H
#define LEVEL_BUILDER_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include "./Window.h"
#include "./Constants.h"
#include "./Game.h"
#include "./FileLoader.h"

namespace LevelBuilder
{
    static void updateBuffer(Byte buffer[][GAME_WIDTH][4], const GameType world[][GAME_HEIGHT], 
                             IntType cameraX, GameType userItem, sf::Vector2i mousePos)
    {
        for(IntType y = 0; y < GAME_HEIGHT; y++)
        {
            for(IntType x = 0; x < GAME_WIDTH; x++)
            {
                // Current Pixel
                GameType gamePixel = world[cameraX + x][y];

                // Mouse Pointer
                if(x + cameraX == mousePos.x && y == mousePos.y)
                { gamePixel = userItem; }

                // Current Pixel
                const Game::GameTypeData pixelData = Game::GetTypeData(gamePixel);

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

    static IntType loopTypeIndex(IntType index)
    {
        while(index < 0) index += GameTypeCount;
        index %= GameTypeCount;
        return index;
    }

    struct UndoData
    {
        sf::Vector2i pos;
        GameType oldBlock;
    };

    static IntType Loop(sf::RenderWindow &app, IntType level, IntType cameraX)
    {
        app.setFramerateLimit(60);
        sf::Text SavedIcon = GET_DEFAULT_TEXT();
        SavedIcon.setPosition((GAME_WIDTH-11)*GAME_SCALE,GAME_SCALE*14);

        sf::Text Help = SavedIcon;
        Help.setPosition((GAME_WIDTH-14)*GAME_SCALE,GAME_SCALE*14);
        Help.setString( 
            "\n             Escape = Exit Editor"
            "\n      Ctrl + Escape = Force Exit"
            "\n           Ctrl + S = Save Level"  
            "\n           Ctrl + Z = Undo"
            "\n   Ctrl + Shift + Z = Revert To Save"
            "\n          Up + Down = Change Block"
            "\n         Left Click = Place Block"
            "\n       Left + Right = Move Camera" 
            "\nCtrl + Left + Right = Change Level"    
        );

        sf::Text Block = SavedIcon;
        Block.setPosition(GAME_SCALE / 2,GAME_SCALE * (GAME_HEIGHT - 3));
        Block.setScale(sf::Vector2f(2 / TEXT_SCALE, 2 / TEXT_SCALE));

        sf::Text BlockDown = Block;
        BlockDown.setPosition(GAME_SCALE / 2, GAME_SCALE * (GAME_HEIGHT - 1));
        BlockDown.setScale(sf::Vector2f(1/TEXT_SCALE, 1/TEXT_SCALE));

        sf::Text BlocksUp[BLOCK_LIST_SIZE];
        for(IntType i = 0; i < BLOCK_LIST_SIZE; ++i)
        {
            BlocksUp[i] = BlockDown;
            BlocksUp[i].setPosition(GAME_SCALE / 2, GAME_SCALE * (GAME_HEIGHT - (4 + i)));
        }

        std::stack<UndoData> undoList;
        GameType world[GAME_LENGTH][GAME_HEIGHT] = {};
        Byte buffer[GAME_HEIGHT][GAME_WIDTH][4] = {};
        IntType item = 0;
        IntType frame = 0;
        bool edits = false;

        Loader::LoadWorld(level, world);

        sf::Vector2i mouse(0,0);
        while (app.isOpen())
        {
            // Frame Counter
            ++frame;

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
                
                // Buttons which are count sensitive
                else if(event.type == sf::Event::KeyPressed)
                {            
                    if(Game::upKey()) --item;
                    else if(Game::downKey() // Save has the same key press
                    && !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) 
                        ++item;

                    // Undoing
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)
                    && sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
                    {
                        if(!undoList.empty())
                        {
                            const sf::Vector2i pos = undoList.top().pos;
                            world[pos.x][pos.y] = undoList.top().oldBlock;
                            undoList.pop();

                            edits = true;
                        }
                    }
                }
            }

            // Exiting
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) && !edits)
            {  break; }

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) 
            && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
            { break; }

            // Loop Items
            item = loopTypeIndex(item);

            // Indicate Item
            for(IntType i = 0; i < BLOCK_LIST_SIZE; ++i)
            {
                BlocksUp[i].setString(Game::GameTypeList[loopTypeIndex(item - (i + 1))].data.name);
                BlocksUp[i].setFillColor(Game::GameTypeList[loopTypeIndex(item - (i + 1))].data.color);
            }

            Block.setString(Game::GameTypeList[loopTypeIndex(item - 0)].data.name);
            Block.setFillColor(Game::GameTypeList[loopTypeIndex(item - 0)].data.color);
            BlockDown.setString(Game::GameTypeList[loopTypeIndex(item + 1)].data.name);
            BlockDown.setFillColor(Game::GameTypeList[loopTypeIndex(item + 1)].data.color);

            // Reverting
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)
            && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)
            && sf::Keyboard::isKeyPressed(sf::Keyboard::Z)
            && edits)
            {
                edits = false;
                undoList = std::stack<UndoData>();
                Loader::LoadWorld(level, world); 
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
                        Loader::LoadWorld(level, world); 
                        while(Game::leftKey());
                    }
                } else {
                    if(cameraX > 0 
                    && frame % EDITOR_CAMERA_SPEED == 0) 
                        --cameraX; 
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
                        Loader::LoadWorld(level, world); 
                        while(Game::rightKey());
                    }
                } else {    
                    if(cameraX < GAME_LENGTH - GAME_WIDTH 
                    && frame % EDITOR_CAMERA_SPEED == 0) 
                        ++cameraX; 
                }
            }

            // Saving 
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)
            && sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            {
                Loader::SaveWorld(level, world);
                edits = false;
            }

            if(!edits)
            {
                if(level == 0) SavedIcon.setString("      (End Level Saved)");
                else SavedIcon.setString("      (Level " + std::to_string(level) + " Saved)");
                SavedIcon.setFillColor(sf::Color::Green);
            } else {
                if(level == 0) SavedIcon.setString("      (End Level Not Saved)");
                else SavedIcon.setString("      (Level " + std::to_string(level) + " Not Saved)");
                SavedIcon.setFillColor(sf::Color::Red);
            }

            // Calculate mouse pixel
            mouse = sf::Mouse::getPosition(app);
            mouse.x /= app.getSize().x/double(GAME_WIDTH);
            mouse.y /= app.getSize().y/double(GAME_HEIGHT);
            mouse.x += cameraX;

            // Mouse and Updating screen
            if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                if(sf::Mouse::getPosition(app).x >= 0 
                && sf::Mouse::getPosition(app).x < app.getSize().x)
                {
                    if(sf::Mouse::getPosition(app).y >= 0 
                    && sf::Mouse::getPosition(app).y < app.getSize().y)
                    {
                        // Only update if block is different
                        if(world[mouse.x][mouse.y] != Game::GameTypeList[item].type)
                        {
                            edits = true;
                            undoList.push({mouse, world[mouse.x][mouse.y]});
                            world[mouse.x][mouse.y] = Game::GameTypeList[item].type;
                        }
                    }
                }
            } 

            // Draw World
            updateBuffer(buffer, world, cameraX, Game::GameTypeList[item].type, mouse);
            Graphics::pushRGBA(app, reinterpret_cast<const Byte*>(buffer));

            // Draw Text
            app.draw(SavedIcon);
            app.draw(Help);

            for(sf::Text& BlockText : BlocksUp)
                app.draw(BlockText);

            app.draw(Block);
            app.draw(BlockDown);

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