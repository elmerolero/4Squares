#include "derrota.h"
#include "foursquares.h"
#include "preparacion.h"
#include <iostream>
#include <sstream>
#include "utilidades.h"
#include "database.h"
using namespace std;

const char *finListaOpciones[] = { "Reintentar", "Salir" };

Derrota::Derrota(){
    anchoActual = 0;
    anchoRelativoActual = 0.f;
    incremento = 4;
    incrementoRelativo = unidadesRelativas( incremento );
    lineaSombreada = BOARD_HEIGHT;

    // Crea el string con la información a mostrar sobre la partida
    datosPartida = comparativo( contadorPuntaje, contadorNivel, contadorLineas, comboMaximo, tiempoPartida.obtenerTicks() );
    estadistico.show( false );

    tableroEstadistico.loadFileTexture( "../recursos/imagenes/otros/estadistico.png" );
    tableroEstadistico.escribirDimensionesEspacio( obtenerRectRelativo( tableroEstadistico ) );
    tableroEstadistico.escribirDimensionesTextura( obtenerRectTextura( tableroEstadistico ) );
    tableroEstadistico.show( false );

    // Carga el letrero de "Se acabó"
    if( seAcabo.loadFileTexture( "../recursos/imagenes/texto/se-acabo.png" ) ){
        seAcabo.escribirDimensionesTextura( obtenerRectTextura( seAcabo ) );
        seAcabo.escribirDimensionesEspacio( obtenerRectRelativo( seAcabo ) );
        seAcabo.escribirEspacioY( 0.5 );
        seAcabo.show( false );
    }

    // Carga la textura de opciones
    if( continuar.loadFileTexture( "../recursos/imagenes/otros/selector-pausa.png" ) ){
        continuar.escribirDimensionesTextura( obtenerRectTextura( continuar ) );
        continuar.escribirDimensionesEspacio( obtenerRectRelativo( continuar ) );
        continuar.escribirTexturaH( continuar.leerTexturaH() / 2 );
        continuar.escribirEspacioAlto( continuar.leerEspacioAlto() / 2 );
        continuar.escribirEspacioY( ( espacioAlto / 5.f ) * 4.f );
        continuar.show( false );
        textoContinuar.show( false );
    }

    ocultarElementos = false;

    // Actualiza la vista
    actualizarViewport();
}

void Derrota::estadoEntrada(){

}

