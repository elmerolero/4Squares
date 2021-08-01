#include "pausa.h"
#include "foursquares.h"
#include <iostream>
using namespace std;

Pausa::Pausa()
{
    if( letreroPausa.loadFileTexture( "../recursos/imagenes/texto/pausa.png" ) ){
        // Establece sus dimensiones en pantalla
        SDL_Rect arect = { 0, 0, letreroPausa.getWidth(), letreroPausa.getHeight() };
        SDL_DRect rrect = { 0, 1, ( (float)arect.w * 6.13 ) / (float)1080, ( (float)arect.h * 6.13 ) / (float)1080 };
        letreroPausa.escribirDimensionesTextura( arect );
        letreroPausa.escribirDimensionesEspaciales( rrect );
    }

    if( opcionPausa.loadFileTexture( "../recursos/imagenes/otros/selector-pausa.png" ) ){
        SDL_DRect rrect = { 0, 0, ( (float)opcionPausaRect[ 0 ].w * 6.13 ) / (float)1080, ( (float)opcionPausaRect[ 0 ].h * 6.13 ) / (float)1080 };
        opcionPausa.escribirDimensionesEspaciales( rrect );
    }

    // Opcion pausa por defecto es la primera opcion
    opcion = 1;

    actualizarViewport();
}

void Pausa::estadoEntrada(){
    
}

void Pausa::estadoEventos( SDL_Event &gGameEvent ){
    if( gGameEvent.type == SDL_KEYDOWN ){
        if( gGameEvent.key.keysym.sym == SDLK_UP ){
            // Cambia de opcion hacia arriba
            opcion--;
            if( opcion < 1 ){
                opcion = NUMERO_OPCIONES;
            }
        }
        else if( gGameEvent.key.keysym.sym == SDLK_DOWN ){
            // Cambia de opción hacia abajo
            opcion++;
            if( opcion > NUMERO_OPCIONES ){
                opcion = 1;
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
    // Dibuja el fondo negro
    fourSquares.fondoNegro();

    // Dibuja el letrero de pausa
    letreroPausa.renderTexture( letreroPausa.leerDimensionesTextura(), letreroPausa.leerDimensionesEspaciales() );

    // Dibuja las opciones
    Pausa_DibujarOpciones();
}

void Pausa::actualizarViewport()
{
    // Actualiza el letrero en pantalla
    letreroPausa.escribirEspacialX( ( fourSquares.leerEspacioAncho() - letreroPausa.leerEspacialAncho() ) / 2 );
    letreroPausa.actualizarDimensionesAbsolutas();

    // Actualiza la posicion de las opciones en pantalla 
    opcionPausa.escribirEspacialX( ( fourSquares.leerEspacioAncho() - opcionPausa.leerEspacialAncho() ) / 2 );
    opcionPausa.actualizarDimensionesAbsolutas();
}

void Pausa_SeleccionarOpcion( void ){
    if( opcion == OPCION_CONTINUAR ){
        // Continúa con la partida
        FS_ReanudarPartida();
		fourSquares.finalizarEstado();
    }
    else if( opcion == OPCION_REINICIAR ){
        // Inicia una nueva instancia del juego
        FS_ReanudarPartida();
        fourSquares.establecerEstado( new FourSquares() );
        fourSquares.apilarEstado( new Preparacion() );
    }
    else if( opcion == OPCION_SALIR ){
        // Sale del juego
        fourSquares.salir( true );
    }
}

void Pausa_DibujarOpciones( void ){
    // Obtiene la mitad de la pantalla
    double mediaY = fourSquares.leerEspacioAlto() / 2;

    // Dibuja el las opciones disponibles
    for( int i = 0; i < NUMERO_OPCIONES; i++ ){
        double posicionY = ( opcionPausa.leerEspacialAlto() + .2 ) * ( i ) + mediaY;
        opcionPausa.escribirEspacialY( posicionY );
        opcionPausa.renderTexture( &opcionPausaRect[ 0 ], opcionPausa.leerDimensionesEspaciales() );

        // ¿Es la opción seleccionada?
        if( opcion == i + 1 ){
            opcionPausa.renderTexture( &opcionPausaRect[ 1 ], opcionPausa.leerDimensionesEspaciales() );
        }

        SDL_Color color = { 255, 255, 255 };
        if( opcionTexto.crearTexturaDesdeTextoSolido( opciones[ i ], color, fuenteTexto.fuente ) ){
            SDL_DRect rrect = { opcionPausa.leerEspacialX() + 0.1, posicionY + 0.07, (float)opcionTexto.getWidth() / Objeto::leerMagnitudUnidad(), (float)opcionTexto.getHeight() / Objeto::leerMagnitudUnidad() };
            opcionTexto.escribirDimensionesTextura( 0, 0, opcionTexto.getWidth(), opcionTexto.getHeight() );
            opcionTexto.escribirDimensionesEspaciales( rrect );
            opcionTexto.renderTexture( opcionTexto.leerDimensionesTextura(), opcionTexto.leerDimensionesEspaciales() );
        }
    }
}

int opcion;
const char *opciones[] = { "Continuar", "Reiniciar", "Salir" };

Objeto letreroPausa;
Objeto opcionPausa;

Objeto opcionTexto;