#ifndef PAUSA_H
#define PAUSA_H

#include "estadojuego.h"
#include "preparacion.h"
#include "globales.h"
#include "objeto.h"
#include <string>

class Pausa : public EstadoJuego
{
    public:
        Pausa();
        void estadoEntrada();
        void estadoEventos( SDL_Event &gGameEvent );
        void estadoLogica();
        void estadoRenderizado();
        void actualizarViewport();
        void escribirNombre( std::string nombre );
		void mostrarNombre( void );

    private:   
        std::string nombre;
};

void Pausa_SeleccionarOpcion( void );
void Pausa_DibujarOpciones( void );

extern int indice;

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