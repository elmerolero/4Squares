#include "foursquares.h"
#include "gamestate.h"
#include "game.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <vector>
#include <ctime>
using namespace std; 

FourSquares::FourSquares()
{
	// Carga los elementos del juego
	FS_CargarElementos();

	// Actualiza la ventana
	Juego_ActualizarVentana();

	// Inicializa la cola de figuras
	Cola_Inicializar( queueShapes );
	
	// Establece una nueva pieza
	Pieza_NuevaPieza( piezaJugador, rand() % 7, tetroBoard );

	// Inicializa el tablero
	Tablero_Inicializar( tetroBoard );

	// Indicadores del juego
	contadorNivel = 1;
	contadorLineas = 0;
	contadorCombo = 0;
	contadorPuntaje = 0;

	// Initializa los temporizadores
	tiempoPartida.iniciar();
	gameTimer.iniciar();
	tiempoEntradaBajada.iniciar();
	tiempoEntradaLaterales.iniciar();
	nivelRespuestaLaterales = 0;
	allowedChange = true;

	// Actualiza el viewport
	actualizarViewport();
}
 
FourSquares::~FourSquares()
{
	tetroTexBlock.destroyTexture();
	tetroTexShapes.destroyTexture();
	tetroTexMargin.destroyTexture();
	tetroTexBackground.destroyTexture();
}

void FourSquares::estadoEntrada()
{
	if( SDL_PollEvent( &gGameEvent ) != 0 ){
		if( gGameEvent.type == SDL_QUIT ){
			jSalir = true;
		}
        else if( gGameEvent.type == SDL_KEYDOWN && !arribaPresionado ){
			if( gGameEvent.key.keysym.sym == SDLK_UP ){
				arribaPresionado = true;
			}
			else if( gGameEvent.key.keysym.sym == SDLK_RETURN ){
				FS_PausarPartida();
			}
			else if( gGameEvent.key.keysym.sym == SDLK_ESCAPE ){
				Juego_EstablecerPantallaCompleta( !jPantallaCompleta );
			}
			else if( gGameEvent.key.keysym.sym == SDLK_x && piezaJugador.tipo != FIGURA_CUADRADO ){
				Pieza_Alternar( piezaJugador, tetroBoard, 1 );
				if( gameTimer.obtenerTicks() >= downSpeed[ contadorNivel - 1 ] ){
					pasosRealizados++;
					tiempoAdicional.reiniciar();
				}
			}
			else if( gGameEvent.key.keysym.sym == SDLK_z && piezaJugador.tipo != FIGURA_CUADRADO ){
				Pieza_Alternar( piezaJugador, tetroBoard, -1 );
				if( gameTimer.obtenerTicks() >= downSpeed[ contadorNivel - 1 ] ){
					pasosRealizados++;
					tiempoAdicional.reiniciar();
				}
			}
			else if( gGameEvent.key.keysym.sym == SDLK_LSHIFT || gGameEvent.key.keysym.sym == SDLK_RSHIFT ){
				if( allowedChange ){
					int pieza;
					// ¿Hay una pieza guardada?
					if( pieceSaved ){
						// Obtiene la pieza guardada
						pieza = pieceSaved - 1;

						// Guarda la pieza actualmente en juego
						pieceSaved = piezaJugador.tipo + 1;
					}
					else{
						// Guarda la pieza en juego
						pieceSaved = piezaJugador.tipo + 1;

						// Obtiene una nueva pieza de la cola
						pieza = Cola_ObtenerSiguenteFigura( queueShapes );
					}

					// Establece los nuevos datos de la pieza
					Pieza_NuevaPieza( piezaJugador, pieza, tetroBoard );

					// Establece las banderas y reinicia el tiempo
					allowedChange = false;
					pasosRealizados = 0;
					tiempoAdicional.reiniciar();
					tiempoAdicional.pausar();
				}
			}
			else if( gGameEvent.key.keysym.sym == SDLK_f ){
				jMostrarTasaCuadros = !jMostrarTasaCuadros;
			}
		}
		else if( gGameEvent.type == SDL_WINDOWEVENT ){
			if( gGameEvent.window.event == SDL_WINDOWEVENT_RESIZED ){
				Juego_ActualizarVentana();
			}
			if( gGameEvent.type == SDL_WINDOW_MINIMIZED ){

			}
		}
	}

	if( arribaPresionado ){
		return;
	}
			
	keyboardState = SDL_GetKeyboardState( NULL );

	// Down
	if( keyboardState[ SDL_SCANCODE_DOWN ] && tiempoEntradaBajada.obtenerTicks() >= 50 ){
		if( Tablero_PermiteMover( tetroBoard, piezaJugador, piezaJugador.figura.x, piezaJugador.figura.y + 1 ) ){
			piezaJugador.figura.y++;
		}
		
		tiempoEntradaBajada.reiniciar();
	}
	
	if( keyboardState[ SDL_SCANCODE_LEFT ] ){
		if( tiempoEntradaLaterales.obtenerTicks() >= moveResponseTime[ nivelRespuestaLaterales ] ){
			if( Tablero_PermiteMover( tetroBoard, piezaJugador, piezaJugador.figura.x - 1, piezaJugador.figura.y ) ){
				piezaJugador.figura.x--;
			}

			tiempoEntradaLaterales.reiniciar();
			if( gameTimer.obtenerTicks() >= downSpeed[ contadorNivel - 1 ] ){
				pasosRealizados++;
				tiempoAdicional.reiniciar();
			}

			if( nivelRespuestaLaterales < 2 ){
				nivelRespuestaLaterales++;
			}
		}
		Pieza_ActualizarSombra( piezaJugador, tetroBoard );
	}
	else if( keyboardState[ SDL_SCANCODE_RIGHT ] ){
		if( tiempoEntradaLaterales.obtenerTicks() >= moveResponseTime[ nivelRespuestaLaterales ] ){
			if( Tablero_PermiteMover( tetroBoard, piezaJugador, piezaJugador.figura.x + 1, piezaJugador.figura.y ) ){
				piezaJugador.figura.x++;
			}

			tiempoEntradaLaterales.reiniciar();
			if( gameTimer.obtenerTicks() >= downSpeed[ contadorNivel - 1 ] ){
				pasosRealizados++;
				tiempoAdicional.reiniciar();
			}

			if( nivelRespuestaLaterales < 2 ){
				nivelRespuestaLaterales++;
			}
			Pieza_ActualizarSombra( piezaJugador, tetroBoard );
		}
	}
	else{
		nivelRespuestaLaterales = 0;
	}
}

