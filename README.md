# Upside-Down

## To compile, cd to the root of this project folder on your computer, and run

Linux: `g++ -O3 -o UpsideDown.o ./src/*.cpp -lsfml-window -lsfml-system -lsfml-graphics -std=c++11` 

Mac: `clang++ -O3 -o UpsideDown.o ./src/*.cpp -framework sfml-window -framework sfml-graphics -framework sfml-system -std=c++11`

## To compile the level builder, run

Linux: `g++ -O3 -o LevelBuilder.o ./src/LevelBuilder/*.cpp -lsfml-window -lsfml-system -lsfml-graphics` 

Mac: `clang++ -O3 -o LevelBuilder.o ./src/LevelBuilder/*.cpp -framework sfml-window -framework sfml-graphics -framework sfml-system -std=c++11`

## To run the game run `./UpsideDown.o` and `LevelBuilder.o` for the level builder

### You will need https://www.sfml-dev.org/download.php
