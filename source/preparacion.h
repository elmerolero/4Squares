#ifndef PREPARACION_H
#define PREPARACION_H
#include "gamestate.h"
#include "texture.h"
#include "object.h"
#include "temporizador.h"

class Preparacion : public EstadoJuego
{
    public:
        Preparacion();
        ~Preparacion();

        void estadoEntrada();
		void estadoLogica();
		void estadoRenderizado();
		void actualizarViewport();
};

extern int contador;
extern Temporizador temporizador;
extern Texture cuentaRegresiva;
extern Object cuentaRegresivaObjeto;
extern SDL_Rect fondoNegro;

#endif