void FourSquares::estadoLogica()
{ 
	// Si se presionó tecla arriba
	if( arribaPresionado ){
		if( Tablero_PermiteMover( tetroBoard, piezaJugador, piezaJugador.figura.x, piezaJugador.figura.y + 1 ) ){
			piezaJugador.figura.y++;
		}
	}
	
	// Revisa si es momento de bajar la pieza
	if( ( gameTimer.obtenerTicks() >= downSpeed[ contadorNivel - 1 ] ) || arribaPresionado ){
		// Reinicia el tiempo
		gameTimer.reiniciar();

		// ¿No hay colisión en el siguiente paso?
		if( Tablero_PermiteMover( tetroBoard, piezaJugador, piezaJugador.figura.x, piezaJugador.figura.y + 1 ) ){
			piezaJugador.figura.y++;
			return;
		}

		// Brinda tiempo adicional para acomodar la pieza (útil para niveles rápidos)
		tiempoAdicional.reanudar();
		if( tiempoAdicional.obtenerTicks() < 750 && pasosRealizados < PASOS_MAXIMOS && !arribaPresionado ){
			return;
		}

		// Guarda la pieza en el tablero dado
		Pieza_Grabar( piezaJugador, tetroBoard );

		if( piezaJugador.figura.y == 1 ){
			jSalir = true;
			FS_Finalizar();
		}

		allowedChange = true;
		Tablero_ObtenerLineas( tetroBoard, lineasJugador );
		Tablero_EliminarLineas( tetroBoard, lineasJugador );
		Tablero_Acomodar( tetroBoard, lineasJugador );
		
		contadorCombo++;
		if( lineasJugador.empty() ){
			contadorCombo = 0;
		}

		FS_ActualizarLineas( contadorLineas, lineasJugador, lineasTextura, lineas );
		FS_ActualizarNivel( contadorNivel, contadorLineas, nivelTextura, nivel );
		FS_ActualizarPuntaje( contadorPuntaje, lineasJugador, contadorCombo, puntajeTextura, puntaje );

		Pieza_NuevaPieza( piezaJugador, Cola_ObtenerSiguenteFigura( queueShapes ), tetroBoard );
		arribaPresionado = false;
		pasosRealizados = 0;
		tiempoAdicional.reiniciar();
		tiempoAdicional.pausar();
	}
}

