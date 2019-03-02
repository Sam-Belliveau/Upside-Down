#include "./Headers/Game.h"

/**************************/
/***** STATIC MEMBERS *****/
/**************************/

bool Game::GameTypeData::getProp(RawIntType prop) const
{
    // This allows for testing of more than
    // One property at a time
    return (propertys & prop) == prop;
}

IntType Game::GameTypeData::randomize(IntType cx, IntType x, IntType y) const
{
    if(randomness != 0) 
    {
        const IntType XRand = 1 + x + // Add texture offset
            (cx * cameraSpeed - GET_GLOBAL_FRAME() * textureSpeed + 0.5);
        const IntType YRand = 1 + y;
        
        return std::abs(RANDOMIZE(XRand * YRand)) % randomness;
    }

    return IntType(0);
}

// Game types and their properties    
/* ***** TYPE DATA STRUCTURE *****
 * GameType::XXXXX, <--- Game Type
 * {"[Block Name]", [Block Color], [1*], [2*], [3*],
 *      [Type Propertys] | [Type Propertys]
 * }
 * 
 * 1* = Texture Randomness
 * 2* = How Fast Texture Moves With Camera (Paralax)
 * 3* = How Fast Texture Moves On Its Own
 */ 
const Game::GameTypeLink Game::GameTypeList[GameTypeCount] = {
    {
        GameType::Sky, 
        {"Sky", sf::Color(0, 160, 200), 8, 1.0/3.0, 1.0/GAME_FPS,
            TypeProps::None
        }
    }, {
        GameType::Ground, 
        {"Ground", sf::Color(52, 52, 52), 24, 1.0, 0.0,
            TypeProps::Solid | TypeProps::Jumpable 
        }
    }, {
        GameType::Trap, 
        {"Trap", sf::Color(220, 32, 0), 32, 0.0, 0,
            TypeProps::Trap
        }
    }, {
        GameType::Bounce, 
        {"Bounce", sf::Color(64, 255, 164), 0, 0.0, 0.0,
            TypeProps::Bounce
        }
    }, {
        GameType::Mud, 
        {"Mud", sf::Color(130, 60, 10), -12, 1.0, 0.0,
            TypeProps::Solid | TypeProps::Slow
        }
    }, {
        GameType::Water, 
        {"Water", sf::Color(0, 64, 255), 8, -1.0/2.0, 0.1,
            TypeProps::LowGravity | TypeProps::Jumpable
        }
    }, {
        GameType::Smog, 
        {"Smog", sf::Color(128, 128, 128), 8, 1.0/6.0, 0.5/GAME_FPS,
            TypeProps::Smog | TypeProps::StopStorm
        }
    }, {
        GameType::LowGravity, 
        {"Low Gravity", sf::Color(100, 0, 100), 8, 1.0/3.0, 1.0/GAME_FPS,
            TypeProps::LowGravity
        }
    }, {
        GameType::MoveRight, 
        {"Move Right", sf::Color(64, 196, 0), 64, 0, 1.0/3.0,
            TypeProps::MoveRight
        }
    }, {
        GameType::MoveLeft, 
        {"Move Left", sf::Color(64, 196, 0), 64, 0, -1.0/3.0,
            TypeProps::MoveLeft
        }
    }, {
        GameType::Honey, 
        {"Honey", sf::Color(192, 128, 16), 16, 1.0/3.0, 1.0/GAME_FPS,
            TypeProps::Slow | TypeProps::LowGravity
        }
    }, {
        GameType::Coin, 
        {"Coin", sf::Color(255, 200, 16), 32, 0, 5.0/GAME_FPS,
            TypeProps::Coin
        }
    }, {
        GameType::Goal, 
        {"Goal", sf::Color(196, 255, 16), 0, 0.0, 0.0,
            TypeProps::Goal
        }
    }
};

const Game::GameTypeData Game::GetTypeData(GameType input) 
{
    for(const GameTypeLink& entry : GameTypeList)
        if(input == entry.type) return entry.data;
    return GameTypeList[RANDOMIZE(GET_GLOBAL_FRAME())%GameTypeCount].data;
}

