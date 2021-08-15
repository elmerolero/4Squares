#ifndef PREPARACION_H
#define PREPARACION_H
#include "estadojuego.h"
#include "objeto.h"
#include "globales.h"
#include "temporizador.h"

class Preparacion : public EstadoJuego
{
    public:
        Preparacion();
        ~Preparacion();

        void estadoEntrada();
        void estadoEventos( SDL_Event &gGameEvent );
		void estadoLogica();
		void estadoRenderizado();
		void actualizarViewport();
};

extern int contador;
extern Temporizador temporizador;
extern Objeto cuentaRegresiva;

#endif