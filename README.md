
# 4Squares
4Squares es un juego basado en el juego Tetris Party Deluxe y cuyo principal objetivo es ser multiplataforma.

# Instalación
Puedes descargar el juego para Windows en [mi pagina web](https://micuarto.000webhostapp.com).

# Compilación
Es necesario la instalación de las siguientes librerías en Linux
 - SDL2: -libsdl2-dev
 - SDL_image: -libsdl2-image-dev
 - SDL_ttf: -libsdl2-ttf-dev
 - sqlite3: -libsqlite3-dev

Para compilarlo sigue las siguientes instrucciones
- git clone http://github.com/elmerolero/4Squares
- cd 4Squares
- mkdir build
- cd source
- make
- ./../build/aplicacion

NOTA: Para sistemas operativos Linux es necesario remover del Makefile las siguientes banderas (que son de windows)
 - -mwindows -lmingw32 -lSDL2main
 - -IC:/msys64/mingw64/include/SDL2

## License
[MIT](https://choosealicense.com/licenses/mit/)