/****************************/
/***** INSTANCE MEMBERS *****/
/****************************/

Game::Game() 
{
    loadWorld(START_LEVEL); 
}

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

bool Game::resetKey()
{
    return sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)
        || sf::Joystick::isButtonPressed(DEFAULT_JOYSTICK_PORT, RESET_BUTTON); 
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

bool Game::jumpKey(GravityType gravity)
{
    for(auto ID : JUMP_BUTTONS)
        if(sf::Joystick::isButtonPressed(DEFAULT_JOYSTICK_PORT, ID))
            return true; 

    return sf::Keyboard::isKeyPressed(sf::Keyboard::Space)
        || (upKey() && gravity == GravityType::Down) 
        || (downKey() && gravity == GravityType::Up);
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
    // Update Type data for player blocks
    playerBlockData = GetTypeData(world[player.x][player.y]);
    groundBlockData = GetTypeData(world[player.x][player.y + gravity]);

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

    // Check if player is touching coin
    // Check this last so it gets replaced fastest
    coinLoop();
}

/********************************/
/***** SUBUNITS OF GAMELOOP *****/
/********************************/

void Game::resetKeyLoop()
{
    if(resetKey()) loadWorld(START_LEVEL); 
}

void Game::frameTimeLoop()
{
    ++rawFrame; // Used for game mechanics, must always tick
    if(player.x > START_SIZE && !getWinner()) 
        levelFrames[level] = ++frame;
    else if(level == START_LEVEL) 
    {
        hasCheated = false;
        frame = 0; 
    }
}

void Game::goalLoop()
{
    if(player.x >= GAME_LENGTH 
    || playerBlockData.getProp(TypeProps::Goal)) 
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
            player.x -= 1; 

        if(player.x <= GAME_LENGTH && rightKey())
            player.x += 1; 

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
    if(player.y <= 0 || player.y == GAME_HEIGHT - 1
    || playerBlockData.getProp(TypeProps::Trap)
    || player.x - 1 <= trapX/TRAP_SPEED - TRAP_SMOOTH)
    { 
        if(player.x >= START_SIZE && !getWinner())
        { ++deaths; }

        if(level == START_LEVEL)
        { loadWorld(START_LEVEL); }
        else { reset(); }
        return; 
    }

    if(!getWinner() && !playerBlockData.getProp(TypeProps::StopStorm))
    {
        // Move trap and start timer if player has moved from start
        if(player.x > START_SIZE) { ++trapX; }

        // Prevent player from getting to huge of a lead on the trap
        if(!getWinner())
        { trapX = std::max(trapX, IntType(player.x*TRAP_SPEED - TRAP_LEAD - TRAP_SMOOTH)); }
    }
}

void Game::jumpLoop()
{
    if(jumpKey(gravity))
    {
        if(groundBlockData.getProp(TypeProps::Jumpable))
        { 
            if(canJump) gravity = GravityType(-gravity);
            canJump = false;
        } 
    } else { canJump = true; }
}

void Game::bounceLoop()
{
    if(groundBlockData.getProp(TypeProps::Bounce)
    || playerBlockData.getProp(TypeProps::Bounce))
    { 
        if(canBounce) gravity = GravityType(-gravity);
        if(playerBlockData.getProp(TypeProps::Bounce))
            canBounce = false; 
    }
    else { canBounce = true; }
}

