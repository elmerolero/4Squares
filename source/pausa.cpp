#include "pausa.h"
#include "texture.h"
#include "foursquares.h"
#include "utilidades.h"
#include <iostream>
using namespace std;

Pausa::Pausa()
{
    EstadoJuego_EsperarParaEvento( true );

    if( letreroPausaTextura.loadFileTexture( "../recursos/img/texto/pausa.png" ) ){
        // Establece sus dimensiones en pantalla
        SDL_Rect arect = { 0, 0, letreroPausaTextura.getWidth(), letreroPausaTextura.getHeight() };
        SDL_DRect rrect = { 0, 1, ( (float)arect.w * 6.13 ) / 1080, ( (float)arect.h * 6.13 ) / 1080 };
        letreroPausaObjeto.setTextureCoords( arect );
        letreroPausaObjeto.setRelativeCoords( rrect );
    }

    if( opcionPausaTextura.loadFileTexture( "../recursos/img/otros/selector-pausa.png" ) ){
        SDL_DRect rrect = { 0, 0, ( (float)opcionPausaRect[ 0 ].w * 6.13 ) / 1080, ( (float)opcionPausaRect[ 0 ].h * 6.13 ) / 1080 };
        opcionPausaObjeto.setRelativeCoords( rrect );
    }

    // Opcion pausa por defecto es la primera opcion
    opcionPausa = 1;

    actualizarViewport();
}

void Pausa::estadoEntrada(){
    
}

void Pausa::estadoEventos(){
    if( gGameEvent.type == SDL_KEYDOWN ){
        if( gGameEvent.key.keysym.sym == SDLK_UP ){
            // Cambia de opcion hacia arriba
            opcionPausa--;
            if( opcionPausa < 1 ){
                opcionPausa = NUMERO_OPCIONES;
            }
        }
        else if( gGameEvent.key.keysym.sym == SDLK_DOWN ){
            // Cambia de opción hacia abajo
            opcionPausa++;
            if( opcionPausa > NUMERO_OPCIONES ){
                opcionPausa = 1;
            }
        }
        else if( gGameEvent.key.keysym.sym == SDLK_RETURN ){
            Pausa_SeleccionarOpcion();
        }
    }
}

void Pausa::estadoLogica(){

}

void Pausa::estadoRenderizado(){
    // Dibuja el fondo de pantalla
    SDL_SetRenderDrawColor( gPtrRenderer, 0x00, 0x00, 0x00, 0x88 );
    SDL_RenderFillRect( gPtrRenderer, &fondoPausa );

    // Dibuja el letrero de pausa
    letreroPausaTextura.renderTexture( letreroPausaObjeto.getSrcRect(), letreroPausaObjeto.getDestRect() );

    // Dibuja las opciones
    Pausa_DibujarOpciones();
}

void Pausa::actualizarViewport()
{
    // Actualiza el tamaño del rectangulo del fondo
    fondoPausa = { 0, 0, jAnchoPantalla, jAltoPantalla };

    // Actualiza el letrero en pantalla
    letreroPausaObjeto.setRelativeX( ( gameViewport.w - letreroPausaObjeto.getRelativeW() ) / 2 );
    letreroPausaObjeto.actualizarCoordanadasAbsolutas();

    // Actualiza la posicion de las opciones en pantalla 
    opcionPausaObjeto.setRelativeX( ( gameViewport.w - opcionPausaObjeto.getRelativeW() ) / 2 );
    opcionPausaObjeto.actualizarCoordanadasAbsolutas();
}

void Pausa_SeleccionarOpcion( void ){
    if( opcionPausa == OPCION_CONTINUAR ){
        // Continúa con la partida
        EstadoJuego_EsperarParaEvento( false );
        FS_ReanudarPartida();
		EstadoJuego_Salir();
    }
    else if( opcionPausa == OPCION_REINICIAR ){
        // No hace nada
    }
    else if( opcionPausa == OPCION_SALIR ){
        // Sale del juego
        jSalir = true;
    }
}

void Pausa_DibujarOpciones( void ){
    // Obtiene la mitad de la pantalla
    double mediaY = gameViewport.h / 2;

    // Dibuja el las opciones disponibles
    for( int i = 0; i < NUMERO_OPCIONES; i++ ){
        double posicionY = ( opcionPausaObjeto.getRelativeH() + .2 ) * ( i ) + mediaY;
        opcionPausaObjeto.setRelativeY( posicionY );
        opcionPausaObjeto.actualizarCoordanadasAbsolutas();
        opcionPausaTextura.renderTexture( &opcionPausaRect[ 0 ], opcionPausaObjeto.getDestRect() );

        // ¿Es la opción seleccionada?
        if( opcionPausa == i + 1 ){
            opcionPausaTextura.renderTexture( &opcionPausaRect[ 1 ], opcionPausaObjeto.getDestRect() );
        }

        if( opcionTextoTextura.crearTexturaDesdeTextoSolido( opciones[ i ], COLOR_BLANCO, fuenteArg ) ){
            SDL_DRect rrect = { opcionPausaObjeto.getRelativeX() + 0.1, posicionY + 0.07, (float)opcionTextoTextura.getWidth() / gameUnitSize, (float)opcionTextoTextura.getHeight() / gameUnitSize };
            opcionTextoObjeto.setTextureCoords( obtenerRectTextura( opcionTextoTextura ) );
            opcionTextoObjeto.setRelativeCoords( rrect );
            opcionTextoTextura.renderTexture( opcionTextoObjeto.getSrcRect(), opcionTextoObjeto.getDestRect() );
        }
    }
}

int opcionPausa;
const char *opciones[] = { "Continuar", "Reiniciar", "Salir" };

SDL_Rect fondoPausa;

Texture letreroPausaTextura;
Object letreroPausaObjeto;

Texture opcionPausaTextura;
Object opcionPausaObjeto;

Texture opcionTextoTextura;
Object opcionTextoObjeto;