void FourSquares::estadoRenderizado()
{	
	// Dibuja el fondo
	tetroTexBackground.renderTexture( tetroBackground.getSrcRect(), tetroBackground.getDestRect() );
	tetroTexMargin.renderTexture( tetroMargin.getSrcRect(), tetroMargin.getDestRect() );
	
	// Dibuja el tablero
	Tablero_Dibujar( tetroBoard );
	
	// Dibuja la sombra de la pieza
	Pieza_Dibujar( piezaJugador, piezaJugador.sombra.x, piezaJugador.sombra.y, shadowColor[ piezaJugador.tipo ] );
	
	// Dibuja la pieza
	Pieza_Dibujar( piezaJugador, piezaJugador.figura.x, piezaJugador.figura.y, shapeColor[ piezaJugador.tipo ] );

	// Dibuja la cola de figuras
	Cola_Dibujar( queueShapes );

	// Dibuja la pieza guardada
	FS_DibujarFigura( pieceSaved - 1, ( tetroBoardSurface.getRelativeX() - 1.7f ), 0.7f );

	// Dibuja el puntaje
	puntajeTextura.renderTexture( puntaje.getSrcRect(), puntaje.getDestRect() );

	// Dibuja el nivel
	nivelTextura.renderTexture( nivel.getSrcRect(), nivel.getDestRect() );

	// Dibuja el numero de lineas
	lineasTextura.renderTexture( lineas.getSrcRect(), lineas.getDestRect() );

	// Dibuja el tiempo
	FS_DibujarTiempo( tiempoPartida.obtenerTicks(), tiempoTextura, tiempo, fuenteArg, tetroBoardSurface.getRelativeX() + tetroBoardSurface.getRelativeW() + 0.6, 5.43f );

	// Dibuja el letrero de ¡Ya!
	if( tiempoPartida.obtenerTicks() < 2000 && estadoJuego == this ){
		ya.renderTexture( yaObjeto.getSrcRect(), yaObjeto.getDestRect() );
	}

	FS_EstablecerEstadistico();
}

/* FUNCTIONS */
// Updates game's viewport if window is resized
void FourSquares::actualizarViewport()
{	
	SDL_Rect auxRect;
	
	// Board surface
	tetroBoardSurface.setRelativeX( ( gameViewport.w - tetroBoardSurface.getRelativeW() ) / 2  );
	tetroBoardSurface.setRelativeY( ( ( gameViewport.h - tetroBoardSurface.getRelativeH() ) / 2 ) - tetroBlock.getRelativeW() );
	
	// Background
	tetroBackground.setRelativeCoords( gameViewport );
	auxRect.w = ( ( (float)jAnchoPantalla / (float)jAltoPantalla) / ASPECT_RATIO ) * (float)tetroTexBackground.getWidth();
	auxRect.h = ( ASPECT_RATIO / ( (float)jAnchoPantalla / (float)jAltoPantalla) ) * (float)tetroTexBackground.getHeight();
	auxRect.x = ( tetroTexBackground.getWidth() - auxRect.w ) / 2;
	auxRect.y = ( tetroTexBackground.getHeight() - auxRect.h ) / 2;
	tetroBackground.setTextureCoords( auxRect );
	
	// Margen
	tetroMargin.setRelativeX( ( gameViewport.w - tetroMargin.getRelativeW() ) / 2 );
	tetroMargin.actualizarCoordanadasAbsolutas();

	// Figura
	gFigura.actualizarCoordanadasAbsolutas();

	//
	tetroBlock.actualizarCoordanadasAbsolutas();

	// 
	yaObjeto.setRelativeX( ( gameViewport.w - yaObjeto.getRelativeW() ) / 2 );

	// Texto renderizado
	FS_ActualizarTamanioFuente( fuenteArg, "../recursos/fuentes/Aaargh.ttf", 47.f );
	FS_ActualizarPuntaje( contadorPuntaje, lineasJugador, contadorCombo, puntajeTextura, puntaje );
	FS_ActualizarNivel( contadorNivel, contadorLineas, nivelTextura, nivel );
	FS_ActualizarLineas( contadorLineas, lineasJugador, lineasTextura, lineas );
	FS_ActualizarDatos( fps, fpsTextura, fpsObjeto, 2, fuenteArg, 0.f, 0.f );
}

