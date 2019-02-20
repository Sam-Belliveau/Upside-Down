#include "Game.h"
#include <stdlib.h> /// RNG (ONLY FOR COLORS, NO GAME PLAY IN RNG)

Game::Game()
{ }

void Game::gameLoop()
{
    const bool left =  sf::Keyboard::isKeyPressed(sf::Keyboard::Left)
                    || sf::Keyboard::isKeyPressed(sf::Keyboard::A);
    const bool right = sf::Keyboard::isKeyPressed(sf::Keyboard::Right)
                    || sf::Keyboard::isKeyPressed(sf::Keyboard::D);
    const bool up = sf::Keyboard::isKeyPressed(sf::Keyboard::Up)
                 || sf::Keyboard::isKeyPressed(sf::Keyboard::W);
    const bool down =  sf::Keyboard::isKeyPressed(sf::Keyboard::Down)
                    || sf::Keyboard::isKeyPressed(sf::Keyboard::S);
    const bool jump = sf::Keyboard::isKeyPressed(sf::Keyboard::Space)
                    || (gravity == GravityType::Down && up) 
                    || (gravity == GravityType::Up && down);

    // Update Times
    levelFrames[level] = frame;

    // Goal Detection
    if(player.x == GAME_LENGTH) { loadWorld(level + 1); }

    // Developer Key Combos
    if(sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(0x25)))
    {
        if(sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(0x26)))
        {
            if(sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(0x05)))
            {
                if(player.x > 0 && left){ player.x--; }
                if(player.x < GAME_LENGTH && right){ player.x++; }
                if(player.y > 0 && up){ player.y--; }
                if(player.y < GAME_HEIGHT - 1 && down){ player.y++; }

                if(player.x - cameraX > (GAME_WIDTH >> 1)){ moveCameraRight(); }
                if(player.x - cameraX < (GAME_WIDTH >> 2)){ moveCameraLeft(); }

                return;
            } else if(sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(0x0B)))
            {
                if(left)
                { 
                    level--; 
                    while(sf::Keyboard::isKeyPressed(sf::Keyboard::Left));
                } else if(right)
                { 
                    level++; 
                    while(sf::Keyboard::isKeyPressed(sf::Keyboard::Right));
                }
                loadWorld(level);
                return;
            }
        }
    }

    // Reset key
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    { loadWorld(0); }

    // Trap Detection
    if(player.y == 0 || player.y == GAME_HEIGHT - 1 
    || world[player.x][player.y] == GameType::Trap
    || player.x + TRAP_SMOOTH - 1 <= trapX/TRAP_SPEED)
    { reset(); return; }

    // Move trap and start timer if player has moved from start
    if(player.x > START_SIZE) { ++trapX; ++frame; }

    // Prevent player from getting to huge of a lead on the trap
    trapX = std::max(trapX, IntType(player.x*TRAP_SPEED - TRAP_LEAD - TRAP_SMOOTH));

    // Jumping
    if(jump)
    {
        if(world[player.x][player.y + gravity] == GameType::Ground)
        { 
            if(canJump) flipGravity();
            canJump = false;
        } 
    } else { canJump = true; }

    // Bounce Detection
    if(world[player.x][player.y + gravity] == GameType::Bounce 
    || world[player.x][player.y] == GameType::Bounce)
    { 
        if(canBounce) flipGravity();
        canBounce = false; 
    }
    else { canBounce = true; }

    // Moving
    if(player.x > 0 && left)
    { 
        if(world[player.x - 1][player.y] != GameType::Ground) 
        { player.x--; } 
    } if(right) 
    { 
        if(world[player.x + 1][player.y] != GameType::Ground) 
        { player.x++; } 
    }

    // Gravity
    if(world[player.x][player.y + gravity] != GameType::Ground) 
    { player.y += gravity; } 

    // Camera Movement
    if(player.x - cameraX > (GAME_WIDTH >> 1)){ moveCameraRight(); }
    if(player.x - cameraX < (GAME_WIDTH >> 2)){ moveCameraLeft(); }
}

