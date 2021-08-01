#include "preparacion.h"
#include <iostream>
#include "foursquares.h"
#include "texture.h"
using namespace std;

Preparacion::Preparacion(){
    if( cuentaRegresiva.loadFileTexture( "../recursos/imagenes/texto/cuenta.png" ) ){
        SDL_Rect trect = { 0, 0, 82, 157 };
        SDL_DRect rrect = { 0, 1, ( (float)trect.w * 6.13 ) / 1080, ( (float)trect.h * 6.13 ) / 1080 };
        cuentaRegresiva.escribirDimensionesEspaciales( rrect );
	    cuentaRegresiva.escribirDimensionesTextura( trect );
    }

	tiempoPartida.pausar();
	indicadorTiempo.pausar(); 
	tiempoEntradaBajada.pausar();
	tiempoEntradaLaterales.pausar();

    temporizador.iniciar();
    contador = 0;

    tetroBlock.show( false );

    ya.show( false );
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
    cuentaRegresiva.escribirEspacialX( ( fourSquares.leerEspacioAncho() - cuentaRegresiva.leerEspacialAncho() ) / 2 );
    SDL_Rect trect = { contador * 82, 0, 82, 157 };
    cuentaRegresiva.escribirDimensionesTextura( trect );

    if( temporizador.obtenerTicks() > 1000 ){
        // Incrementa el contador
        contador++;

        if( contador > 2 ){
            contador = 2;
            ya.show( true );
            tetroBlock.show( true );
            tiempoPartida.reanudar();
	        indicadorTiempo.reanudar(); 
	        tiempoEntradaBajada.reanudar();
	        tiempoEntradaLaterales.reanudar();
            tiempoAnimacion.iniciar();
            fourSquares.finalizarEstado();
        }
        temporizador.reiniciar();
    }
}

void Preparacion::estadoRenderizado(){
    // Dibuja un fondo negro
    fourSquares.fondoNegro();

    // Dibuja la cuenta regresiva
    cuentaRegresiva.renderTexture( cuentaRegresiva.leerDimensionesTextura(), cuentaRegresiva.leerDimensionesEspaciales() );
}

void Preparacion::actualizarViewport(){
    // Actuliza la textura de cuenta regresiva
    cuentaRegresiva.actualizarDimensionesAbsolutas();
}

Objeto cuentaRegresiva;

Temporizador temporizador;
int contador;