void FS_CargarElementos( void )
{
	SDL_DRect auxDRect;
	SDL_Rect auxRect;
	
	// Board surface
	if( !tetroBoardSurface.loadCoordinatesFromFile( "../recursos/coord/board.crd" ) ){
		jSalir = true;
	}
	
	// Margin
	if( !tetroTexMargin.loadFileTexture( "../recursos/img/bloques/margen.png" ) )
		jSalir = true;
	else{
		//tetroMargin.loadCoordinatesFromFile( "../recursos/coord/margin.crd" );
		auxDRect = { ( gameViewport.w - tetroMargin.getRelativeW() ) / 2, 0, ( (float)tetroTexMargin.getWidth() * 6.13 ) / 1080, ( (float)tetroTexMargin.getHeight() * 6.13 ) / 1080 };
		auxRect = { 0, 0, tetroTexMargin.getWidth(), tetroTexMargin.getHeight() };
		tetroMargin.setRelativeCoords( auxDRect );
		tetroMargin.setTextureCoords( auxRect );
	}
	
	// Background
	if( !tetroTexBackground.loadFileTexture( "../recursos/img/fondos/space.png" ) )
		jSalir = true;
	else{
		auxRect.w = ( ( (float)jAnchoPantalla / (float)jAltoPantalla) / ASPECT_RATIO ) * (float)tetroTexBackground.getWidth();
		auxRect.h = ( ASPECT_RATIO / ( (float)jAnchoPantalla / (float)jAltoPantalla) ) * (float)tetroTexBackground.getHeight();
		auxRect.x = ( tetroTexBackground.getWidth() - auxRect.w ) / 2;
		auxRect.y = ( tetroTexBackground.getHeight() - auxRect.h ) / 2;
		
		tetroBackground.setRelativeCoords( gameViewport );
		tetroBackground.setTextureCoords( auxRect );
	}
	
	// Blocks
	if( !tetroTexBlock.loadFileTexture( "../recursos/img/bloques/bloque.png" ) ){
		jSalir = true;
	}
	else{
		tetroBlock.loadCoordinatesFromFile( "../recursos/coord/block.crd" );
	}	
	
	// Queue shapes
	if( !tetroTexShapes.loadFileTexture( "../recursos/img/bloques/figuras.png" ) ){
		jSalir = true;
	}
	else{
		// Shapes rect (black rects for the shapes queue	
		SDL_DRect auxDRect;
		auxDRect.w = ( 236.f * 6.13 ) / 1080;
		auxDRect.h = ( 141.f * 6.13 ) / 1080;
		auxDRect.x = ( 0 );
		auxDRect.y = ( 0 );

		gFigura.setRelativeCoords( auxDRect );
	}

	// Textura ya
	if( ya.loadFileTexture( "../recursos/img/texto/ya.png" ) ){
        SDL_Rect trect = { 0, 0, ya.getWidth(), ya.getHeight() };
        SDL_DRect rrect = { 0, 1, ( (float)trect.w * 6.13 ) / 1080, ( (float)trect.h * 6.13 ) / 1080 };
        yaObjeto.setRelativeCoords( rrect );
	    yaObjeto.setTextureCoords( trect );
        yaObjeto.setRelativeX( ( gameViewport.w - yaObjeto.getRelativeW() ) / 2 );
    }

	FS_ActualizarTamanioFuente( fuenteArg, "../recursos/fuentes/Aaargh.ttf", 47.f );
	FS_ActualizarDatos( fps, fpsTextura, fpsObjeto, 2, fuenteArg, 0.f, 0.f );
}

void Pieza_NuevaPieza( Pieza &pieza, int figura, int tablero[ 21 ][ 10 ] ){
	// Establece el tipo de pieza
	pieza.tipo = figura;

	// Establece los bloques correspondientes
	for( int i = 0; i < 4; i++ ){
		pieza.bloques[ i ].x = RELATIVE_POSITIONS[ pieza.tipo ][ i ][ 0 ];
		pieza.bloques[ i ].y = RELATIVE_POSITIONS[ pieza.tipo ][ i ][ 1 ];
	}

	// Establece la posición inicial
	pieza.figura.x = INITIAL_POS_X;
	pieza.figura.y = INITIAL_POS_Y;

	// Actualiza la posicion de la sombra
	Pieza_ActualizarSombra( pieza, tablero );
}

void Pieza_ActualizarSombra( Pieza &pieza, int tablero[ 21 ][ 10 ] ){
	// Establece la posición de la sombra
	pieza.sombra.x = pieza.figura.x;
	pieza.sombra.y = pieza.figura.y;

	// Mientra permita moverse a lo largo del tablero dado
	while( Tablero_PermiteMover( tablero, pieza, pieza.sombra.x, pieza.sombra.y + 1 ) ){
		pieza.sombra.y++;
	}
}

