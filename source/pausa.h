#ifndef PAUSA_H
#define PAUSA_H

#include "estadojuego.h"
#include "preparacion.h"
#include "componentes.h"
#include "objeto.h"

class Pausa : public EstadoJuego
{
    public:
        Pausa();
        void estadoEntrada();
        void estadoEventos( SDL_Event &gGameEvent );
        void estadoLogica();
        void estadoRenderizado();
        void actualizarViewport();

    private:

};

void Pausa_SeleccionarOpcion( void );
void Pausa_DibujarOpciones( void );

// Selector de opcion
extern int opcion;
const int NUMERO_OPCIONES = 3;

// Opciones
const int OPCION_CONTINUAR = 1;
const int OPCION_REINICIAR = 2;
const int OPCION_SALIR = 3;

// Elementos para dibujar las opciones
extern const char *opciones[];
const SDL_Rect opcionPausaRect[] = { { 0, 0, 340, 85 }, { 0, 85, 340, 85 } };
extern Objeto opcionPausa;
extern Objeto opcionTexto;

// Letrero que indica pausa
extern Objeto letreroPausa;

#endif