#ifndef TIMES_LEADER_BOARD_HPP
#define TIMES_LEADER_BOARD_HPP

#include "./Constants.h"
#include "./Game.h"

namespace TextTimes
{
    const char* Base32 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    static void UpdateHash(const Game& game, sf::Text& text)
    {
        // Setup string buffer
        std::ostringstream stream;
        stream << GAME_VERSION << " : ";
        HashType gameHash = game.getLevelHash();
        while(gameHash != 0)
        {
            stream << Base32[gameHash & 0x1f];
            gameHash >>= 5;
        }
        text.setString(stream.str());
    }

    static void UpdateTimer(const Game& game, sf::Text& text)
    {
        if(game.getCheater())
        { 
            text.setFillColor(BAD_COLOR);
            text.setString(
                "(Cheats Used)\n" 
                "Time: " + (std::to_string(double(game.getFrame()) / double(GAME_FPS)).substr(0,7)) + "s\n" +
                "Frames: " + std::to_string(game.getFrame()) + " / " + std::to_string(GAME_FPS) + "fps\n" +
                "Deaths: " + std::to_string(game.getDeaths())
            );
        } else {
            text.setFillColor(GOOD_COLOR);
            text.setString(
                "Time: " + (std::to_string(double(game.getFrame()) / double(GAME_FPS)).substr(0,7)) + "s\n" +
                "Frames: " + std::to_string(game.getFrame()) + " / " + std::to_string(GAME_FPS) + "fps\n" +
                "Deaths: " + std::to_string(game.getDeaths())
            );
        }
    }

    static void UpdateLeaderboard(const Game& game, sf::Text& text)
    {
        text.setPosition(GAME_SCALE * (TEXT_X - game.getCameraX()), GAME_SCALE * TEXT_Y);

        // Leader Board
        if(game.getCameraX() < START_SIZE 
        && game.getLevel() != START_LEVEL)
        {
            // Setup string buffer
            std::ostringstream stream;
            stream << std::setprecision(2) << std::fixed;
            stream << "Times:\n"; 
            
            if(game.getCheater())
                text.setFillColor(BAD_COLOR);
            else text.setFillColor(GOOD_COLOR); 

            if(game.getWinner())
            {
                for(std::size_t i = std::max(game.getFinalLevel() - 10, IntType(1)); 
                    i <= game.getFinalLevel(); ++i)
                {
                    // Balance Level Sign
                    stream << "Level " << i << ": ";
                    if(i < 10) { stream << " "; }
                    
                    // Calculate Time
                    stream << double(game.getLevelFrame(i)) / double(GAME_FPS) << "s\n";
                }
            } else 
            {
                for(std::size_t i = std::max(game.getLevel() - 10, IntType(1)); 
                    i < game.getLevel(); ++i)
                {
                    // Balance Level Sign
                    stream << "Level " << i << ": ";
                    if(i < 10) { stream << " "; }
                    
                    // Calculate Time
                    stream << double(game.getLevelFrame(i)) / double(GAME_FPS) << "s\n";
                }
            }
            
            // Print times to leader board
            text.setString(stream.str());
        } else
        { text.setString(""); }
    }
}

#endif