void Pieza_Rotar( Pieza &pieza, int direccion ){
	// Saves the new positions
    int piezaRotada[ 4 ][ 2 ];

    for(int i = 0; i < 4; i++){
        // NX = ( VX * cos( 90 ) ) - ( VY * sin( 90 ) )
        piezaRotada[ i ][ 0 ] = -( pieza.bloques[ i ].y * direccion );

        // NX = ( VX * sen( 90 ) ) - ( VY * cos( 90 ) )
        piezaRotada[ i ][ 1 ] = ( pieza.bloques[ i ].x * direccion );
    }

    // Assignes the new positions to the piece
    for( int i = 0; i < 4; i++ ){
        pieza.bloques[ i ].x = piezaRotada[ i ][ 0 ];
		pieza.bloques[ i ].y = piezaRotada[ i ][ 1 ];
	}
}

void Pieza_Alternar( Pieza &pieza, int tablero[ 21 ][ 10 ], int direccion  ){
	// Rota la figura
	Pieza_Rotar( pieza, direccion );

	if( pieza.tipo != FIGURA_LINEA ){
		if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x, pieza.figura.y ) ){
			Pieza_ActualizarSombra( pieza, tablero );
			return;
		}
		// Puede moverse si se mueve a la derecha
		else if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x + 1, pieza.figura.y ) ){
			pieza.figura.x++;
		}
		// Puede moverse si se mueve hacia la izquierda
		else if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x - 1, pieza.figura.y ) ){
			pieza.figura.x--;
		}
		// Puede moverse si se sube
		else if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x, pieza.figura.y - 1 ) ){
			pieza.figura.y--;
		}
		else{
			Pieza_Rotar( pieza, direccion * -1 );
		}
	}
	else{
		if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x, pieza.figura.y ) ){
			Pieza_ActualizarSombra( pieza, tablero );
			return;
		}
		// Puede moverse si se mueve a la derecha
		else if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x + 1, pieza.figura.y ) ){
			pieza.figura.x++;
		}
		else if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x + 2, pieza.figura.y ) ){
			pieza.figura.x += 2;
		}
		// Puede moverse si se mueve hacia la izquierda
		else if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x - 1, pieza.figura.y ) ){
			pieza.figura.x--;
		}
		else if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x - 2, pieza.figura.y ) ){
			pieza.figura.x -= 2;
		}
		// Puede moverse si se sube
		else if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x, pieza.figura.y - 1 ) ){
			pieza.figura.y--;
		}
		else if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x, pieza.figura.y - 2 ) ){
			pieza.figura.y -= 2;
		}
		// Puede moverse si se baja
		else if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x, pieza.figura.y + 1 ) ){
			pieza.figura.y++;
		}
		else{
			Pieza_Rotar( pieza, direccion * -1 );
		}
	}

	Pieza_ActualizarSombra( pieza, tablero );
}

void Pieza_Grabar( Pieza &pieza, int tablero[ 21 ][ 10 ] ){
	for( int i = 0; i < 4; i++ ){
		int posX = pieza.figura.x + pieza.bloques[ i ].x;
		int posY = pieza.figura.y + pieza.bloques[ i ].y;
        tablero[ posY ][ posX ] = pieza.tipo + 1;
	}
}

// Dibuja la pieza
void Pieza_Dibujar( Pieza &pieza, int posicionX, int posicionY, SDL_Color color )
{
	SDL_Rect auxRect = { 0, 0, tetroBlock.getDestRect() -> w, tetroBlock.getDestRect() -> h };
	int tempX = tetroBoardSurface.getDestRect() -> x;
	int tempY = tetroBoardSurface.getDestRect() -> y;
	
	// Establece el color
	tetroTexBlock.setColorMod( color );

	// Dibuja la figura
	for( int i = 0; i < 4; i++ ){
		auxRect.x = tempX + ( ( posicionX + pieza.bloques[ i ].x ) * auxRect.w );
		auxRect.y = tempY + ( ( posicionY + pieza.bloques[ i ].y ) * auxRect.w );

		tetroTexBlock.renderTexture( tetroBlock.getSrcRect(), &auxRect );
	}
}