// Loading level from
IntType Game::loadWorld(const IntType inLevel)
{
    sf::Image img;
    level = inLevel % MAX_LEVEL_COUNT;
    if(img.loadFromFile("./Levels/L" + std::to_string(++level) + ".bmp"))
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
                world[x][y] = static_cast<GameType>(out);
            }
        }
    } else { return loadWorld(level+1); }
    reset();

    return level;
}

// Render Game
Byte* Game::returnWorldPixels()
{
    for(IntType y = 0; y < GAME_HEIGHT; y++)
    {
        for(IntType x = 0; x < GAME_WIDTH; x++)
        {
            // Current Pixel
            const GameType gamePixel = world[cameraX + x][y];

            // Values to feed into buffer
            int R, G, B;
            if(player.x == cameraX + x && player.y == y) // Player
            {
                R = 200;
                G = 255;
                B = 200;
            } else if(gamePixel == GameType::Ground) // Ground
            {
                const int brightness = randomize((cameraX + x) * (y + 1)) % 24 + 52;
                R = brightness;
                G = brightness;
                B = brightness;
            } else if(gamePixel == GameType::Bounce) // Bounces
            {
                R = 64; 
                G = 164; 
                B = 255;
            } else if(gamePixel == GameType::Trap) // Traps
            {
                const int brightness = randomize((x+1)*(y+1))%32 - 16;
                R = 220 + brightness;
                G = 32 + brightness;
                B = 0;
            } else // Sky 
            {
                const IntType rVal[3] = // Random Values to Average
                {
                    randomize((-((cameraX + 2)/3) - x)*(y+1))%8,
                    randomize((-((cameraX + 1)/3) - x)*(y+1))%8,
                    randomize((-((cameraX + 0)/3) - x)*(y+1))%8
                };
                const int brightness = (rVal[0] + rVal[1] + rVal[2])/3;
                const int redBoost = 4 * (gravity == GravityType::Up ? GAME_HEIGHT - y : y);
                R = 0   + brightness + redBoost;
                G = 160 + brightness - redBoost;
                B = 200 + brightness - redBoost;
            }

            if(x + cameraX <= START_SIZE) { G += 64; }

            if(x + cameraX <= trapX/TRAP_SPEED) {
                const double redVal = -(256.0/TRAP_SMOOTH)*(double(x + cameraX) - trapX/TRAP_SPEED);
                R += redVal;
                G -= redVal/4.0;
                B -= redVal/4.0;
            }

            const std::size_t pixel = 4 * (y * GAME_WIDTH + x);  
            buffer[pixel + 0] = static_cast<Byte>(std::max(std::min(R, 255), 0)); // Red
            buffer[pixel + 1] = static_cast<Byte>(std::max(std::min(G, 255), 0)); // Green
            buffer[pixel + 2] = static_cast<Byte>(std::max(std::min(B, 255), 0)); // Blue
            buffer[pixel + 3] = 255; // Alpha
        }
    }

    return buffer;
}

IntType Game::getCameraX() const { return cameraX; }
IntType Game::getLevel() const { return level; }
IntType Game::getFrame() const { return frame; }
IntType Game::getLevelFrame(IntType level) const 
{ return levelFrames[level]; }

bool Game::moveCameraLeft()
{
    if(cameraX > 0){ cameraX--; }
    return cameraX > 0;
}

bool Game::moveCameraRight()
{
    if(cameraX < GAME_LENGTH - GAME_WIDTH){ cameraX++; }
    return cameraX < GAME_LENGTH - GAME_WIDTH;
}

void Game::flipGravity()
{
    if (gravity == GravityType::Down) { gravity = GravityType::Up; } 
    else { gravity = GravityType::Down; }
}

void Game::reset()
{
    player = sf::Vector2<IntType>(5, 18);
    gravity = GravityType::Down; 
    trapX = TRAP_START;
    cameraX = 0;

    if(level <= 1) {
        frame = 0;
        for(auto &i : levelFrames) i = 0;
    }
}





