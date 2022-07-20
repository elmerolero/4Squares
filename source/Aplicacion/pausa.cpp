#include "pausa.h"
#include "foursquares.h"
#include <iostream>
using namespace std;

Pausa::Pausa(): nombre( "pausa" )
{
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
    else if( gGameEvent.type == SDL_MOUSEMOTION ){
        // Lee las coordenadas del mouse
        int mouseX = 0;
        int mouseY = 0;

        SDL_GetMouseState( &mouseX, &mouseY );

        // Obtiene la mitad de la pantalla
        double mediaY = espacioAlto / 2;

        // Dibuja el las opciones disponibles
        for( int i = 0; i < NUMERO_OPCIONES; i++ ){
            int posicionX = floor( ( objOpciones.leerEspacioX() + 0.3f ) * Objeto::leerMagnitudUnidad() );
            int posicionY = floor( ( ( objOpciones.leerEspacioAlto() + 0.3f ) * ( i ) + mediaY ) * Objeto::leerMagnitudUnidad() );
            int posicionB = posicionX + floor( objOpciones.leerAbsolutoAncho() );
            int posicionA = posicionY + floor( objOpciones.leerAbsolutoAlto() );
            if( mouseX > posicionX && mouseX < posicionB && mouseY > posicionY && mouseY < posicionA ){
                opcion = i + 1;
            }
        }
    }
    else if( gGameEvent.type == SDL_MOUSEBUTTONDOWN ){
        // Lee las coordenadas del mouse
        int mouseX = 0;
        int mouseY = 0;

        SDL_GetMouseState( &mouseX, &mouseY );

        // Obtiene la mitad de la pantalla
        double mediaY = espacioAlto / 2;

        // Dibuja el las opciones disponibles
        int posicionX = floor( ( objOpciones.leerEspacioX() + 0.3f ) * Objeto::leerMagnitudUnidad() );
        int posicionY = floor( ( ( objOpciones.leerEspacioAlto() + 0.3f ) * ( opcion - 1 ) + mediaY ) * Objeto::leerMagnitudUnidad() );
        int posicionB = posicionX + floor( objOpciones.leerAbsolutoAncho() );
        int posicionA = posicionY + floor( objOpciones.leerAbsolutoAlto() );
        if( mouseX > posicionX && mouseX < posicionB && mouseY > posicionY && mouseY < posicionA ){
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
    objPausa.renderTexture( objPausa.leerDimensionesTextura(), objPausa.leerDimensionesEspacio() );

    // Dibuja las opciones
    Pausa_DibujarOpciones();
}

void Pausa::actualizarViewport()
{
    // Actualiza el letrero en pantalla
    objPausa.escribirEspacioX( ( espacioAncho - objPausa.leerEspacioAncho() ) / 2 );
    objPausa.actualizarDimensionesAbsolutas();

    // Actualiza la posicion de las opciones en pantalla 
    objOpciones.escribirEspacioX( ( espacioAncho - objOpciones.leerEspacioAncho() ) / 2 );
    objOpciones.actualizarDimensionesAbsolutas();
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
        double posicionY = ( objOpciones.leerEspacioAlto() + .3 ) * ( i ) + mediaY;
        objOpciones.escribirEspacioY( posicionY );
        objOpciones.renderTexture( &opcionPausaRect[ 0 ], objOpciones.leerDimensionesEspacio() );

        // ¿Es la opción seleccionada?
        if( opcion == i + 1 ){
            objOpciones.renderTexture( &opcionPausaRect[ 1 ], objOpciones.leerDimensionesEspacio() );
        }

        SDL_Color color = { 255, 255, 255 };
        if( opcionTexto.crearTexturaDesdeTextoSolido( opciones[ i ], color, fuenteTexto.fuente ) ){
            SDL_DRect rrect = { objOpciones.leerEspacioX() + 0.3, posicionY + 0.07, (float)opcionTexto.getWidth() / Objeto::leerMagnitudUnidad(), (float)opcionTexto.getHeight() / Objeto::leerMagnitudUnidad() };
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

Objeto opcionTexto;