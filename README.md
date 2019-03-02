# Upside-Down

## To compile, run this command at the root of the folder

**Linux (SFML REQUIRED):** `clang++ -o UpsideDown.out ./src/*.cpp -lsfml-window -lsfml-system -lsfml-graphics -lsfml-audio -std=c++17 -O3` 

**Mac (SFML REQUIRED):** `clang++ -o UpsideDown.out ./src/*.cpp -framework sfml-window -framework sfml-graphics -framework sfml-system -framework sfml-graphics -std=c++17 -O3`

**Windows:** Extract the DLLs into the root of the project and run the exe.

*This game relys on other files in the project folder, keep everything where it is*

To run the game run `./UpsideDown.out` 

#### You will need https://www.sfml-dev.org/download.php to compile the game

## What is a `.lvl`

I don't have all day to be making this game and its levels, so I made a level editor. 

You can access this by pressing `Ctrl + Shift + E`. 

A `.lvl` Is the file type that stores these levels. Here is a diagram of a `.lvl` file:

```
[32bit Magic Number] = 0x53616d42 // Spells "SamB" <--- My Initials
[32bit Height]
[32bit Length]
[GAME DATA] ...
[GAME DATA] ...
[GAME DATA] ...
[GAME DATA] ...
[GAME DATA] ...
```

The game data is a series of 8bit numbers that indicate what type of block they are. Data in the file is stored in little endian, and thus needs conversion for most prossesors.

## DEV ONLY

**Cross Compile Linux to Windows:** `i686-w64-mingw32-g++ -O3 ./src/*.cpp -o UpsideDown.exe -static-libgcc -static-libstdc++ -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio`