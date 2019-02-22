#include "./Extras/Game.h"

/**************************/
/***** STATIC MEMBERS *****/
/**************************/

IntType Game::randomize(IntType n)
{
    // 24 Rounds of Blum Blum Shub with a 
    // 24 bit M setup. Should be pretty random
    n = n*n % M; n = n*n % M; n = n*n % M;
    n = n*n % M; n = n*n % M; n = n*n % M;
    n = n*n % M; n = n*n % M; n = n*n % M;
    n = n*n % M; n = n*n % M; n = n*n % M;
    n = n*n % M; n = n*n % M; n = n*n % M;
    n = n*n % M; n = n*n % M; n = n*n % M;
    n = n*n % M; n = n*n % M; n = n*n % M;
    n = n*n % M; n = n*n % M; n = n*n % M;

    // Blum Blum Shub only produces positive numbers, 
    // but I use std::abs incase the RNG changes
    return std::abs(n);
}

IntType Game::GameTypeData::randomize(IntType cx, IntType x, IntType y) const
{
    if(randomness != 0) 
    {
        // If Camera Speed is 0, then don't divide
        if(cameraSpeed == 0) 
        { return Game::randomize((x+1) * (y+1)) % randomness; } 
        
        return Game::randomize((cx / cameraSpeed + x) * (y+1)) % randomness;
    }
    return IntType(0);
}

// Game types and their propertys
const Game::GameTypeLink Game::GameTypeList[Game::GameTypeCount] = {
    {
        GameType::Sky, 
        {"Sky", sf::Color(0, 160, 200), 8, 3, 
            false, // Solid
            false, // Liquid
            false, // Trap
            false, // Jump
            false, // Bounce
            false // Smog
        }
    }, {
        GameType::Ground, 
        {"Ground", sf::Color(52, 52, 52), 24, 1, 
            true,  // Solid
            false, // Liquid
            false, // Trap
            true,  // Jump
            false, // Bounce
            false // Smog
        }
    }, {
        GameType::Trap, 
        {"Trap", sf::Color(220, 32, 0), 32, 0, 
            false, // Solid
            false, // Liquid
            true,  // Trap
            false, // Jump
            false // Smog
        }
    }, {
        GameType::Bounce, 
        {"Bounce", sf::Color(64, 255, 164), 0, 0, 
            false, // Solid
            false, // Liquid
            false, // Trap
            false, // Jump
            true,  // Bounce
            false // Smog
        }
    }, {
        GameType::Mud, 
        {"Mud", sf::Color(130, 60, 10), -12, 1, 
            true,  // Solid
            false, // Liquid
            false, // Trap
            false, // Jump
            false, // Bounce
            false  // Smog
        }
    }, {
        GameType::Water, 
        {"Water", sf::Color(0, 64, 255), 8, -2, 
            false, // Solid
            true,  // Liquid
            false, // Trap
            true,  // Jump
            false, // Bounce
            false  // Smog
        }
    }, {
        GameType::Smog, 
        {"Smog", sf::Color(128, 128, 128), 8, 6, 
            false, // Solid
            false, // Liquid
            false, // Trap
            false, // Jump
            false, // Bounce
            true   // Smog
        }
    }
};

const Game::GameTypeData Game::GetGameTypeData(Game::GameType input) 
{
    for(const GameTypeLink& entry : GameTypeList)
        if(input == entry.type) return entry.data;
    return GameTypeList[0].data;
}

/****************************/
/***** INSTANCE MEMBERS *****/
/****************************/

Game::Game() { loadWorld(START_LEVEL); }

/********************/
/***** CONTROLS *****/
/********************/
 
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
        if(sf::Joystick::isButtonPressed(DEFAULT_JOYSTICK_PORT, ID))
            return true; 

    return sf::Keyboard::isKeyPressed(sf::Keyboard::Space)
        || upKey() || downKey();
}

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

bool Game::editorCheatKey()
{
    return cheatKey() && sf::Keyboard::isKeyPressed(sf::Keyboard::E);
}

/********************/
/***** GAMELOOP *****/
/********************/

