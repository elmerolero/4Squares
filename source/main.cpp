#include <cstdlib>
#include <iostream>
#include "juego.h"
#include "componentes.h"
#include "foursquares.h"
#include "preparacion.h"

using namespace std;

int main( int argc, char * args[] )
{
	// Inicializa el juego
	fourSquares.iniciar( "Four Squares", "../recursos/datos/es-mx.dat" );

	fourSquares.establecerEstado( new FourSquares() );
	fourSquares.apilarEstado( new Preparacion() );

	// Inicializa el juego
	while( !fourSquares.salir() ){
		
		// Genera retraso de un milisegundo
		SDL_Delay( 1 );

		// Lee la entrada
		fourSquares.entrada();

		// Actualiza el estado
		fourSquares.logica();
		
		// Dibuja la pantalla
		fourSquares.renderizar();

		// Revisa si hubo un cambio en el estado del juego
		fourSquares.actualizarEstado();
	}

	fourSquares.cerrar();
	
	return 0;
}
