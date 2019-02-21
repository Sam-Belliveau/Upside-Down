#include "./Extras/Game.h"

Game::Game() { }

void Game::gameLoop()
{
    // Check for game reset command
    resetKeyLoop();

    // Update times
    frameTimeLoop();

    // Check to see if user has reached goal
    goalLoop();

    // Check for debug tools
    if(cheatLoop()) { return; }

    // Check for lava and the red wall
    trapLoop();

    // Check for bouncing blocks
    bounceLoop();

    // Update Joystick before moving
    sf::Joystick::update();

    // Check for user jump
    jumpLoop();

    // Check for user movement
    movementLoop();

    // Update camera to follow player
    cameraLoop();

    // Update player using gravity
    gravityLoop();
}

void Game::resetKeyLoop()
{
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    { loadWorld(START_LEVEL); }
}

void Game::frameTimeLoop()
{
    levelFrames[level] = frame;
    if(player.x > START_SIZE && !getWinner()) { ++frame; }
    else if(level == START_LEVEL) { frame = 0; }
}

void Game::goalLoop()
{
    if(player.x >= GAME_LENGTH) { loadWorld(level + 1); }
}

// Return if player has cheated
bool Game::cheatLoop()
{
    // Developer Key Combos
    if(flyCheatKey())
    {
        hasCheated = true;
        if(player.x > 0 && leftKey()){ player.x -= CHEAT_SPEED; }
        if(player.x <= GAME_LENGTH && rightKey()){ player.x += CHEAT_SPEED; }
        if(player.y > 0 && upKey()){ player.y -= 1; }
        if(player.y < GAME_HEIGHT - 1 && downKey()){ player.y += 1; }
        
        cameraLoop();
        return true;
    } else if(levelCheatKey())
    {
        hasCheated = true;
        if(leftKey()) { level--; while(leftKey()); } 
        else if(rightKey()) { level++; while(rightKey()); }
        loadWorld(level);
        return true;
    } 

    return false;
}

void Game::trapLoop()
{
    // Trap Detection
    if(player.y == 0 || player.y == GAME_HEIGHT - 1 
    || world[player.x][player.y] == GameType::Trap
    || player.x + TRAP_SMOOTH - 1 <= trapX/TRAP_SPEED)
    { reset(); return; }

    // Move trap and start timer if player has moved from start
    if(player.x > START_SIZE && !getWinner()) { ++trapX; }

    // Prevent player from getting to huge of a lead on the trap
    if(!getWinner())
    { trapX = std::max(trapX, IntType(player.x*TRAP_SPEED - TRAP_LEAD - TRAP_SMOOTH)); }
}

void Game::jumpLoop()
{
    if(jumpKey())
    {
        if(world[player.x][player.y + gravity] == GameType::Ground)
        { 
            if(canJump) gravity = GravityType(-gravity);
            canJump = false;
        } 
    } else { canJump = true; }
}

void Game::bounceLoop()
{
    if(world[player.x][player.y + gravity] == GameType::Bounce 
    || world[player.x][player.y] == GameType::Bounce)
    { 
        if(canBounce) gravity = GravityType(-gravity);
        canBounce = false; 
    }
    else { canBounce = true; }
}

void Game::movementLoop()
{
    if(player.x > 0 && leftKey())
    { 
        if(world[player.x - 1][player.y] != GameType::Ground) 
        { player.x--; } 
    } 
    if(rightKey()) 
    { 
        if(world[player.x + 1][player.y] != GameType::Ground) 
        { player.x++; } 
    }
}

void Game::cameraLoop()
{
    // Check for right border (377/987 is inverse golden ratio)
    while(player.x - cameraX > RIGHT_CAMERA_BOARDER) // Move Camera Right
    { 
        if(cameraX < GAME_LENGTH - GAME_WIDTH)
        { ++cameraX; } 
        else { break; }
    }
    
    while(player.x - cameraX < LEFT_CAMERA_BOARDER) // Move Camera Left
    { 
        if(cameraX > 0)
        { --cameraX; } 
        else { break; }
    }
}

void Game::gravityLoop()
{
    if(world[player.x][player.y + gravity] != GameType::Ground) 
    { player.y += gravity; } 
}

// Controls
float Game::joyXAxis()
{ 
    return sf::Joystick::getAxisPosition(DEFAULT_JOYSTICK_PORT, sf::Joystick::X)
         + sf::Joystick::getAxisPosition(DEFAULT_JOYSTICK_PORT, sf::Joystick::PovX); 
}

float Game::joyYAxis()
{ 
    return sf::Joystick::getAxisPosition(DEFAULT_JOYSTICK_PORT, sf::Joystick::Y)
         + sf::Joystick::getAxisPosition(DEFAULT_JOYSTICK_PORT, sf::Joystick::PovY); 
}

bool Game::upKey()
{
    return sf::Keyboard::isKeyPressed(sf::Keyboard::Up)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::W)
        || joyYAxis() < -Y_JOYSTICK_DEAD_ZONE; 
}

bool Game::downKey()
{
    return sf::Keyboard::isKeyPressed(sf::Keyboard::Down)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::S)
        || joyYAxis() > Y_JOYSTICK_DEAD_ZONE; 
}

bool Game::leftKey() 
{ 
    return sf::Keyboard::isKeyPressed(sf::Keyboard::Left)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::A)
        || joyXAxis() < -X_JOYSTICK_DEAD_ZONE; 
}

bool Game::rightKey()
{
    return sf::Keyboard::isKeyPressed(sf::Keyboard::Right)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::D)
        || joyXAxis() > X_JOYSTICK_DEAD_ZONE; 
}

bool Game::jumpKey()
{
    for(auto ID : JUMP_BUTTONS)
    {
        if(sf::Joystick::isButtonPressed(DEFAULT_JOYSTICK_PORT, ID))
        { return true; }
    }

    return sf::Keyboard::isKeyPressed(sf::Keyboard::Space)
        || (gravity == GravityType::Down && upKey()) 
        || (gravity == GravityType::Up && downKey());
}

// Cheat Keys
bool Game::cheatKey()
{
    return sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)
        && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift);
}

bool Game::flyCheatKey()
{
    return cheatKey() && sf::Keyboard::isKeyPressed(sf::Keyboard::F);
}

bool Game::levelCheatKey()
{
    return cheatKey() && sf::Keyboard::isKeyPressed(sf::Keyboard::L);
}

// Loading level from
IntType Game::loadWorld(const IntType inLevel)
{
    sf::Image img;
    level = inLevel % MAX_LEVEL_COUNT;

    //if(level == 0 && hasCheated) ++level;

    if(img.loadFromFile("./Levels/L" + std::to_string(level) + ".bmp"))
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
    } else { return loadWorld(level + 1); }
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
                G = 255; 
                B = 164;
            } else if(gamePixel == GameType::Trap) // Traps
            {
                const int brightness = randomize((x+1)*(y+1))%32 - 16;
                R = 220 + brightness;
                G = 32 + brightness;
                B = 0;
            } else // Sky 
            {
                const int brightness = randomize((-((cameraX)/3) - x)*(y+1))%8;
                const int redBoost = 4 * (gravity == GravityType::Up ? GAME_HEIGHT - y : y);
                R = 0   + brightness + redBoost;
                G = 160 + brightness - redBoost;
                B = 200 + brightness - redBoost;
            }

            if(x + cameraX <= START_SIZE) { G += 48; }

            if(x + cameraX <= trapX/TRAP_SPEED) {
                const double redVal = -(256.0/TRAP_SMOOTH)*(double(x + cameraX) - double(trapX/TRAP_SPEED));
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
IntType Game::getLevelFrame(IntType level) const { return levelFrames[level]; }
bool Game::getCheater() const { return hasCheated; }
void Game::setCheater() { hasCheated = true; }
bool Game::getWinner() const { return level == 0; }

void Game::reset()
{
    player = sf::Vector2<IntType>(5, 18);
    gravity = GravityType::Down; 
    trapX = TRAP_START;
    cameraX = 0;

    if(level == START_LEVEL) {
        hasCheated = false;
        frame = 0;
    }
}





