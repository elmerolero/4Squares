#include <cstdlib>
#include <iostream>
using namespace std;
#include "game.h"
#include "foursquares.h" 

int main( int argc, char * args[] )
{
	// Inicializa el juego
	jSalir = !Juego_Iniciar( "4Squares" );

	while( !jSalir ){
		estadoJuego -> estadoEntrada();
		estadoJuego -> estadoLogica();
		estadoJuego -> estadoRenderizado();
	}
	
	return 0;
}
