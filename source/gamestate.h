#ifndef GAME_STATE_H
#define GAME_STATE_H

class EstadoJuego
{
	public:
		virtual void estadoEntrada() = 0;
		virtual void estadoLogica() = 0;
		virtual void estadoRenderizado() = 0;
};

#endif
