CC=g++
LINKER_FLAGS= -lSDL2 -lSDL2_image -lSDL2_ttf

Tetroblock: game.o temporizador.o texture.o tetroblock.o object.o
	$(CC) $(LINKER_FLAGS) gamestate.h main.cpp -o Tetroblock game.o object.o texture.o temporizador.o tetroblock.o

