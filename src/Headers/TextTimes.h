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

        text.setPosition(6, 6 + GAME_SCALE*(GAME_HEIGHT - 1));
        text.setString(stream.str());
    }

    static void UpdateTimer(const Game& game, sf::Text& text)
    {
        std::ostringstream stream;
        if(game.getCheater())
        { 
            text.setFillColor(BAD_COLOR);
            stream << "(Cheats Used)\n";
        } else {
            text.setFillColor(GOOD_COLOR);
        }

        stream << std::setprecision(4) << std::fixed;
        stream << "Time: " << (double(game.getFrame()) / double(GAME_FPS)) << "s\n";
        stream << "Total Coins: " << game.getCoins() << " / " << game.getMaxCoins() << '\n';
        if(!game.getWinner())
        {   
            stream << "Level Coins: " 
                << game.getLevelCoins(game.getLevel()) << " / " 
                << game.getLevelMaxCoins(game.getLevel())  << '\n';
        }

        stream << "Deaths: " << game.getDeaths() << '\n';
        text.setString(stream.str());
        text.setPosition(6 + std::max(GAME_SCALE * (1 + START_SIZE - game.getCameraX()), 0), 0);
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

            const IntType startLevel = std::max(game.getFinalLevel() - 10, IntType(1));
            const IntType endLevel = game.getWinner() ? game.getFinalLevel() : game.getLevel() - 1;
            
            for(IntType i = startLevel; i <= endLevel; ++i)
            {
                // Balance Level Sign
                stream << "L" << i << ": ";
                if(i < 10) { stream << " "; }
                
                // Calculate Time
                stream << double(game.getLevelFrame(i)) / double(GAME_FPS) << 's';
                stream << " (" << game.getLevelCoins(i) << " / " << game.getLevelMaxCoins(i) << ')';
                stream << '\n';
            }

            // Print times to leader board
            text.setString(stream.str());
        } else
        { text.setString(""); }
    }
}

#endif