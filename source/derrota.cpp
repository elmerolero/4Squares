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
				<< (t / 60000) % 60 << "'" << setw( 2 ) << (t / 1000) % 60 << "'" << setw( 2 ) << (t % 1000) / 10 << setfill( ' ' ) << setw( 12 ) << 10 << setw( 23 ) << 300 << '\n' << '\n'
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
        textoContinuar.show( false );
    }

    // Actualiza la vista
    actualizarViewport();
}

void Derrota::estadoEntrada(){

}

void Derrota::estadoEventos(){
    if( gGameEvent.type == SDL_KEYDOWN ){
		if( gGameEvent.key.keysym.sym == SDLK_RETURN ){
			jSalir = true;
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
    textoContinuar.show( true );
}

void Derrota::estadoRenderizado(){
    // Dibuja el fondo
    SDL_SetRenderDrawColor( gPtrRenderer, 0x00, 0x00, 0x00, 0x88 );
    SDL_RenderFillRect( gPtrRenderer, &fondoDerrota );

    // Letrero "Se acabó"
    seAcaboTextura.renderTexture( seAcaboObjeto.getSrcRect(), seAcaboObjeto.getDestRect() );

    // Tamaño del tablero
    tableroEstadistico.renderTexture( tableroEstadisticoObjeto.getSrcRect(), tableroEstadisticoObjeto.getDestRect() );

    // Tamaño de la información
    estadisticoTextura.renderTexture( estadisticoObjeto.getSrcRect(), estadisticoObjeto.getDestRect() );

    // El botón continuar
    continuarTextura.renderTexture( continuarObjeto.getSrcRect(), continuarObjeto.getDestRect() );
    continuarObjeto.establecerTexturaY( continuarObjeto.obtenerTexturaY() + continuarObjeto.obtenerTexturaH() );
    continuarTextura.renderTexture( continuarObjeto.getSrcRect(), continuarObjeto.getDestRect() );
    continuarObjeto.establecerTexturaY( 0 );
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
Texture textoContinuar;
