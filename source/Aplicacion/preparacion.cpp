#include "preparacion.h"
#include "globales.h"
#include "foursquares.h"
#include <iostream>
using namespace std;

Preparacion::Preparacion(): nombre( "Preparacion" ){
	tiempoPartida.pausar();
	indicadorTiempo.pausar(); 
	tiempoEntradaBajada.pausar();
	tiempoEntradaLaterales.pausar();

    temporizador.iniciar();
    contador = 0;

    objBloque.show( false );

    objYa.show( false );
    actualizarViewport();
}

Preparacion::~Preparacion(){

}

void Preparacion::estadoEntrada(){
    // No hay entradas que procesar
}

void Preparacion::estadoEventos( SDL_Event &gGameEvent ){
    // No hay eventos que procesar
}

void Preparacion::estadoLogica(){
    // Actualiza las dimensiones de la textura
    objCuentaRegresiva.escribirTexturaX( contador * 82 );
    objCuentaRegresiva.escribirEspacioX( ( espacioAncho - objCuentaRegresiva.leerEspacioAncho() ) / 2 );

    if( temporizador.obtenerTicks() > 1000 ){
        // Incrementa el contador
        contador++;

        if( contador > 2 ){
            contador = 2;

            // Inicializa los parametros de los jugadores
            for( size_t contador = 0; contador < numeroJugadores; ++contador ){
                Jugador_Iniciar( jugadores[ contador ] );
	        }

            objYa.show( true );
            objBloque.show( true );
            tiempoPartida.reanudar();
	        /* indicadorTiempo.reanudar(); 
	        tiempoEntradaBajada.reanudar();
	        tiempoEntradaLaterales.reanudar();
            tiempoAnimacion.iniciar(); */
            Juego_EstablecerEstado( nullptr, ESTADO_FINALIZAR );
        }
        temporizador.reiniciar();
    }
}

void Preparacion::estadoRenderizado(){
    // Dibuja un fondo negro
    Juego_FondoNegro();

    // Dibuja la cuenta regresiva
    objCuentaRegresiva.renderTexture( objCuentaRegresiva.leerDimensionesTextura(), objCuentaRegresiva.leerDimensionesEspacio() );
}

void Preparacion::actualizarViewport(){
    // Actuliza la textura de cuenta regresiva
    objCuentaRegresiva.actualizarDimensionesAbsolutas();
}

void Preparacion::escribirNombre( std::string nombre ){
    this -> nombre = nombre;
}

void Preparacion::mostrarNombre( void ){
    cout << nombre << endl;
}

Temporizador temporizador;
int contador;