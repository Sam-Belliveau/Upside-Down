# Upside-Down

## To compile, cd to the root of this project folder on your computer, and run

Linux: `clang++ -o UpsideDown.out ./src/*.cpp -lsfml-window -lsfml-system -lsfml-graphics -std=c++17 -O3` 

Mac: `clang++ -o UpsideDown.out ./src/*.cpp -framework sfml-window -framework sfml-graphics -framework sfml-system -std=c++17 -O3`

Cross Compile Linux to Windows: `i686-w64-mingw32-g++ -O3 -o UpsideDown.o ./src/*.cpp -o UpsideDown.exe -static-libgcc -static-libstdc++ -static -mwindows -L /usr/i686-w64-mingw32/lib/ -I /usr/i686-w64-mingw32/include/ -lpthread -DSFML_STATIC -lsfml-graphics-s -lsfml-window-s -lsfml-system-s`
OR 
Windows: Extract the DLLs into the root of the project and run the exe.

To run the game run `./UpsideDown.out` 

#### You will need https://www.sfml-dev.org/download.php

## What is a `.udl`

I don't have all day to be making this game and its levels, so I made a level editor. You can access this by pressing `Ctrl + Shift + E`. A `.udl` Is the file type that stores these levels. Here is a diagram of a `.udl` file

```
[32bit Magic Number] = 0x01020304
[32bit Height]
[32bit Length]
[GAME DATA]
[GAME DATA]
[GAME DATA]
```

The game data is a series of 8bit numbers that indicate what type of block they are.