void Game::gameLoop()
{
    // Check for game reset command
    resetKeyLoop();

    // Update times
    frameTimeLoop();

    // Check to see if user has reached goal
    goalLoop();

    // Check for debug tools
    if(cheatLoop()) return;

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

/********************************/
/***** SUBUNITS OF GAMELOOP *****/
/********************************/

void Game::resetKeyLoop()
{
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        loadWorld(START_LEVEL); 
}

void Game::frameTimeLoop()
{
    if(player.x > START_SIZE && !getWinner()) 
        levelFrames[level] = ++frame;
    else if(level == START_LEVEL) 
        frame = 0; 
}

void Game::goalLoop()
{
    if(player.x >= GAME_LENGTH) 
        loadWorld(level + 1);
}

// Return if player has cheated
bool Game::cheatLoop()
{
    // Developer Key Combos
    if(flyCheatKey())
    {
        setCheater();
        if(player.x > 0 && leftKey())
            player.x -= CHEAT_SPEED; 

        if(player.x <= GAME_LENGTH && rightKey())
            player.x += CHEAT_SPEED; 

        if(player.y > 0 && upKey())
            player.y -= 1; 

        if(player.y < GAME_HEIGHT - 1 && downKey())
            player.y += 1; 
        
        cameraLoop();
        return true;
    } else if(levelCheatKey())
    {
        setCheater();
        if(leftKey()) 
        { 
            level--; 
            while(leftKey()); 
        } else if(rightKey()) 
        { 
            level++; 
            while(rightKey()); 
        }
        loadWorld(level);
        return true;
    }

    return false;
}

void Game::trapLoop()
{
    // Trap Detection
    if(player.y == 0 || player.y == GAME_HEIGHT - 1 
    || GetGameTypeData(world[player.x][player.y]).trap
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
        if(GetGameTypeData(world[player.x][player.y + gravity]).jump)
        { 
            if(canJump) gravity = GravityType(-gravity);
            canJump = false;
        } 
    } else { canJump = true; }
}

void Game::bounceLoop()
{
    if(GetGameTypeData(world[player.x][player.y + gravity]).bounce
    || GetGameTypeData(world[player.x][player.y]).bounce)
    { 
        if(canBounce) gravity = GravityType(-gravity);
        canBounce = false; 
    }
    else { canBounce = true; }
}

void Game::movementLoop()
{
    if(player.x > 0 && leftKey())
        if(!GetGameTypeData(world[player.x - 1][player.y]).solid) 
            player.x--;

    if(rightKey()) 
        if(!GetGameTypeData(world[player.x + 1][player.y]).solid) 
            player.x++; 
}

void Game::cameraLoop()
{
    // Check for right border (377/987 is inverse golden ratio)
    while(player.x - cameraX > RIGHT_CAMERA_BOARDER) // Move Camera Right
    { 
        if(cameraX < GAME_LENGTH - GAME_WIDTH) ++cameraX; 
        else break; 
    }
    
    while(player.x - cameraX < LEFT_CAMERA_BOARDER) // Move Camera Left
    { 
        if(cameraX > 0) --cameraX; 
        else break; 
    }
}

void Game::gravityLoop()
{
    if(!GetGameTypeData(world[player.x][player.y + gravity]).solid)
    {
        if(GetGameTypeData(world[player.x][player.y + gravity]).liquid) 
        { if(frame % 2 == 0) player.y += gravity; }
        else player.y += gravity; 
    } 
}

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

/*******************************************/
/***** WORLD RENDERING / WORLD LOADING *****/
/*******************************************/

IntType Game::loadWorld(const IntType inLevel)
{
    sf::Image img;
    level = inLevel % MAX_LEVEL_COUNT;

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
                if(pixel.g > COLOR_THRESHOLD) { out |= 0b010; }
                if(pixel.b > COLOR_THRESHOLD) { out |= 0b001; }
                world[x][y] = static_cast<GameType>(out);
            }
        }
    } else { return loadWorld(level + 1); }
    reset();

    if(level != 0) finalLevel = level;
    return level;
}

// Render Game
const Byte* Game::returnWorldPixels()
{
    const bool smog = GetGameTypeData(world[player.x][player.y]).smog;    
    for(IntType y = 0; y < GAME_HEIGHT; y++)
    {
        for(IntType x = 0; x < GAME_WIDTH; x++)
        {
            IntType R, G, B;
            if(cameraX + x == player.x && y == player.y)
            {
                R = 200; G = 255; B = 200;
            } else 
            {
                // Current Pixel
                const GameType gamePixel = world[cameraX + x][y];
                const GameTypeData pixelData = GetGameTypeData(gamePixel);

                // Values to feed into buffer
                R = pixelData.color.r; 
                G = pixelData.color.g; 
                B = pixelData.color.b;
                
                // Randomize Color
                IntType random = pixelData.randomize(cameraX, x, y);
                R += random; G += random; B += random;

                // Smog
                if(smog)
                {
                    double dis = std::max(
                        1.0, 
                        std::hypot(double(player.x - (x + cameraX)), double(player.y - y)) - SMOG_SIZE
                    );
                    R /= dis*dis;  G /= dis*dis;  B /= dis*dis;
                    IntType random = randomize(frame*(x+1)*(y+1))%4;
                    R += random; G += random; B += random; 
                } 
            }

            // Start Area
            if(x + cameraX <= START_SIZE) G += 48;

            // Trap Wall
            if(x + cameraX <= trapX/TRAP_SPEED) 
            {
                const double red = -(256.0/TRAP_SMOOTH)*(x + cameraX - trapX/TRAP_SPEED);
                R += red;
                G -= red/4.0;
                B -= red/4.0;
            }

            // Cap RGB Values
            buffer[y][x][0] = static_cast<Byte>(std::max(std::min(R, IntType(255)), IntType(0))); // Red
            buffer[y][x][1] = static_cast<Byte>(std::max(std::min(G, IntType(255)), IntType(0))); // Green
            buffer[y][x][2] = static_cast<Byte>(std::max(std::min(B, IntType(255)), IntType(0))); // Blue
            buffer[y][x][3] = 255; // Alpha
        }
    }

    return reinterpret_cast<const Byte*>(buffer);
}

/*****************************/
/***** GETTERS / SETTERS *****/
/*****************************/

IntType Game::getCameraX() const 
{ return cameraX; }

IntType Game::getLevel() const 
{ return level; }

IntType Game::getFinalLevel() const 
{ return finalLevel; }

IntType Game::getFrame() const
{ return frame; }

IntType Game::getLevelFrame(IntType level) const 
{ 
    level = std::max(level, IntType(0));
    level = std::min(level, MAX_LEVEL_COUNT);
    return levelFrames[level]; 
}

bool Game::getCheater() const 
{ return hasCheated; }

void Game::setCheater() 
{ hasCheated = true; }

bool Game::getWinner() const 
{ return level == 0; }






