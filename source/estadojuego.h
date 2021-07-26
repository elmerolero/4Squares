#ifndef ESTADO_JUEGO_H
#define ESTADO_JUEGO_H
#include <SDL2/SDL.h>

class EstadoJuego
{
	public:
		virtual void estadoEntrada() = 0;
		virtual void estadoEventos( SDL_Event &gGameEvent ) = 0;
		virtual void estadoLogica() = 0;
		virtual void estadoRenderizado() = 0;
		virtual void actualizarViewport() = 0;
};

#endif
