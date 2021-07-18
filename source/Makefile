CC=g++
LINKER_FLAGS= -lmingw32 -mwindows -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -mwindows
CFLAGS= -IC:/msys64/mingw64/include/SDL2
OBJETOS= game.o object.o texture.o temporizador.o foursquares.o preparacion.o pausa.o

../build/aplicacion: main.cpp $(OBJETOS)
	$(CC) gamestate.h main.cpp $(CFLAGS) -o ../build/aplicacion $(OBJETOS) $(LINKER_FLAGS)

foursquares.o: foursquares.h foursquares.cpp
	$(CC) foursquares.h foursquares.cpp $(CFLAGS) -c

game.o: game.h game.cpp
	$(CC) game.h game.cpp $(CFLAGS) -c

preparacion.o: preparacion.h preparacion.cpp
	$(CC) preparacion.h preparacion.cpp $(CFLAGS) -c

pausa.o: pausa.h pausa.cpp
	$(CC) pausa.h pausa.cpp $(CFLAGS) -c