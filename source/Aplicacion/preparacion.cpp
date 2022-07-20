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
    objCuentaRegresiva.escribirEspacioX( ( espacioAncho - objCuentaRegresiva.leerEspacioAncho() ) / 2 );
    objCuentaRegresiva.escribirDesplazamientoX( contador );

    actualizarViewport();
}

Preparacion::~Preparacion(){

}

void Preparacion::estadoEntrada(){
    // Espera un segundo
    if( temporizador.obtenerTicks() > 1000 ){
        // Incrementa el contador
        contador++;

        // Reinicia el temporizador
        temporizador.reiniciar();
    }
}

void Preparacion::estadoEventos( SDL_Event &gGameEvent ){
    // No hay eventos
}

void Preparacion::estadoLogica(){
    // Si el contador ya va en el último frame
    if( contador > 2 ){
        // Impide que el contador vaya más lejos
        contador = 2;

        // Inicializa los parametros de los jugadores
        for( size_t contador = 0; contador < numeroJugadores; ++contador ){
            Jugador_Iniciar( jugadores[ contador ] );
        }

        // Inicializa la partida
        objYa.show( true );
        objBloque.show( true );
        tiempoPartida.reanudar();
        Juego_EstablecerEstado( nullptr, ESTADO_FINALIZAR );
    }

    // Actualiza el frame en el que va
    objCuentaRegresiva.escribirDesplazamientoX( contador );
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