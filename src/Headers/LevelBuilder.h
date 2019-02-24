#ifndef LEVEL_BUILDER_H
#define LEVEL_BUILDER_H

#include <SFML/Graphics.hpp>
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

    static IntType LoopTypeIndex(IntType index)
    {
        while(index < 0) index += GameTypeCount;
        index %= GameTypeCount;
        return index;
    }
    
    static IntType GetTypeIndex(Game::GameTypeLink *list, GameType block)
    {
        for(IntType i = 0; i < GameTypeCount; ++i)
            if(block == list[i].type) return i;
        return 0;
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
        SavedIcon.setPosition((GAME_WIDTH-11)*GAME_SCALE,GAME_SCALE * (GAME_HEIGHT - 9));

        sf::Text Help = SavedIcon;
        Help.setPosition((GAME_WIDTH-14)*GAME_SCALE,GAME_SCALE * (GAME_HEIGHT - 9));
        Help.setString( 
            "\n             Escape = Exit Editor"
            "\n      Ctrl + Escape = Force Exit"
            "\n           Ctrl + S = Save Level"  
            "\n           Ctrl + Z = Undo"
            "\n   Ctrl + Shift + Z = Revert To Save"
            "\n          Up + Down = Change Block"
            "\n         Left Click = Place Block"
            "\n        Right Click = Copy Block"
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

        Game::GameTypeLink sortedTypeList[GameTypeCount];
        for(IntType i = 0; i < GameTypeCount; ++i)
            sortedTypeList[i] = Game::GameTypeList[i];

        std::stable_sort(std::begin(sortedTypeList), std::end(sortedTypeList),
            [](const Game::GameTypeLink& a, const Game::GameTypeLink& b) -> bool 
            {   
                return GetLuminance(a.data.color) < GetLuminance(b.data.color);
            }
        );
        
        std::stack<UndoData> undoList;
        GameType world[GAME_LENGTH][GAME_HEIGHT] = {};
        Byte buffer[GAME_HEIGHT][GAME_WIDTH][4] = {};
        IntType item = 0, frame = 0;
        bool edits = false;

        Loader::LoadWorld(level, world);

        sf::Vector2i mouse(0,0);
        while (app.isOpen())
        {
            // Slow Movement with a frame counter
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
            item = LoopTypeIndex(item);

            // Indicate Item
            for(IntType i = 0; i < BLOCK_LIST_SIZE; ++i)
            {
                BlocksUp[i].setString(sortedTypeList[LoopTypeIndex(item - (i + 1))].data.name);
                BlocksUp[i].setFillColor(sortedTypeList[LoopTypeIndex(item - (i + 1))].data.color);
            }

            Block.setString(sortedTypeList[LoopTypeIndex(item - 0)].data.name);
            Block.setFillColor(sortedTypeList[LoopTypeIndex(item - 0)].data.color);
            BlockDown.setString(sortedTypeList[LoopTypeIndex(item + 1)].data.name);
            BlockDown.setFillColor(sortedTypeList[LoopTypeIndex(item + 1)].data.color);

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
                    if(!edits)
                    { 
                        cameraX = 0; 
                        if(level == 0) level = MAX_LEVEL_COUNT - 1;
                        else --level;
                        
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
                    if(!edits)
                    { 
                        cameraX = 0; 
                        ++level; level %= MAX_LEVEL_COUNT;

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
            if(sf::Mouse::getPosition(app).x >= 0 
            && sf::Mouse::getPosition(app).x < app.getSize().x)
            {
                if(sf::Mouse::getPosition(app).y >= 0 
                && sf::Mouse::getPosition(app).y < app.getSize().y)
                {
                    if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
                    {
                        // Only update if block is different
                        if(world[mouse.x][mouse.y] != sortedTypeList[item].type)
                        {
                            edits = true;
                            undoList.push({mouse, world[mouse.x][mouse.y]});
                            world[mouse.x][mouse.y] = sortedTypeList[item].type;
                        }
                    }

                    if(sf::Mouse::isButtonPressed(sf::Mouse::Right))
                        item = GetTypeIndex(sortedTypeList, world[mouse.x][mouse.y]);
                }
            }

            // Draw World
            updateBuffer(buffer, world, cameraX, sortedTypeList[item].type, mouse);
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