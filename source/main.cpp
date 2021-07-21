#include <cstdlib>
#include <iostream>
#include "game.h"
using namespace std;

int main( int argc, char * args[] )
{
	// Inicializa el juego
	jSalir = !Juego_Iniciar( "4Squares" );

	while( !jSalir ){
		SDL_Delay( 1 );
		EstadoJuego_Salir( estadosJuego );
		EstadoJuego_Entrada();
		EstadoJuego_Logica();
		EstadoJuego_Renderizar();
	}
	
	return 0;
}
