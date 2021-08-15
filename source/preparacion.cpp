#include "preparacion.h"
#include "globales.h"
#include "foursquares.h"
#include <iostream>
using namespace std;

Preparacion::Preparacion(){
    cuentaRegresiva.leerObjetoDesdeArchivo( "../recursos/imagenes/texto/cuenta.png" );
    cuentaRegresiva.escribirDimensionesTextura( 0, 0, 82, 157 );
    cuentaRegresiva.escribirEspacioY( 1 );

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
    cuentaRegresiva.escribirEspacioX( ( espacioAncho - cuentaRegresiva.leerEspacioAncho() ) / 2 );
    SDL_Rect trect = { contador * 82, 0, 82, 157 };
    cuentaRegresiva.escribirDimensionesTextura( trect );

    if( temporizador.obtenerTicks() > 1000 ){
        // Incrementa el contador
        contador++;

        if( contador > 2 ){
            contador = 2;
            objYa.show( true );
            objBloque.show( true );
            tiempoPartida.reanudar();
	        indicadorTiempo.reanudar(); 
	        tiempoEntradaBajada.reanudar();
	        tiempoEntradaLaterales.reanudar();
            tiempoAnimacion.iniciar();
            Juego_FinalizarEstado();
        }
        temporizador.reiniciar();
    }
}

void Preparacion::estadoRenderizado(){
    // Dibuja un fondo negro
    Juego_FondoNegro();

    // Dibuja la cuenta regresiva
    cuentaRegresiva.renderTexture( cuentaRegresiva.leerDimensionesTextura(), cuentaRegresiva.leerDimensionesEspacio() );
}

void Preparacion::actualizarViewport(){
    // Actuliza la textura de cuenta regresiva
    cuentaRegresiva.actualizarDimensionesAbsolutas();
}

Objeto cuentaRegresiva;

Temporizador temporizador;
int contador;