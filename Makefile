CC=g++
LINKER_FLAGS= -lmingw32 -mwindows -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
CFLAGS= -IC:/msys64/mingw64/include/SDL2

Tetroblock: game.o temporizador.o texture.o tetroblock.o object.o
	$(CC) gamestate.h main.cpp $(CFLAGS) -o Tetroblock game.o object.o texture.o temporizador.o tetroblock.o $(LINKER_FLAGS)

tetroblock.o: tetroblock.h tetroblock.cpp
	$(CC) tetroblock.h tetroblock.cpp $(CFLAGS) -c
	