// Inicializa el tablero en ceros
void Tablero_Inicializar( int tablero[ 21 ][ 10 ] ){
	for( size_t renglones = 0; renglones < BOARD_HEIGHT; renglones++ ){
		for( int columnas = 0; columnas < BOARD_WIDTH; columnas++ ){
			tetroBoard[ renglones ][ columnas ] = 0;
		}
	}
}

// Indica si dentro del tablero dado se puede mover una pieza dada
bool Tablero_PermiteMover( int tablero[ 21 ][ 10 ], Pieza &pieza, int posicionX, int posicionY )
{
	for( int i = 0; i < 4; i++ ){
		int posX = posicionX + pieza.bloques[ i ].x;
		int posY = posicionY + pieza.bloques[ i ].y;
		if( posX >= BOARD_WIDTH || posY >= BOARD_HEIGHT || posX < 0 || posY < 0 ||
			Tablero_CasillaUtilizada( tablero, posX, posY ) ){
			return false;
		}
	}
	
	return true;
}

// Indica si la casilla indicada ya contiene un bloque
bool Tablero_CasillaUtilizada( int tablero[ 21 ][ 10 ], int posicionX, int posicionY ){
	return tablero[ posicionY ][ posicionX ] != 0;
}

// Obtiene el número de líneas realizadas en un tablero dado
void Tablero_ObtenerLineas( int tablero[ 21 ][ 10 ], vector< int > &lineas ){
	// Contador de casillas llenas por renglon
	int casillasLlenas = 0;

	// Para cada renglón
	for( size_t renglon = 0; renglon < BOARD_HEIGHT; ++renglon ){
		// Revisar que la casilla de cada columna
		for( size_t columna = 0; columna < BOARD_WIDTH; ++columna ){
			// Si contiene un número distinto de cero (está ocupada)
			if( tablero[ renglon ][ columna ] > 0 ){
				// Se incrementa el número de casillas llenas para ese renglón
				casillasLlenas++;
			}
		}
		
		// Si el renglón tiene llena todas las casillas, entonces se agregar al vector de líneas realizadas
		if( casillasLlenas == BOARD_WIDTH ){
			lineas.push_back( renglon );
		}

		// Reinicia la variable de casillas llenas
		casillasLlenas = 0;
	}
}

// Elimina las líneas dadas
void Tablero_EliminarLineas( int tablero[ 21 ][ 10 ], vector< int > &lineas )
{
	for( size_t columnas = 0; columnas < BOARD_WIDTH; ++columnas ){
		for( size_t renglones = 0; renglones < lineas.size(); ++renglones ){
			tablero[ lineas[ renglones ] ][ columnas ] = 0;
		}

		EstadoJuego_Renderizar();
		SDL_Delay( 20 );
	}
}


void Tablero_Acomodar( int tablero[ 21 ][ 10 ], vector< int > &lineas )
{
	for( int linea : lineas ){
		// A partir de esa linea copiamos el contenido de las casillas hacia arriba
		for( size_t i = linea; i > 0; --i ){
			for( size_t j = 0; j < BOARD_WIDTH; ++j ){
				tablero[ i ][ j ] = tablero[ i - 1 ][ j ];
				tablero[ i - 1 ][ j ] = 0;
			}
		}
	}
}

// Dibuja el tablero
void Tablero_Dibujar( int tablero[ 21 ][ 10 ] )
{
	SDL_Rect auxRect = { 0, 0, tetroBlock.getDestRect() -> w, tetroBlock.getDestRect() -> h };
	int tempX = tetroBoardSurface.getDestRect() -> x;
	int tempY = tetroBoardSurface.getDestRect() -> y;

	for( int renglones = 0; renglones < BOARD_HEIGHT; renglones++ ){
		for( int columnas = 0; columnas < BOARD_WIDTH; columnas++ ){
			if( tablero[ renglones ][ columnas ] ){
				auxRect.x = tempX + ( auxRect.w * columnas );
				auxRect.y = tempY + ( auxRect.w * renglones );
				tetroTexBlock.setColorMod( shapeColor[ tetroBoard[ renglones ][ columnas ] - 1 ] );
				tetroTexBlock.renderTexture( tetroBlock.getSrcRect(), &auxRect );
			}
		}
	}
}

// Inicializa la cola de figuras dada
void Cola_Inicializar( int colaFiguras[ 4 ] ){
	for( size_t i = 0; i < 4; ++i ){
		colaFiguras[ i ] = rand() % 7;
	}
}

