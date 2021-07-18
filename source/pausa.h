#ifndef PAUSA_H
#define PAUSA_H

#include "game.h"
#include "gamestate.h"
#include "object.h"
#include "texture.h" 

class Pausa : public EstadoJuego
{
    public:
        Pausa();
        void estadoEntrada();
        void estadoLogica();
        void estadoRenderizado();
        void actualizarViewport();

    private:

};

void Pausa_SeleccionarOpcion( void );
void Pausa_DibujarOpciones( void );

// Selector de opcion
extern int opcionPausa;
const int NUMERO_OPCIONES = 3;

// Opciones
const int OPCION_CONTINUAR = 1;
const int OPCION_REINICIAR = 2;
const int OPCION_SALIR = 3;

// Elementos para dibujar las opciones
extern const char *opciones[];
const SDL_Rect opcionPausaRect[] = { { 0, 0, 340, 85 }, { 0, 85, 340, 85 } };
extern Texture opcionPausaTextura;
extern Object opcionPausaObjeto;
extern Texture opcionTextoTextura;
extern Object opcionTextoObjeto;

// Fondo de pausa
extern SDL_Rect fondoPausa;

// Letrero que indica pausa
extern Texture letreroPausaTextura;
extern Object letreroPausaObjeto;

#endif