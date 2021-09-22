#include "pausa.h"
#include "foursquares.h"
#include <iostream>
using namespace std;

Pausa::Pausa(): nombre( "pausa" )
{
    if( letreroPausa.loadFileTexture( "../recursos/imagenes/texto/pausa.png" ) ){
        // Establece sus dimensiones en pantalla
        SDL_Rect arect = { 0, 0, letreroPausa.getWidth(), letreroPausa.getHeight() };
        SDL_DRect rrect = { 0, 1, ( (float)arect.w * 6.13 ) / (float)1080, ( (float)arect.h * 6.13 ) / (float)1080 };
        letreroPausa.escribirDimensionesTextura( arect );
        letreroPausa.escribirDimensionesEspacio( rrect );
    }

    if( opcionPausa.loadFileTexture( "../recursos/imagenes/otros/selector-pausa.png" ) ){
        SDL_DRect rrect = { 0, 0, ( (float)opcionPausaRect[ 0 ].w * 6.13 ) / (float)1080, ( (float)opcionPausaRect[ 0 ].h * 6.13 ) / (float)1080 };
        opcionPausa.escribirDimensionesEspacio( rrect );
    }

    // Opcion pausa por defecto es la primera opcion
    opcion = 1;

    actualizarViewport();
}

void Pausa::estadoEntrada(){
    
}

void Pausa::estadoEventos( SDL_Event &gGameEvent ){
    if( joystickConectado && gGameEvent.type == SDL_JOYBUTTONDOWN ){
        if( gGameEvent.jbutton.which == indice ){
            if( gGameEvent.jbutton.button == controles[ indice ].soltarPieza ){
                // Cambia de opcion hacia arriba
                opcion--;
                if( opcion < 1 ){
                    opcion = NUMERO_OPCIONES;
                }
            }
            else if( gGameEvent.cbutton.button == controles[ indice ].bajarPieza ){
                // Cambia de opción hacia abajo
                opcion++;
                if( opcion > NUMERO_OPCIONES ){
                    opcion = 1;
                }
            }
            else if( gGameEvent.jbutton.button == controles[ indice ].pausarJuego ){
                Pausa_SeleccionarOpcion();
            }
        }
    }
    else if( gGameEvent.type == SDL_KEYDOWN ){
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
    Juego_FondoNegro();

    // Dibuja el letrero de pausa
    letreroPausa.renderTexture( letreroPausa.leerDimensionesTextura(), letreroPausa.leerDimensionesEspacio() );

    // Dibuja las opciones
    Pausa_DibujarOpciones();
}

void Pausa::actualizarViewport()
{
    // Actualiza el letrero en pantalla
    letreroPausa.escribirEspacioX( ( espacioAncho - letreroPausa.leerEspacioAncho() ) / 2 );
    letreroPausa.actualizarDimensionesAbsolutas();

    // Actualiza la posicion de las opciones en pantalla 
    opcionPausa.escribirEspacioX( ( espacioAncho - opcionPausa.leerEspacioAncho() ) / 2 );
    opcionPausa.actualizarDimensionesAbsolutas();
}

void Pausa_SeleccionarOpcion( void ){
    if( opcion == OPCION_CONTINUAR ){
        // Continúa con la partida
        FS_ReanudarPartida();
		Juego_EstablecerEstado( nullptr, ESTADO_FINALIZAR );
    }
    else if( opcion == OPCION_REINICIAR ){
        // Inicia una nueva instancia del juego
        FS_ReanudarPartida();
        Juego_EstablecerEstado( new FourSquares(), ESTADO_ESTABLECER );
        Juego_EstablecerEstado( new Preparacion(), ESTADO_APILAR );
    }
    else if( opcion == OPCION_SALIR ){
        // Sale del juego
        salir = true;
    }
}

void Pausa_DibujarOpciones( void ){
    // Obtiene la mitad de la pantalla
    double mediaY = espacioAlto / 2;

    // Dibuja el las opciones disponibles
    for( int i = 0; i < NUMERO_OPCIONES; i++ ){
        double posicionY = ( opcionPausa.leerEspacioAlto() + .2 ) * ( i ) + mediaY;
        opcionPausa.escribirEspacioY( posicionY );
        opcionPausa.renderTexture( &opcionPausaRect[ 0 ], opcionPausa.leerDimensionesEspacio() );

        // ¿Es la opción seleccionada?
        if( opcion == i + 1 ){
            opcionPausa.renderTexture( &opcionPausaRect[ 1 ], opcionPausa.leerDimensionesEspacio() );
        }

        SDL_Color color = { 255, 255, 255 };
        if( opcionTexto.crearTexturaDesdeTextoSolido( opciones[ i ], color, fuenteTexto.fuente ) ){
            SDL_DRect rrect = { opcionPausa.leerEspacioX() + 0.1, posicionY + 0.07, (float)opcionTexto.getWidth() / Objeto::leerMagnitudUnidad(), (float)opcionTexto.getHeight() / Objeto::leerMagnitudUnidad() };
            opcionTexto.escribirDimensionesTextura( 0, 0, opcionTexto.getWidth(), opcionTexto.getHeight() );
            opcionTexto.escribirDimensionesEspacio( rrect );
            opcionTexto.renderTexture( opcionTexto.leerDimensionesTextura(), opcionTexto.leerDimensionesEspacio() );
        }
    }
}

void Pausa::escribirNombre( std::string nombre ){
    this -> nombre = nombre;
}

void Pausa::mostrarNombre( void ){
    cout << nombre << endl;
}

int opcion;
int indice;
const char *opciones[] = { "Continuar", "Reiniciar", "Salir" };

Objeto letreroPausa;
Objeto opcionPausa;

Objeto opcionTexto;