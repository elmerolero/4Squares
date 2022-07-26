#include "derrota.h"
#include "foursquares.h"
#include "preparacion.h"
#include "globales.h"
#include <iostream>
#include <sstream>
#include "utilidades.h"
using namespace std;

const char *finListaOpciones[] = { "Reintentar", "Salir" };
int derrotaOpcion;
int derrotaIndice;

const int DERROTA_OPCIONES = 2;
const int DERROTA_REINICIAR = 0;
const int DERROTA_SALIR = 1;

Derrota::Derrota(){
    derrotaOpcion = DERROTA_REINICIAR;
    anchoActual = 0;
    anchoRelativoActual = 0.f;
    incremento = 4;
    incrementoRelativo = unidadesRelativas( incremento );
    lineaSombreada = BOARD_HEIGHT;

    // Crea el string con la información a mostrar sobre la partida
    if( numeroJugadores == 1 ){
        informacionPartida = comparativo( jugadores[ JUGADOR_UNO ].puntaje, jugadores[ JUGADOR_UNO ].nivel, jugadores[ JUGADOR_UNO ].lineas, jugadores[ JUGADOR_UNO ].comboMaximo, tiempoPartida.obtenerTicks() );
    }
    objInformacion.show( false );

    objCuadroInformativo.show( true );
    objInformacion.show( true );
    //informacionPartida = comparativo( jugador.puntaje, jugador.nivel, jugador.lineas, jugador.comboMaximo, tiempoPartida.obtenerTicks() );
    actualizarTamanioTexto( informacionPartida, objInformacion, fuenteInformacion, 30, 800 );
    objInformacion.escribirDimensionesEspacio( obtenerRectRelativo( objInformacion ) );
    objInformacion.escribirDimensionesTextura( obtenerRectTextura( objInformacion ) );
    objInformacion.escribirEspacioX( ( espacioAncho - objInformacion.leerEspacioAncho() ) / 2.f );
    objInformacion.escribirEspacioY( ( ( espacioAlto - objInformacion.leerEspacioAlto() ) / 2.f ) );
    objInformacion.actualizarDimensionesAbsolutas();

    // Carga la textura de opciones
    if( continuar.loadFileTexture( "../recursos/imagenes/otros/selector-pausa.png" ) ){
        continuar.escribirDimensionesTextura( obtenerRectTextura( continuar ) );
        continuar.escribirDimensionesEspacio( 
            obtenerRectRelativo( continuar ) );
        continuar.escribirTexturaH( continuar.leerTexturaH() / 2 );
        continuar.escribirEspacioAlto( continuar.leerEspacioAlto() / 2 );
        continuar.escribirEspacioY( ( espacioAlto / 5.f ) * 4.f );
        continuar.show( false );
        textoContinuar.show( false );
    }

    ocultarElementos = false;

    // Actualiza la vista
    actualizarViewport();

    tiempoEspera.iniciar();
}

void Derrota::estadoEntrada(){

}

void Derrota::estadoEventos(  SDL_Event &gGameEvent ){
    if( gGameEvent.type == SDL_KEYDOWN ){
		if( gGameEvent.key.keysym.sym == SDLK_RETURN ){
			if( continuar.show() && objCuadroInformativo.show() ){
                // Oculta todo y ejecuta el siguiente menú
                objCuadroInformativo.show( false );
                objInformacion.show( false );
                ocultarElementos = true;
            }
            else if( ocultarElementos ){
                if( derrotaOpcion == DERROTA_REINICIAR ){
                    objSeAcabo.show( false );
                    Juego_EstablecerEstado( new FourSquares(), ESTADO_ESTABLECER );
                    Juego_EstablecerEstado( new Preparacion(), ESTADO_APILAR );
                }
                if( derrotaOpcion == DERROTA_SALIR ){
                    salir = true;
                }
            }
		}
        else if( gGameEvent.key.keysym.sym == SDLK_DOWN ){
            finOpciones.seleccionada++;
            if( finOpciones.seleccionada > 2 ){
                finOpciones.seleccionada = 1;   
            }
            cout << finOpciones.seleccionada << endl;
        }
        else if( gGameEvent.key.keysym.sym == SDLK_UP ){
            finOpciones.seleccionada--;
            if( finOpciones.seleccionada < 1 ){
                finOpciones.seleccionada = 2;   
            }
            cout << finOpciones.seleccionada << endl;
        }
	}
	else if( gGameEvent.type == SDL_WINDOWEVENT ){
		if( gGameEvent.type == SDL_WINDOW_MINIMIZED ){

		}
	}
}

void Derrota::estadoLogica(){
    // Espera dos segundos
    if( tiempoEspera.obtenerTicks() < 1000 ){
        return;
    }

    if( !ocultarElementos ){
        // Muestra las estadísticas
        objInformacion.show( true );
        objCuadroInformativo.show( true );

        // Si el ancho actual es menor que el ancho de la textura
        if( anchoActual < objInformacion.getWidth() ){
            // Actualiza el ancho del rectángulo
            objInformacion.escribirTexturaX( ( objInformacion.getWidth() - anchoActual ) / 2 );
            objInformacion.escribirTexturaW( anchoActual );
            objInformacion.escribirEspacioX( objInformacion.leerEspacioX() + ( objInformacion.leerEspacioAncho() - anchoRelativoActual ) / 2 );
            objInformacion.escribirEspacioAncho( unidadesRelativas( objInformacion.leerTexturaW() ) );

            // Incrementa el contador
            anchoActual += incremento;
            anchoRelativoActual += incrementoRelativo;
            return;
        }

        continuar.show( true );
        textoContinuar.show( true );
    }
}

