#include "preparacion.h"
#include <iostream>
#include "game.h"
#include "foursquares.h"
#include "texture.h"
using namespace std;

Preparacion::Preparacion(){
    if( cuentaRegresiva.loadFileTexture( "../recursos/img/texto/cuenta.png" ) ){
        SDL_Rect trect = { 0, 0, 82, 157 };
        SDL_DRect rrect = { 0, 1, ( (float)trect.w * 6.13 ) / 1080, ( (float)trect.h * 6.13 ) / 1080 };
        cuentaRegresivaObjeto.setRelativeCoords( rrect );
	    cuentaRegresivaObjeto.setTextureCoords( trect );
    }

	tiempoPartida.pausar();
	gameTimer.pausar(); 
	tiempoEntradaBajada.pausar();
	tiempoEntradaLaterales.pausar();

    temporizador.iniciar();
    contador = 0;

    actualizarViewport();
}

Preparacion::~Preparacion(){

}

void Preparacion::estadoEntrada(){
    if( SDL_PollEvent( &gGameEvent ) != 0 ){
        if( gGameEvent.type == SDL_QUIT ){
			jSalir = true;
		}
        else if( gGameEvent.type == SDL_KEYDOWN && !arribaPresionado ){
			if( gGameEvent.key.keysym.sym == SDLK_UP ){
				arribaPresionado = true;
			}
			else if( gGameEvent.key.keysym.sym == SDLK_RETURN ){
				FS_Pausar();
			}
			else if( gGameEvent.key.keysym.sym == SDLK_ESCAPE ){
				Juego_EstablecerPantallaCompleta( !jPantallaCompleta );
			}
			else if( gGameEvent.key.keysym.sym == SDLK_f ){
				jMostrarTasaCuadros = !jMostrarTasaCuadros;
			}
		}
		else if( gGameEvent.type == SDL_WINDOWEVENT ){
			if( gGameEvent.window.event == SDL_WINDOWEVENT_RESIZED ){
				Juego_ActualizarVentana();
			}
		}
    }
}

void Preparacion::estadoLogica(){
    cuentaRegresivaObjeto.setRelativeX( ( gameViewport.w - cuentaRegresivaObjeto.getRelativeW() ) / 2 );
    SDL_Rect trect = { contador * 82, 0, 82, 157 };
    cuentaRegresivaObjeto.setTextureCoords( trect );
    if( temporizador.obtenerTicks() > 1000 ){
        contador++;
        if( contador > 2 ){
            contador = 2;
            tiempoPartida.reanudar();
	        gameTimer.reanudar(); 
	        tiempoEntradaBajada.reanudar();
	        tiempoEntradaLaterales.reanudar();
            EstadoJuego_Salir();
        }
        temporizador.reiniciar();
    }
}

void Preparacion::estadoRenderizado(){
    SDL_SetRenderDrawColor( gPtrRenderer, 0x00, 0x00, 0x00, 0x88 );
    SDL_RenderFillRect( gPtrRenderer, &fondoNegro );

    cuentaRegresiva.renderTexture( cuentaRegresivaObjeto.getSrcRect(), cuentaRegresivaObjeto.getDestRect() );
}

void Preparacion::actualizarViewport(){
    fondoNegro = { 0, 0, jAnchoPantalla, jAltoPantalla };
    cuentaRegresivaObjeto.actualizarCoordanadasAbsolutas();
    yaObjeto.actualizarCoordanadasAbsolutas();
}

Texture cuentaRegresiva;
Object cuentaRegresivaObjeto;
SDL_Rect fondoNegro;

Temporizador temporizador;
int contador;