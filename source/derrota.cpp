#include "derrota.h"
#include "foursquares.h"
#include <iostream>
#include <sstream>
#include "utilidades.h"
using namespace std;

Derrota::Derrota(){
    anchoActual = 0;
    anchoRelativoActual = 0.f;
    incremento = 4;
    incrementoRelativo = unidadesRelativas( incremento );
    lineaSombreada = BOARD_HEIGHT;

    // Crea el string con la información a mostrar sobre la partida
    Uint32 t = tiempoPartida.obtenerTicks();
    stringstream informacion;
    informacion << "Puntaje:" << setw( 14 ) << "Nivel:" << setw( 32 ) << "Puntaje máximo:" << '\n'
				<< setfill( '0' ) << setw( 7 ) << contadorPuntaje << setfill( ' ' ) << setw( 10 ) << contadorNivel << setw( 26 ) << "0000000" << '\n' << '\n'
				<< "Tiempo:" << setw( 22 ) << "Combo máximo:" << setw( 20 ) << "Máximo líneas:" << '\n' << setfill( '0' )
				<< (t / 60000) % 60 << "'" << setw( 2 ) << (t / 1000) % 60 << "'" << setw( 2 ) << (t % 1000) / 10 << setfill( ' ' ) << setw( 12 ) << 00 << setw( 23 ) << 000 << '\n' << '\n'
				<< "Líneas:" << setw( 23 ) << "Menor tiempo: " << '\n'
				<< contadorLineas << setw( 23 ) << "00:00:00" << endl;
    
    datosPartida = informacion.str();
    estadisticoTextura.show( false );

    tableroEstadistico.loadFileTexture( "../recursos/img/otros/estadistico.png" );
    tableroEstadisticoObjeto.setRelativeCoords( obtenerRectRelativo( tableroEstadistico ) );
    tableroEstadisticoObjeto.setTextureCoords( obtenerRectTextura( tableroEstadistico ) );
    tableroEstadistico.show( false );

    // Carga el letrero de "Se acabó"
    if( seAcaboTextura.loadFileTexture( "../recursos/img/texto/se-acabo.png" ) ){
        seAcaboObjeto.setTextureCoords( obtenerRectTextura( seAcaboTextura ) );
        seAcaboObjeto.setRelativeCoords( obtenerRectRelativo( seAcaboTextura ) );
        seAcaboObjeto.setRelativeY( 0.5 );
        seAcaboTextura.show( false );
    }

    // Carga la textura de opciones
    if( continuarTextura.loadFileTexture( "../recursos/img/otros/selector-pausa.png" ) ){
        continuarObjeto.setTextureCoords( obtenerRectTextura( continuarTextura ) );
        continuarObjeto.setRelativeCoords( obtenerRectRelativo( continuarTextura ) );
        continuarObjeto.establecerTexturaH( continuarObjeto.obtenerTexturaH() / 2 );
        continuarObjeto.setRelativeH( continuarObjeto.getRelativeH() / 2 );
        continuarObjeto.setRelativeY( ( gameViewport.h / 5.f ) * 4.f );
        continuarTextura.show( false );
        textoContinuarTextura.show( false );
    }

    ocultarElementos = false;

    // Actualiza la vista
    actualizarViewport();
}

void Derrota::estadoEntrada(){

}