// Obtiene la figura de una cola dada
int Cola_ObtenerSiguenteFigura( int colaFiguras[ 4 ] ){
	// Lee la figura a devolver
	int figura = colaFiguras[ 0 ];

	// Recorre las piezas
	for( size_t contador = 1; contador < 4; ++contador ){
		colaFiguras[ contador - 1 ] = colaFiguras[ contador ];
	}

	// Define la nueva figura
	colaFiguras[ 3 ] = rand() % 7;

	return figura;
}

// Dibuja la cola de figuras
void Cola_Dibujar( int colaFiguras[ 4 ] ){
	for( int contador = 0; contador < 4; ++contador ){
		int figura = colaFiguras[ contador ];
		FS_DibujarFigura( figura,  tetroBoardSurface.getRelativeX() + tetroBoardSurface.getRelativeW() + 0.3, 0.68 + ( gFigura.getRelativeH() * contador ) + (0.12 * contador ) );
	}
}

void FS_DibujarFigura( int figura, double x, double y ){
	gFigura.setRelativeX( x );
	gFigura.setRelativeY( y );
	tetroTexShapes.setColorMod( shapeColor[ figura ] );
	tetroTexShapes.renderTexture( &shapeRects[ figura ], gFigura.getDestRect() ); 
}

void FS_ActualizarLineas( int &lineasJugador, vector< int > &lineasRealizadas, Texture &textura, Object &objeto ){
	// Suma la cantidad de líneas realizadas
	lineasJugador += lineasRealizadas.size();

	// Actualiza la textura con el nuevo número de líneas
	FS_ActualizarDatos( lineasJugador, textura, objeto, 2, fuenteArg, tetroMargin.getRelativeX() + 0.75f, 5.4f );
}

void FS_ActualizarNivel( int &nivelJugador, int &lineasJugador, Texture &textura, Object &objeto ){
	// Determina el nivel del jugador 
	nivelJugador = ( lineasJugador / 10 ) + 1;
	if( nivelJugador > 15 ){
		nivelJugador = 15;
	}

	FS_ActualizarDatos( nivelJugador, textura, objeto, 2, fuenteArg, tetroMargin.getRelativeX() + 0.75f, 4.4f );
}

void FS_ActualizarPuntaje( int &puntaje, vector< int > &lineas, int &combo, Texture &textura, Object &objeto )
{
	// Obtiene el nuevo puntaje
	int nuevoPuntaje = ( lineas.empty() ? 0 : 50 );

	// Actualiza el puntaje
	while( !lineas.empty() ){
		nuevoPuntaje = nuevoPuntaje * lineas.size();
		lineas.pop_back();
	}
	
	// Multiplica el puntaje de acuerdo al combo
	puntaje += nuevoPuntaje * combo;
	
	// Actualiza la textura del puntaje
	FS_ActualizarDatos( puntaje, textura, objeto, 7, fuenteArg, tetroMargin.getRelativeX() + 0.227, 2.33 );
}

void FS_ActualizarTamanioFuente( TTF_Font *&fuente, string archivo, double tamanioBase )
{
	// Fuente
	TTF_CloseFont( fuente );
	fuente = TTF_OpenFont( archivo.c_str(), (int)( ( (float)jAltoPantalla / 1080.f ) * tamanioBase ) );
	if( fuente == nullptr ){
		cout << "Error al cargar la fuente. Error: " << TTF_GetError() << endl;
	}
}

void FS_ActualizarDatos( int dato, Texture &textura, Object &objeto, int relleno, TTF_Font *fuente, double x, double y )
{
	stringstream datoStr;
	datoStr << setfill( '0' ) << setw( relleno ) << dato;

	// Fuente
	SDL_Color color = { 255, 255, 255 };
	if( !textura.crearTexturaDesdeTexto( datoStr.str().c_str(), color, fuente ) ){
		jSalir = true;
		return;
	}

	SDL_Rect acoords = { 0, 0, textura.getWidth(), textura.getHeight() };
	SDL_DRect rcoords = { x, y, (float)textura.getWidth() / gameUnitSize, (float)textura.getHeight() / gameUnitSize };
	objeto.setRelativeCoords( rcoords );
	objeto.setTextureCoords( acoords );
}

