#ifndef OPCIONES_H
#define OPCIONES_H

#include "game.h"
#include "object.h"
#include "texture.h"

// Indica el fragmento de la textura que representa la opción
#define OPCION 0
#define SELECCION 1

struct Opciones{
    int numero;
    int seleccionada;
};

void Opciones_Dibujar( Opciones &informacionOpciones, const char *opciones[], Texture &opcionTextura, Objeto &opcionObjeto, Texture &textoTextura );

#endif