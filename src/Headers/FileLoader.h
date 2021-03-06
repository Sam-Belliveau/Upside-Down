#ifndef LEVEL_FILE_LOADER_H
#define LEVEL_FILE_LOADER_H

#include "./Constants.h"

namespace Loader
{
    struct HeaderData
    {
    private: 
        static void SaveNumber(Byte* arr, RawIntType num)
        {
            arr[0] = (num >> 0)  & 0xff;
            arr[1] = (num >> 8)  & 0xff;
            arr[2] = (num >> 16) & 0xff;
            arr[3] = (num >> 24) & 0xff;
        }

        static RawIntType ReadNumber(const Byte* arr)
        {
            RawIntType out = 0;
            out |= arr[0] << 0;
            out |= arr[1] << 8;
            out |= arr[2] << 16;
            out |= arr[3] << 24;
            return out;
        }

        Byte header[12];

    public: 
        HeaderData() {}
        HeaderData(RawIntType magicNumber, RawIntType height, RawIntType length)
        {
            SaveNumber(&header[0], magicNumber);
            SaveNumber(&header[4], height);
            SaveNumber(&header[8], length);
        }

        char* getHeaderData() { return reinterpret_cast<char*>(header); }
        const char* getHeaderData() const { return reinterpret_cast<const char*>(header); }
        RawIntType getMagicNumber() const { return ReadNumber(&header[0]); }
        RawIntType getHeight() const { return ReadNumber(&header[4]); }
        RawIntType getLength() const { return ReadNumber(&header[8]); }
    };

    static bool LoadWorld(const IntType inLevel, GameType world[][GAME_HEIGHT], bool clearWorld = true)
    {
        std::ifstream levelFile(LEVEL_FOLDER + LEVEL_PREFIX + std::to_string(inLevel) + LEVEL_EXTENTION);
        if(levelFile.good())
        {
            HeaderData header;
            levelFile.read(reinterpret_cast<char*>(header.getHeaderData()), sizeof(header));
            if(header.getMagicNumber() != MAGIC_NUMBER)
            {
                levelFile.close();
                return false;
            }

            if(header.getLength() == GAME_LENGTH && header.getHeight() == GAME_HEIGHT)
            {
                // If Headers Match Game Settings, read directly
                levelFile.read(reinterpret_cast<char*>(world), GAME_LENGTH*GAME_HEIGHT*sizeof(GameType));
            } else
            {
                // If Not, do conversions
                const RawIntType HeaderLength = header.getLength();
                const RawIntType HeaderHeight = header.getHeight();
                const RawIntType GameLength = std::min(GAME_LENGTH, HeaderLength);
                const RawIntType GameHeight = std::min(GAME_HEIGHT, HeaderHeight);
                for(RawIntType x = 0; x < GAME_LENGTH; ++x)
                {
                    if(x < GameLength)
                    {
                        levelFile.read(reinterpret_cast<char*>(&world[x][0]), GameHeight);
                        for(RawIntType y = GameHeight; y < GAME_HEIGHT; ++y)
                            world[x][y] = GameType::Trap; 
                    } else 
                    {
                        for(RawIntType y = 0; y < GAME_HEIGHT; ++y)
                            world[x][y] = GameType::Goal;
                    }

                    if(GAME_HEIGHT < HeaderHeight) levelFile.ignore(HeaderHeight - GAME_HEIGHT);
                }
            }

            levelFile.close();
            return true;
        } else 
        {
            if(clearWorld)
            {
                for(RawIntType x = 0; x < GAME_LENGTH; ++x)
                {
                    for(RawIntType y = 0; y < GAME_HEIGHT; ++y)
                    {
                        if(x <= START_SIZE && y <= 7)
                        {
                            if(x == START_SIZE || y == 7)
                                world[x][y] = GameType::Trap;
                            else world[x][y] = GameType::Sky;
                        } else world[x][y] = (y >= GAME_HEIGHT-3) ? GameType::Ground : GameType::Sky;
                    }
                }
            }
            levelFile.close();
            return false;
        }

    }

    static void SaveWorld(const IntType inLevel, GameType world[][GAME_HEIGHT])
    {
        const HeaderData header(MAGIC_NUMBER, GAME_HEIGHT, GAME_LENGTH);

        std::ofstream levelFile(LEVEL_FOLDER + LEVEL_PREFIX + std::to_string(inLevel) + LEVEL_EXTENTION, std::ios::binary);
        levelFile.write(reinterpret_cast<const char*>(header.getHeaderData()), sizeof(header));
        levelFile.write(reinterpret_cast<const char*>(world), GAME_LENGTH*GAME_HEIGHT*sizeof(GameType));
        levelFile.close();
    }
}

#endif