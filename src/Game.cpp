#include "Game.h"
#include <stdlib.h> /// RNG (ONLY FOR COLORS, NO GAME PLAY IN RNG)

Game::Game()
{ }

bool Game::moveCameraLeft()
{
    if(cX > 0){ cX--; }
    return cX > 0;
}

bool Game::moveCameraRight()
{
    if(cX < GAME_LENGTH - GAME_WIDTH){ cX++; }
    return cX < GAME_LENGTH - GAME_WIDTH;
}

void Game::reset()
{
    playerX = 5;
    playerY = 18;
    gravity = false; // false = down, true = up;
    trapX = TRAP_START;
    cX = 0;

    if(level == 0) frame = 0;
}

// Loading level from
void Game::loadWorld(const IntType w)
{
    sf::Image img;
    if(img.loadFromFile(LEVEL_NAMES[w]))
    {
        for(IntType x = 0; x < GAME_LENGTH; x++)
        {
            for(IntType y = 0; y < GAME_HEIGHT; y++)
            {
                // Detect different game elements
                        if(img.getPixel(x, y).r > 128)  { world[x][y] = type::ground; }
                else    if(img.getPixel(x, y).b > 128)  { world[x][y] = type::bounce; }
                else    if(img.getPixel(x, y).g > 128)  { world[x][y] = type::trap; }
                else                                    { world[x][y] = type::sky; }
            }
        }
    }
    reset();
}

void Game::gameLoop()
{
    // Goal Detection
    if(playerX == GAME_LENGTH){ level = (level + 1)%LEVEL_COUNT; loadWorld(level); }

    // Developer Key Combos
    if(sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(0x25)))
    {
        if(sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(0x26)))
        {
            if(sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(0x05)))
            {
                if(playerX > 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){ playerX--; }
                if(playerX < GAME_LENGTH && sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){ playerX++; }
                if(playerY > 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){ playerY--; }
                if(playerY < GAME_HEIGHT - 1 && sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){ playerY++; }

                if(playerX - cX > (GAME_WIDTH >> 1)){ moveCameraRight(); }
                if(playerX - cX < (GAME_WIDTH >> 2)){ moveCameraLeft(); }

                return;
            } else if(sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(0x0B)))
            {
                if(level > 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                { level--; while(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){} }
                if(level < LEVEL_COUNT && sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                { level++; while(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){} }

                loadWorld(level);
                return;
            }
        }
    }

    // Trap Detection
    if(playerY == 0 || playerY == GAME_HEIGHT - 1 
    || world[playerX][playerY] == type::trap
    || sf::Keyboard::isKeyPressed(sf::Keyboard::R))
    { reset(); return; }

    // Left wall trap
    if(playerX + TRAP_SMOOTH - 1 <= trapX/TRAP_SPEED)
    { reset(); return; }

    // Move trap and start timer if player has moved from start
    if(playerX > START_SIZE) { ++trapX; ++frame; }

    // Prevent player from getting to huge of a lead on the trap
    trapX = std::max(trapX, playerX - TRAP_LEAD);

    // Jumping
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)
    || (!gravity && sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    || (gravity  && sf::Keyboard::isKeyPressed(sf::Keyboard::Down)))
    {
        if(world[playerX][playerY + (gravity ? -1 : 1)] == type::ground && canJump)
        { 
            gravity = !gravity; 
            canJump = false;
        } 
    } else { canJump = true; }

    // Moving
    if(playerX > 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    { if(world[playerX-1][playerY] != type::ground) { playerX--; } }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    { if(world[playerX+1][playerY] != type::ground) { playerX++; } }

    // Bounce Detection
    if(world[playerX][playerY + (gravity ? -1 : 1)] == type::bounce 
    || world[playerX][playerY] == type::bounce)
    { 
        if(canBounce) gravity = !gravity; 
        canBounce = false; 
    }
    else { canBounce = true; }

    // Gravity
    if(gravity) { if(world[playerX][playerY-1] != type::ground) { playerY--; } }
    else        { if(world[playerX][playerY+1] != type::ground) { playerY++; } }

    // Camera Movement
    if(playerX - cX > (GAME_WIDTH >> 1)){ moveCameraRight(); }
    if(playerX - cX < (GAME_WIDTH >> 2)){ moveCameraLeft(); }
}

// Render Game
unsigned char* Game::returnPixels()
{
    IntType pixel = 0;
    for(IntType y = 0; y < GAME_HEIGHT; y++)
    {
        for(IntType x = 0; x < GAME_WIDTH; x++)
        {
            if(playerX == cX+x && playerY == y)
            {
                buffer[pixel + 0] = 200;
                buffer[pixel + 1] = 255;
                buffer[pixel + 2] = 200;
                buffer[pixel + 3] = 255;
            } else if(world[cX+x][y] == type::ground)
            {
                srand((cX+x)*(y+1));
                const char brightness = rand()%24 + 52;
                buffer[pixel + 0] = brightness;
                buffer[pixel + 1] = brightness;
                buffer[pixel + 2] = brightness;
                buffer[pixel + 3] = 255;
            } else if(world[cX+x][y] == type::bounce)
            {
                buffer[pixel + 0] = 64;
                buffer[pixel + 1] = 164;
                buffer[pixel + 2] = 255;
                buffer[pixel + 3] = 255;
            } else if(world[cX+x][y] == type::trap)
            {
                srand((x+1)*(y+1));
                const char brightness = rand()%32 - 16;
                buffer[pixel + 0] = 220 + brightness;
                buffer[pixel + 1] = 32 + brightness;
                buffer[pixel + 2] = 0;
                buffer[pixel + 3] = 255;
            } else // Sky 
            {
                srand((-(cX/3) - x)*(y+1));
                const char brightness = rand()%8 - ((gravity ? GAME_HEIGHT - y : y) << 2);
                buffer[pixel + 0] = 16 - brightness;
                buffer[pixel + 1] = 160 + brightness;
                buffer[pixel + 2] = 200 + brightness;
                buffer[pixel + 3] = 255;
            }

            if(x + cX <= START_SIZE) {
                buffer[pixel + 1] = std::min(255, buffer[pixel + 1] + 48);
            }

            if(x + cX <= trapX/TRAP_SPEED) {
                const double redVal = -(256.0/TRAP_SMOOTH)*(double(x + cX) - trapX/TRAP_SPEED);
                buffer[pixel + 0] = std::min(255, int(buffer[pixel] + redVal));
                buffer[pixel + 1] = std::max(0, int(buffer[pixel + 1] - redVal/4.0));
                buffer[pixel + 2] = std::max(0, int(buffer[pixel + 2] - redVal/4.0));
            }

            pixel += 4;
        }
    }

    return buffer;
}
