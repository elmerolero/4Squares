#include <cstdlib>
#include <iostream>
#include "juego.h"
#include "foursquares.h"
#include "preparacion.h"
#include "inicio.h"

using namespace std;

int main( int argc, char * args[] )
{
	// Inicializa el juego
	Juego_Iniciar( "Four Squares", "../recursos/datos/es-mx.dat" );
	Juego_CargarMedia( "480p" );
	Juego_EstablecerEstado( new Inicio(), ESTADO_ESTABLECER );
	Juego_ActualizarEstado();

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
