#ifndef ESTADO_JUEGO_H
#define ESTADO_JUEGO_H
#include <SDL2/SDL.h>
#include <string>

class EstadoJuego
{
	public:
		virtual void estadoEntrada() = 0;
		virtual void estadoEventos( SDL_Event &gGameEvent ) = 0;
		virtual void estadoLogica() = 0;
		virtual void estadoRenderizado() = 0;
		virtual void actualizarViewport() = 0;
		virtual void escribirNombre( std::string nombre ) = 0;
		virtual void mostrarNombre( void ) = 0;
};

enum EstadoAccion{
	ESTADO_ESTABLECER = 1,
	ESTADO_APILAR,
	ESTADO_FINALIZAR, 
	ESTADO_SALIR,
};

struct Pendiente{
    EstadoJuego *estado;
    EstadoAccion accion;
};

#endif