void Derrota::estadoRenderizado(){
    // Dibuja el fondo
    Juego_FondoNegro();

    if( !ocultarElementos ){
        // Tamaño del tablero
        objCuadroInformativo.renderTexture( objCuadroInformativo.leerDimensionesTextura(), objCuadroInformativo.leerDimensionesEspacio() );

        // Tamaño de la información
        objInformacion.renderTexture( objInformacion.leerDimensionesTextura(), objInformacion.leerDimensionesEspacio() );

        // El botón continuar
        continuar.renderTexture( continuar.leerDimensionesTextura(), continuar.leerDimensionesEspacio() );
        continuar.escribirTexturaY( continuar.leerTexturaY() + continuar.leerTexturaH() );
        continuar.renderTexture( continuar.leerDimensionesTextura(), continuar.leerDimensionesEspacio() );
        continuar.escribirTexturaY( 0 );
        textoContinuar.renderTexture( textoContinuar.leerDimensionesTextura(), textoContinuar.leerDimensionesEspacio() );
    }
    else{
        // Dibuja
        Opciones_Dibujar( finOpciones, finListaOpciones, continuar, textoContinuar  );
    }
}

void Derrota::actualizarViewport(){
    incrementoRelativo = unidadesRelativas( incremento );

    // Tablero
    objCuadroInformativo.escribirEspacioX( ( espacioAncho - objCuadroInformativo.leerEspacioAncho() ) / 2  );
    objCuadroInformativo.escribirEspacioY( ( espacioAlto - objCuadroInformativo.leerEspacioAlto() ) / 2  );
    objCuadroInformativo.actualizarDimensionesAbsolutas();

    // Texto
    actualizarTamanioTexto( datosPartida, objInformacion, fuenteInformacion, 30, 800 );
    objInformacion.escribirEspacioX( ( espacioAncho - objInformacion.leerEspacioAncho() ) / 2 );
    objInformacion.escribirEspacioY( ( ( espacioAlto - objInformacion.leerEspacioAlto() ) / 2 ) + .15 );

    // Boton continuar
    continuar.escribirEspacioX( ( espacioAncho - continuar.leerEspacioAncho() ) / 2 );
    continuar.actualizarDimensionesAbsolutas();

    // Texto coninuar
    Fuente_ActualizarTamanio( fuenteTexto );
    Fuente_ActualizarTexto( "Continuar", fuenteTexto, textoContinuar );
    textoContinuar.escribirEspacioX( continuar.leerEspacioX() + 0.1 );
    textoContinuar.escribirEspacioY( continuar.leerEspacioY() + 0.07 );
    textoContinuar.actualizarDimensionesAbsolutas();
}

void Derrota::escribirNombre( std::string nombre ){
    this -> nombre = nombre;
}

void Derrota::mostrarNombre( void ){
    cout << nombre << endl;
}

int lineaSombreada;
string datosPartida;


// Datos del juego
int incremento;
double incrementoRelativo;
int anchoActual;
double anchoRelativoActual;

// Letrero de se acabó
Temporizador tiempoEspera;

// 
Objeto continuar;
Objeto textoContinuar;

bool ocultarElementos = false;
Opciones finOpciones = { 2, 1 };

void Opciones_Dibujar( Opciones &informacionOpciones, const char *opciones[], Objeto &opcion, Objeto &opcionTexto ){
    // Obtiene la mitad de la pantalla
    double mediaY = espacioAlto / 2;
    SDL_Color color = { 255, 255, 255 };

    // Dibuja el las opciones disponibles
    for( int i = 0; i < informacionOpciones.numero; ++i ){
        double posicionY = ( opcion.leerEspacioAlto() + .2 ) * ( i ) + mediaY;
        opcion.escribirEspacioY( posicionY );
        opcion.renderTexture( opcion.leerDimensionesTextura(), opcion.leerDimensionesEspacio() );

        // ¿Es la opción seleccionada?
        if( informacionOpciones.seleccionada == i + 1 ){
            opcion.escribirTexturaY( opcion.leerTexturaY() + opcion.leerTexturaH() );
            opcion.renderTexture( opcion.leerDimensionesTextura(), opcion.leerDimensionesEspacio() );
            opcion.escribirTexturaY( 0 );
        }

        if( opcionTexto.crearTexturaDesdeTextoSolido( opciones[ i ], color, fuenteTexto.fuente ) ){
            SDL_DRect rrect = { opcion.leerEspacioX() + 0.1, posicionY + 0.07, (float)opcionTexto.getWidth() / Objeto::leerMagnitudUnidad(), (float)opcionTexto.getHeight() / Objeto::leerMagnitudUnidad() };
            opcionTexto.escribirDimensionesTextura( obtenerRectTextura( opcionTexto ) );
            opcionTexto.escribirDimensionesEspacio( rrect );
            opcionTexto.renderTexture( opcionTexto.leerDimensionesTextura(), opcionTexto.leerDimensionesEspacio() );
        }
    }
}