void FS_DibujarTiempo( Uint32 tiempo, Texture &textura, Object &objeto, TTF_Font *fuente, double x, double y )
{
	stringstream tiempoStr;
	tiempoStr << (tiempo / 60000) % 60 << ":" << std::setfill('0') << std::setw(2) << (tiempo / 1000) % 60 << ":" << (tiempo % 1000) / 10;

	// Fuente
	SDL_Color color = { 255, 255, 255 };
	if( !textura.crearTexturaDesdeTexto( tiempoStr.str().c_str(), color, fuente ) ){
		jSalir = true;
		return;
	}

	SDL_Rect acoords = { 0, 0, textura.getWidth(), textura.getHeight() };
	SDL_DRect rcoords = { x, y, (float)textura.getWidth() / gameUnitSize, (float)textura.getHeight() / gameUnitSize };
	objeto.setRelativeCoords( rcoords );
	objeto.setTextureCoords( acoords );
	textura.renderTexture( objeto.getSrcRect(), objeto.getDestRect() );
}

void FS_PausarPartida(){
	// Pausa el tiempo
	gameTimer.pausar();
	tiempoPartida.pausar();
	tiempoAdicional.pausar();
	tiempoEntradaBajada.pausar();
	tiempoEntradaLaterales.pausar();

	// Evita que se muestren algunas cosas
	tetroTexBlock.show( false );

	// Establece el estado de pausa
	EstadoJuego_ApilarEstado( estadosJuego, new Pausa() );
}

void FS_ReanudarPartida( void ){
	// Reanuda el tiempo
	tiempoPartida.reanudar();
	tiempoAdicional.reanudar();
	tiempoEntradaBajada.reanudar();
	tiempoEntradaLaterales.reanudar();
	gameTimer.reanudar();

	// Vuelve a mostrar lo que se había ocultado
	tetroTexBlock.show( true );
}

void FS_Finalizar( void )
{
	// Crea un archivo de error
    ofstream log;
	Uint32 tiempo = tiempoPartida.obtenerTicks();

    // Se abre el archivo
    log.open( "resultados.txt", ios_base::out );
    if( !log ){
        cout << "Ha ocurrido un error." << endl;
     }

    // Indica el tipo de error generado
    log << "********** Resultados **********" << endl;
	log << "Puntaje: " << contadorPuntaje << endl
		<< "Líneas realizadas: " << contadorLineas << endl
		<< "Nivel alcanzado: " << contadorNivel << endl << endl
		<< "Tiempo: " << (tiempo / 60000) % 60 << ":" << std::setfill('0') << std::setw(2) << (tiempo / 1000) % 60 << ":" << (tiempo % 1000) / 10 << endl;

	if( contadorNivel == 15 ){
		log << "Llegaste al nivel máximo. ¡Bien hecho!" << endl;
	}

	log << "Fin del juego, cierra este bloc para salir." << endl;

    // Cierra el archivo
    log.close();

	system( "notepad resultados.txt &" );
}

void FS_EstablecerEstadistico( void )
{
}

/* VARIABLES */

// Fondo
Texture tetroTexBackground;
Object tetroBackground;

// Margin
Texture tetroTexMargin;
Object tetroMargin;

// Board surface
Object tetroBoardSurface;

// Blocks attributes
Texture tetroTexBlock;
Object  tetroBlock;

// Shapes attributes
Texture tetroTexShapes;
Object gFigura;

Object estadisticoObjeto;
Texture estadisticoTextura;

// Tiempo del juego
Temporizador tiempoPartida;

// Game's timer
Temporizador tiempoAdicional;
Temporizador gameTimer;

// Input timer
int nivelRespuestaLaterales;
Temporizador tiempoEntradaBajada;
Temporizador tiempoEntradaLaterales;

//
Temporizador animationTimer;

//
vector< int > lineasJugador;

// Game's variables
int tetroBoard[ BOARD_HEIGHT ][ BOARD_WIDTH ];

// Pieza
Pieza piezaJugador;
int piezaCodigo;
int pieza[ 4 ][ 2 ];
int piecePosX;
int piecePosY;

// Sombra
int sombraPosX;
int sombraPosY;

// Queue
int queueIndex;
int queueShapes[ 4 ];

//
bool allowedChange;
int pieceSaved;
int pasosRealizados = 0;
bool arribaPresionado = false;

// Puntaje
int contadorCombo;
int contadorLineas;
int contadorNivel;
int contadorPuntaje;

Object puntaje;
Texture puntajeTextura;

Object nivel;
Texture nivelTextura;

Object lineas;
Texture lineasTextura;

Object tiempo;
Texture tiempoTextura;

Object fpsObjeto;
Texture fpsTextura;

Texture ya;
Object yaObjeto;