void Game::movementLoop()
{
    if(groundBlockData.getProp(TypeProps::Slow) 
    || playerBlockData.getProp(TypeProps::Slow))
        if(rawFrame % 2 != 0) return;

    if(((player.x > 0 && leftKey()) || playerBlockData.getProp(TypeProps::MoveLeft))
    && !GetTypeData(world[player.x - 1][player.y]).getProp(TypeProps::MoveRight))
    {
        if(!GetTypeData(world[player.x - 1][player.y]).getProp(TypeProps::Solid)) 
        {
            player.x--;

            // Update Block Data Info
            playerBlockData = GetTypeData(world[player.x][player.y]);
            groundBlockData = GetTypeData(world[player.x][player.y + gravity]);
        }
    }

    if((rightKey() || playerBlockData.getProp(TypeProps::MoveRight)) 
    && !GetTypeData(world[player.x + 1][player.y]).getProp(TypeProps::MoveLeft))
    {
        if(!GetTypeData(world[player.x + 1][player.y]).getProp(TypeProps::Solid)) 
        {
            player.x++; 

            // Update Block Data Info
            playerBlockData = GetTypeData(world[player.x][player.y]);
            groundBlockData = GetTypeData(world[player.x][player.y + gravity]);
        }    
    }
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
    // You cant use ground block data as player moved in the movement loop
    if(!groundBlockData.getProp(TypeProps::Solid))
    {
        if(groundBlockData.getProp(TypeProps::LowGravity)) 
        { if(rawFrame % 2 == 0) player.y += gravity; }
        else player.y += gravity; 

        // Update Block Data Info
        playerBlockData = GetTypeData(world[player.x][player.y]);
        groundBlockData = GetTypeData(world[player.x][player.y + gravity]);
    } 
}

void Game::coinLoop()
{
    if(playerBlockData.getProp(TypeProps::Coin))
    {
        // Count Coin
        ++coins;
        ++levelCoins[level];

        // Replace coin block
        const GameTypeData leftBlock   = GetTypeData(world[player.x - 1][player.y]);
        const GameTypeData rightBlock  = GetTypeData(world[player.x + 1][player.y]);
        const GameTypeData topBlock    = GetTypeData(world[player.x][player.y - gravity]);
        
        // Guess Using Rules what block should take the coins place
        if(!leftBlock.getProp(TypeProps::Solid)
        && !rightBlock.getProp(TypeProps::Solid)
        && world[player.x - 1][player.y] == world[player.x + 1][player.y])
        {
            world[player.x][player.y] = world[player.x - 1][player.y];
        } else if(!topBlock.getProp(TypeProps::Solid))
        {
            world[player.x][player.y] = world[player.x][player.y - gravity];
        } else if(!groundBlockData.getProp(TypeProps::Solid))
        {
            world[player.x][player.y] = world[player.x][player.y + gravity];
        } else 
        { 
            world[player.x][player.y] = GameType::Sky; 
        }
    }
}

void Game::reset()
{
    player = sf::Vector2<IntType>(GAME_START_X, GAME_START_Y);
    gravity = GravityType::Down; 
    trapX = TRAP_START;
    rawFrame = 0;
    cameraX = 0;

    if(level == START_LEVEL) {
        frame = 0; 
        deaths = 0;
        coins = 0;
        hasCheated = false;
        for(IntType& t : levelFrames) t = 0;
        for(IntType& t : levelCoins) t = 0;
    }
}

/*******************************************/
/***** WORLD RENDERING / WORLD LOADING *****/
/*******************************************/

IntType Game::loadWorld(const IntType inLevel)
{
    std::ifstream levelFile(LEVEL_FOLDER + LEVEL_PREFIX + std::to_string(inLevel) + LEVEL_EXTENTION);
    level = inLevel % MAX_LEVEL_COUNT;

    if(!Loader::LoadWorld(level, world, false))
        return loadWorld(level + 1);

    updateLevelHash();
    reset();
    return level;
}

