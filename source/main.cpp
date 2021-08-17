#include <cstdlib>
#include <iostream>
#include "juego.h"
#include "foursquares.h"
#include "preparacion.h"

using namespace std;

int main( int argc, char * args[] )
{
	// Inicializa el juego
	Juego_Iniciar( "Four Squares", "../recursos/datos/es-mx.dat" );
	Juego_CargarMedia();
	Juego_EstablecerEstado( new FourSquares() );
	Juego_ApilarEstado( new Preparacion() );

	// Inicializa el juego
	while( !salir ){
		// Lee la entrada
		Juego_Entrada();

		// Actualiza el estado
		Juego_Logica();
		
		// Dibuja la pantalla
		Juego_Renderizar();

		// Revisa si hubo un cambio en el estado del juego
		Juego_ActualizarEstado();
	}

	Juego_Cerrar();
	
	return 0;
}
