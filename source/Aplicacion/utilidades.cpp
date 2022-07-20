#include "utilidades.h"

SDL_Rect obtenerRectTextura( Texture &textura ){
    // Declara el rect de textura
    SDL_Rect rectTextura;

    // Establece las dimensiones
    rectTextura.x = 0;
    rectTextura.y = 0;
    rectTextura.w = textura.getWidth();
    rectTextura.h = textura.getHeight();

    // Devuelve la referencia
    return rectTextura;
}

SDL_DRect obtenerRectRelativo( Texture &textura ){
    // Declara el rect relativo
    SDL_DRect rectRelativo;

    // Establece las dimensiones
    rectRelativo.x = 0;
    rectRelativo.y = 0;
    rectRelativo.w = ( (float)textura.getWidth() * 10 ) / 1080;
    rectRelativo.h = ( (float)textura.getHeight() * 10 ) / 1080;

    // Devuelve la referencia
    return rectRelativo;
}

double unidadesRelativas( int dato ){
    return ( (float)dato * 10 ) / 1080;
}