#include "inicio.h"
#include "juego.h"
#include "foursquares.h"
#include "preparacion.h"
#include <iostream>
using namespace std;

// Constructor
Inicio::Inicio(): startPresionado( false ), transparencia( 0 ), nombre( "Presentacion" ){
    // Actualiza el viewport
    actualizarViewport();

    // Inicia los temporizadores
    tiempoAtenuacion.iniciar();
}

void Inicio::estadoEntrada(){
    // Vacío
}

void Inicio::estadoEventos( SDL_Event &gGameEvent ){
    if( joystickConectado && gGameEvent.type == SDL_JOYBUTTONDOWN ){
        if( gGameEvent.cbutton.button == SDL_CONTROLLER_BUTTON_START ){
			startPresionado = true;
		}
    }
    else if( gGameEvent.type == SDL_KEYDOWN ){
        if( gGameEvent.key.keysym.sym == SDLK_RETURN ){
            startPresionado = true;
        }
    }
}

void Inicio::estadoLogica(){
	if( startPresionado && tiempoAtenuacion.obtenerTicks() > 15 ){
        transparencia += 5;
        if( transparencia >= 255 ){
            transparencia -= 5;

            Juego_EstablecerEstado( new FourSquares(), ESTADO_ESTABLECER );
	        Juego_EstablecerEstado( new Preparacion(), ESTADO_APILAR );
        }

        tiempoAtenuacion.reiniciar();
    }
}

void Inicio::estadoRenderizado(){
    SDL_Rect rect = { 0, 0, pantallaAncho, pantallaAlto }; 
    objFondoInicio.renderTexture( objFondoInicio.leerDimensionesTextura(), objFondoInicio.leerDimensionesEspacio() );
    SDL_SetRenderDrawColor( gRender, 0x00, 0x00, 0x00, transparencia );
    SDL_RenderFillRect( gRender, &rect );
}

void Inicio::actualizarViewport(){
    objFondoInicio.escribirTexturaW( ( ( (float)espacioAncho / (float)espacioAlto ) / Objeto::RELACION_ASPECTO ) * (float)objFondoInicio.getWidth() );
	objFondoInicio.escribirTexturaX( ( objFondoInicio.getWidth() - objFondoInicio.leerTexturaW() ) / 2 );
	objFondoInicio.escribirDimensionesEspacio( espacioX, espacioY, espacioAncho, espacioAlto );
}

void Inicio::escribirNombre( std::string nombre ){
    this -> nombre = nombre;
}

void Inicio::mostrarNombre( void ){
    cout << nombre << endl;
}