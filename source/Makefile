CC=g++
LINKER_FLAGS= -mwindows -lmingw32 -lSDL2main -lsqlite3 -lSDL2 -lSDL2_image -lSDL2_ttf
CFLAGS= -Iinclude/SDL2 -Ilib/sqlite3
OBJETOS= globales.o juego.o utilidades.o database.o temporizador.o objeto.o foursquares.o texture.o preparacion.o pausa.o derrota.o
../build/aplicacion: main.cpp $(OBJETOS)
	$(CC) estadojuego.h main.cpp $(CFLAGS) -g -o ../build/aplicacion $(OBJETOS) $(LINKER_FLAGS)

foursquares.o: foursquares.h foursquares.cpp
	$(CC) foursquares.h foursquares.cpp $(CFLAGS) -g -c

preparacion.o: preparacion.h preparacion.cpp
	$(CC) preparacion.h preparacion.cpp $(CFLAGS) -c

pausa.o: pausa.h pausa.cpp
	$(CC) pausa.h pausa.cpp $(CFLAGS) -c

derrota.o: derrota.h derrota.cpp
	$(CC) derrota.h derrota.cpp $(CFLAGS) -c

utilidades.o: utilidades.h utilidades.cpp
	$(CC) utilidades.h utilidades.cpp $(CFLAGS) -c

texture.o: texture.h texture.cpp
	$(CC) texture.h texture.cpp $(CFLAGS) -c

database.o: database.h database.cpp
	$(CC) database.h database.cpp $(CFLAGS) -c

juego.o: juego.h juego.cpp
	$(CC) juego.h juego.cpp $(CFLAGS) -c

temporizador.o: temporizador.h temporizador.cpp
	$(CC) temporizador.h temporizador.cpp $(CFLAGS) -c

objeto.o: objeto.h objeto.cpp
	$(CC) objeto.h objeto.cpp $(CFLAGS) -c

globales.o: globales.h globales.cpp
	$(CC) globales.h globales.cpp $(CFLAGS) -c