// Render Game
const Byte* Game::returnWorldPixels(bool focus)
{
    const bool smog = GetTypeData(world[player.x][player.y]).getProp(TypeProps::Smog);    
    for(IntType y = 0; y < GAME_HEIGHT; y++)
    {
        for(IntType x = 0; x < GAME_WIDTH; x++)
        {
            IntType R, G, B;
            if(cameraX + x == player.x && y == player.y)
            {
                R = PLAYER_COLOR.r; 
                G = PLAYER_COLOR.g; 
                B = PLAYER_COLOR.b;
            } else 
            {
                // Current Pixel
                const GameType gamePixel = world[cameraX + x][y];
                const GameTypeData pixelData = GetTypeData(gamePixel);

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
                    IntType smogRand = RANDOMIZE(frame*(x+cameraX+1)*(y+1))%4;
                    R /= dis*dis;  G /= dis*dis;  B /= dis*dis;
                    R += smogRand; G += smogRand; B += smogRand; 
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

            if(!focus)
            {
                R = (R + 256.0*(LOST_FOCUS_COLOR - 1.0)) / LOST_FOCUS_COLOR;
                G = (G + 256.0*(LOST_FOCUS_COLOR - 1.0)) / LOST_FOCUS_COLOR;
                B = (B + 256.0*(LOST_FOCUS_COLOR - 1.0)) / LOST_FOCUS_COLOR;
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

HashType Game::updateLevelHash() 
{
    HashType oldHash = hash; hash = 0; 
    HashType oldCoins = maxCoins; maxCoins = 0;
    HashType oldFinalLevel = finalLevel; finalLevel = 0;
    for(IntType& t : levelMaxCoins) t = 0;
    
    GameType hashWorld[GAME_LENGTH][GAME_HEIGHT];
    for(IntType lvl = 0; lvl < MAX_LEVEL_COUNT; ++lvl)
    {
        if(Loader::LoadWorld(lvl, hashWorld, false))
        {
            if(finalLevel < lvl) { finalLevel = lvl; }
            for(IntType x = 0; x < GAME_LENGTH; ++x)
            {
                for(IntType y = 0; y < GAME_HEIGHT; ++y)
                {
                    if(GetTypeData(hashWorld[x][y]).getProp(TypeProps::Coin))
                    { 
                        ++levelMaxCoins[lvl];
                        ++maxCoins; 
                    }

                    hash += ROTATE(hash, 7);
                    hash += ROTATE(hash, 20 + (19*lvl)%23);
                    hash += LookUp::PiTable[(
                        x*GAME_HEIGHT +
                        y + hashWorld[x][y]
                    ) & 0xff];
                    hash += ROTATE(hash, 43);
                }
            }
        } else {
            for(IntType round = 0; round < 0x100; ++round)
            {
                hash += ROTATE(hash, 7);
                hash += ROTATE(hash, 20 + (19*lvl)%23);
                hash += LookUp::PiTable[Byte(round)];
                hash += ROTATE(hash, 43);
            }
        }
    }

    // Final Mix
    for(IntType round = 0; round < 0x100; ++round)
    {
        hash += ROTATE(hash, 7);
        hash += ROTATE(hash, 20 + (19*round)%23);
        hash += LookUp::PiTable[Byte(round)];
        hash += ROTATE(hash, 43);
    }

    if(hash != oldHash 
    || maxCoins != oldCoins
    || finalLevel != oldFinalLevel) setCheater();
    return hash;
}

/*****************************/
/***** GETTERS / SETTERS *****/
/*****************************/

IntType Game::getCameraX() const 
{ 
    return cameraX; 
}

IntType Game::getLevel() const 
{ 
    return level; 
}

IntType Game::getDeaths() const
{ 
    return deaths; 
}

IntType Game::getCoins() const
{
    return coins;
}

IntType Game::getMaxCoins() const
{
    return maxCoins;
}

IntType Game::getLevelCoins(IntType level) const
{    
    level = std::max(level, IntType(0));
    level = std::min(level, MAX_LEVEL_COUNT);
    return levelCoins[level];
}

IntType Game::getLevelMaxCoins(IntType level) const
{    
    level = std::max(level, IntType(0));
    level = std::min(level, MAX_LEVEL_COUNT);
    return levelMaxCoins[level];
}

IntType Game::getFinalLevel() const 
{ 
    return finalLevel; 
}

IntType Game::getFrame() const
{ 
    return frame; 
}

IntType Game::getLevelFrame(IntType level) const 
{ 
    level = std::max(level, IntType(0));
    level = std::min(level, MAX_LEVEL_COUNT);
    return levelFrames[level]; 
}

HashType Game::getLevelHash() const 
{
    return hash;
}

bool Game::getCheater() const 
{ 
    return hasCheated; 
}

bool Game::getWinner() const 
{ 
    return level == 0; 
}

void Game::setCheater() 
{ 
    hasCheated = true; 
}
