#ifndef UTILIDADES_H
#define UTILIZADES_H

#include "game.h"
#include "texture.h"

SDL_Rect obtenerRectTextura( Texture &textura );
SDL_DRect obtenerRectRelativo( Texture &textura );
double unidadesRelativas( int dato );

#endif