void Derrota::estadoEventos(  SDL_Event &gGameEvent ){
    if( gGameEvent.type == SDL_KEYDOWN ){
		if( gGameEvent.key.keysym.sym == SDLK_RETURN ){
			if( continuar.show() && tableroEstadistico.show() ){
                // Oculta todo y ejecuta el siguiente menú
                tableroEstadistico.show( false );
                estadistico.show( false );
                ocultarElementos = true;
            }
            else if( ocultarElementos ){
                if( finOpciones.seleccionada == 1 ){
                    Juego_EstablecerEstado( new FourSquares(), ESTADO_ESTABLECER );
                    Juego_EstablecerEstado( new Preparacion(), ESTADO_APILAR );
                }
                if( finOpciones.seleccionada == 2 ){
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
    //
    if( lineaSombreada > 0 ){
        Tablero_EstablecerColorRenglon( tablero, --lineaSombreada, DERROTA );
        SDL_Delay( 50 );
        return;
    }

    // Muestra la textura de "Se acabo" y empieza a contar el tiempo
    if( !seAcabo.show() ){
        seAcabo.show( true );
        tiempoEspera.iniciar();
    }

    // Espera dos segundos
    if( tiempoEspera.obtenerTicks() < 1000 ){
        return;
    }

    if( !ocultarElementos ){
        // Muestra las estadísticas
        estadistico.show( true );
        tableroEstadistico.show( true );

        // Si el ancho actual es menor que el ancho de la textura
        if( anchoActual < estadistico.getWidth() ){
            // Actualiza el ancho del rectángulo
            estadistico.escribirTexturaX( ( estadistico.getWidth() - anchoActual ) / 2 );
            estadistico.escribirTexturaW( anchoActual );
            estadistico.escribirEspacioX( estadistico.leerEspacioX() + ( estadistico.leerEspacioAncho() - anchoRelativoActual ) / 2 );
            estadistico.escribirEspacioAncho( unidadesRelativas( estadistico.leerTexturaW() ) );

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

    // Letrero "Se acabó"
    seAcabo.renderTexture( seAcabo.leerDimensionesTextura(), seAcabo.leerDimensionesEspacio() );

    if( !ocultarElementos ){
        // Tamaño del tablero
        tableroEstadistico.renderTexture( tableroEstadistico.leerDimensionesTextura(), tableroEstadistico.leerDimensionesEspacio() );

        // Tamaño de la información
        estadistico.renderTexture( estadistico.leerDimensionesTextura(), estadistico.leerDimensionesEspacio() );

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
    // Letrero de se acabó
    seAcabo.escribirEspacioX( ( espacioAncho - seAcabo.leerEspacioAncho() ) / 2  );
    seAcabo.actualizarDimensionesAbsolutas();

    // Tablero
    tableroEstadistico.escribirEspacioX( ( espacioAncho - tableroEstadistico.leerEspacioAncho() ) / 2  );
    tableroEstadistico.escribirEspacioY( ( espacioAlto - tableroEstadistico.leerEspacioAlto() ) / 2  );
    tableroEstadistico.actualizarDimensionesAbsolutas();

    // Texto
    actualizarTamanioTexto( datosPartida, estadistico, fuenteInformacion, 30, 800 );
    estadistico.escribirEspacioX( ( espacioAncho - estadistico.leerEspacioAncho() ) / 2 );
    estadistico.escribirEspacioY( ( ( espacioAlto - estadistico.leerEspacioAlto() ) / 2 ) + .15 );

    // Boton continuar
    continuar.escribirEspacioX( ( espacioAncho - continuar.leerEspacioAncho() ) / 2 );
    continuar.actualizarDimensionesAbsolutas();

    // Texto coninuar
    Fuente_ActualizarTamanio( fuenteTexto );
    Fuente_ActualizarTexto( "Continuar", fuenteTexto, textoContinuar );
    textoContinuar.escribirEspacioX( continuar.leerEspacioX() + 0.1 );
    textoContinuar.escribirEspacioY( continuar.leerEspacioY() + 0.07 );
}

void actualizarTamanioTexto( string texto, Objeto &objeto, TTF_Font *fuente, int tamanioBase, int anchoTextura ){
    // Cierra la fuente anterior
    if( fuente != nullptr ){
        TTF_CloseFont( fuente );
    }

    // Vuelve a abrir otras fuentes
    fuente = TTF_OpenFont( "../recursos/fuentes/Aaargh.ttf", tamanioBase );
    SDL_Color color = { 255, 255, 255 };
    objeto.crearTexturaDesdeTextoBlended( texto.c_str(), color, fuente, anchoTextura );
    objeto.escribirDimensionesTextura( obtenerRectTextura( objeto ) );
    objeto.escribirDimensionesEspacio( obtenerRectRelativo( objeto ) );
}

string comparativo( int puntaje, int nivel, int lineas, int combo, Uint32 tiempo )
{
    // Base de datos de donde abrirá el archivo
    Database database;

    // Mejores logros
    int auxiliar = 0;
    int puntajeMaximo = 0;
    int lineasMaximas = 0;
    Uint32 mejorTiempo = 0;
    int lineasLogradas = 0;
    int comboMaximo = 0;

    // Realiza la consulta a la base de datos
    database.open( databaseFile );
    database.query( "select * from records" );
    database.close();

    // ¿Hay resultados?
    if( results.size() > 0 ){
        puntajeMaximo = stoi( ( *results.at( 0 ) )[ "puntaje_maximo" ] );
        lineasMaximas = stoi( ( *results.at( 0 ) )[ "lineas_maximas" ] );
        mejorTiempo = stoi( ( *results.at( 0 ) )[ "mejor_tiempo" ] );
        lineasLogradas = stoi( ( *results.at( 0 ) )[ "lineas_logradas" ] ); 
        comboMaximo = stoi( ( *results.at( 0 ) )[ "combo_maximo" ] );
    }

    // Realiza la comparación
    lineasMaximas = ( lineasMaximas > lineas ? lineasMaximas : lineas );
    comboMaximo = ( comboMaximo > combo ? comboMaximo : combo );

    // Contruye el comparativo
    stringstream informacion;
    if( puntajeMaximo < puntaje ){
        puntajeMaximo = puntaje;
        informacion << ' ' << setw( 36 ) << "¡Nuevo récord!" << endl;
    }

    informacion << "Puntaje:" << setw( 15 ) << "Nivel:" << setw( 29 ) << "Puntaje máximo:" << "\n"
				<< setfill( '0' ) << setw( 7 ) << puntaje << setfill( ' ' ) << setw( 10 ) << nivel << setfill( ' ' ) << setw( 17 ) << " " << setfill( '0' ) << setw( 7 ) << puntajeMaximo << setfill( ' ' ) << "\n\n"
				<< "Tiempo:" << setw( 21 ) << "Mejor combo:" << setw( 21 ) << "Máximo líneas:" << '\n' << setfill( '0' )
				<< (tiempo / 60000) % 60 << "'" << setw( 2 ) << (tiempo / 1000) % 60 << "'" << setw( 2 ) << (tiempo % 1000) / 10 << setfill( ' ' ) << setw( 13 ) << comboMaximo << setw( 23 ) << lineasMaximas << '\n' << '\n'
				<< "Líneas:\n" << setw( 3 ) << setfill( '0' ) << lineas << setfill( ' ' ) << endl;

    database.open( databaseFile );
    if( results.empty() ){
        database.query( "insert into records values( 1, " + to_string( puntajeMaximo ) + ", " + to_string( lineasMaximas ) + ", " + to_string( mejorTiempo ) + ", " + to_string( lineasLogradas ) + "," + to_string( comboMaximo ) + ")" );
    }
    else{
        database.query( "update records set puntaje_maximo = " + to_string( puntajeMaximo ) + ", lineas_maximas = " + to_string( lineasMaximas ) + ", mejor_tiempo = " + to_string( mejorTiempo ) + ", lineas_logradas = " + to_string( lineasLogradas ) + ", combo_maximo = " + to_string( comboMaximo ) + " where codigo = 1" );
    }
    database.close();

    return informacion.str();
}

int lineaSombreada;
string datosPartida;
Objeto tableroEstadistico;
TTF_Font *fuenteInformacion;

// Datos del juego
int incremento;
double incrementoRelativo;
int anchoActual;
double anchoRelativoActual;
Objeto estadistico;

// Letrero de se acabó
Temporizador tiempoEspera;
Objeto seAcabo;

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