void Derrota::estadoEventos(){
    if( gGameEvent.type == SDL_KEYDOWN ){
		if( gGameEvent.key.keysym.sym == SDLK_RETURN ){
			if( continuarTextura.show() && tableroEstadistico.show() ){
                // Oculta todo y ejecuta el siguiente menú
                tableroEstadistico.show( false );
                estadisticoTextura.show( false );
                ocultarElementos = true;
            }
            else if( ocultarElementos ){
                if( finOpciones.seleccionada == 2 ){
                    jSalir = true;
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
    //
    if( lineaSombreada > 0 ){
        Tablero_EstablecerColorRenglon( tetroBoard, --lineaSombreada, DERROTA );
        SDL_Delay( 50 );
        return;
    }

    // Muestra la textura de "Se acabo" y empieza a contar el tiempo
    if( !seAcaboTextura.show() ){
        seAcaboTextura.show( true );
        tiempoEspera.iniciar();
    }

    // Espera dos segundos
    if( tiempoEspera.obtenerTicks() < 1000 ){
        return;
    }

    if( !ocultarElementos ){
        // Muestra las estadísticas
        estadisticoTextura.show( true );
        tableroEstadistico.show( true );

        // Si el ancho actual es menor que el ancho de la textura
        if( anchoActual < estadisticoTextura.getWidth() ){
            // Actualiza el ancho del rectángulo
            estadisticoObjeto.establecerTexturaX( ( estadisticoTextura.getWidth() - anchoActual ) / 2 );
            estadisticoObjeto.establecerTexturaW( anchoActual );
            estadisticoObjeto.setRelativeX( estadisticoObjeto.getRelativeX() + ( estadisticoObjeto.getRelativeW() - anchoRelativoActual ) / 2 );
            estadisticoObjeto.setRelativeW( unidadesRelativas( estadisticoObjeto.obtenerTexturaW() ) );

            // Incrementa el contador
            anchoActual += incremento;
            anchoRelativoActual += incrementoRelativo;
            return;
        }

        continuarTextura.show( true );
        textoContinuarTextura.show( true );
    }
}

void Derrota::estadoRenderizado(){
    // Dibuja el fondo
    SDL_SetRenderDrawColor( gPtrRenderer, 0x00, 0x00, 0x00, 0x88 );
    SDL_RenderFillRect( gPtrRenderer, &fondoDerrota );

    // Letrero "Se acabó"
    seAcaboTextura.renderTexture( seAcaboObjeto.getSrcRect(), seAcaboObjeto.getDestRect() );

    if( !ocultarElementos ){
        // Tamaño del tablero
        tableroEstadistico.renderTexture( tableroEstadisticoObjeto.getSrcRect(), tableroEstadisticoObjeto.getDestRect() );

        // Tamaño de la información
        estadisticoTextura.renderTexture( estadisticoObjeto.getSrcRect(), estadisticoObjeto.getDestRect() );

        // El botón continuar
        continuarTextura.renderTexture( continuarObjeto.getSrcRect(), continuarObjeto.getDestRect() );
        continuarObjeto.establecerTexturaY( continuarObjeto.obtenerTexturaY() + continuarObjeto.obtenerTexturaH() );
        continuarTextura.renderTexture( continuarObjeto.getSrcRect(), continuarObjeto.getDestRect() );
        continuarObjeto.establecerTexturaY( 0 );
        textoContinuarTextura.renderTexture( textoContinuarObjeto.getSrcRect(), textoContinuarObjeto.getDestRect() );
    }
    else{
        // Dibuja
        Opciones_Dibujar( finOpciones, finListaOpciones, continuarTextura, continuarObjeto, textoContinuarTextura, textoContinuarObjeto  );
    }
}

void Derrota::actualizarViewport(){
    // Fondo de pantalla
    fondoDerrota = { 0, 0, jAnchoPantalla, jAltoPantalla };

    // Letrero de se acabó
    seAcaboObjeto.setRelativeX( ( gameViewport.w - seAcaboObjeto.getRelativeW() ) / 2  );
    seAcaboObjeto.actualizarCoordanadasAbsolutas();

    // Tablero
    tableroEstadisticoObjeto.setRelativeX( ( gameViewport.w - tableroEstadisticoObjeto.getRelativeW() ) / 2  );
    tableroEstadisticoObjeto.setRelativeY( ( gameViewport.h - tableroEstadisticoObjeto.getRelativeH() ) / 2  );
    tableroEstadisticoObjeto.actualizarCoordanadasAbsolutas();

    // Texto
    actualizarTamanioTexto( datosPartida, estadisticoTextura, estadisticoObjeto, fuenteInformacion, 30, 800 );
    estadisticoObjeto.setRelativeX( ( gameViewport.w - estadisticoObjeto.getRelativeW() ) / 2 );
    estadisticoObjeto.setRelativeY( ( ( gameViewport.h - estadisticoObjeto.getRelativeH() ) / 2 ) + .15 );

    // Boton continuar
    continuarObjeto.setRelativeX( ( gameViewport.w - continuarObjeto.getRelativeW() ) / 2 );
    continuarObjeto.actualizarCoordanadasAbsolutas();

    // Texto coninuar
    FS_ActualizarTamanioFuente( fuenteArg, "../recursos/fuentes/Aaargh.ttf", 47 );
    FS_ActualizarTexto( "Continuar", textoContinuarTextura, textoContinuarObjeto, fuenteArg, continuarObjeto.getRelativeX() + 0.1, continuarObjeto.getRelativeY() + 0.07 );
}

void actualizarTamanioTexto( string texto, Texture &textura, Object &objeto, TTF_Font *fuente, int tamanioBase, int anchoTextura ){
    // Cierra la fuente anterior
    if( fuente != nullptr ){
        TTF_CloseFont( fuente );
    }

    // Vuelve a abrir otras fuentes
    fuente = TTF_OpenFont( "../recursos/fuentes/Aaargh.ttf", tamanioBase );
    textura.crearTexturaDesdeTextoBlended( texto.c_str(), COLOR_BLANCO, fuente, anchoTextura );
    objeto.setTextureCoords( obtenerRectTextura( textura ) );
    objeto.setRelativeCoords( obtenerRectRelativo( textura ) );
}

int lineaSombreada;
string datosPartida;
Object tableroEstadisticoObjeto;
Texture tableroEstadistico;
TTF_Font *fuenteInformacion;
SDL_Rect fondoDerrota;

// Datos del juego
int incremento;
double incrementoRelativo;
int anchoActual;
double anchoRelativoActual;
Object estadisticoObjeto;
Texture estadisticoTextura;

// Letrero de se acabó
Temporizador tiempoEspera;
Texture seAcaboTextura;
Object seAcaboObjeto;

// 
Object continuarObjeto;
Texture continuarTextura;
Object textoContinuarObjeto;
Texture textoContinuarTextura;

void Opciones_Dibujar( Opciones &informacionOpciones, const char *opciones[], Texture &opcionTextura, Object &opcionObjeto, Texture &textoTextura, Object &opcionTextoObjeto ){
    // Obtiene la mitad de la pantalla
    double mediaY = gameViewport.h / 2;

    // Dibuja el las opciones disponibles
    for( int i = 0; i < informacionOpciones.numero; ++i ){
        double posicionY = ( opcionObjeto.getRelativeH() + .2 ) * ( i ) + mediaY;
        opcionObjeto.setRelativeY( posicionY );
        opcionTextura.renderTexture( opcionObjeto.getSrcRect(), opcionObjeto.getDestRect() );

        // ¿Es la opción seleccionada?
        if( informacionOpciones.seleccionada == i + 1 ){
            opcionObjeto.establecerTexturaY( opcionObjeto.obtenerTexturaY() + opcionObjeto.obtenerTexturaH() );
            opcionTextura.renderTexture( opcionObjeto.getSrcRect(), opcionObjeto.getDestRect() );
            opcionObjeto.establecerTexturaY( 0 );
        }

        if( textoTextura.crearTexturaDesdeTextoSolido( opciones[ i ], COLOR_BLANCO, fuenteArg ) ){
            SDL_DRect rrect = { opcionObjeto.getRelativeX() + 0.1, posicionY + 0.07, (float)textoTextura.getWidth() / gameUnitSize, (float)textoTextura.getHeight() / gameUnitSize };
            opcionTextoObjeto.setTextureCoords( obtenerRectTextura( textoTextura ) );
            opcionTextoObjeto.setRelativeCoords( rrect );
            textoTextura.renderTexture( opcionTextoObjeto.getSrcRect(), opcionTextoObjeto.getDestRect() );
        }
    }
}