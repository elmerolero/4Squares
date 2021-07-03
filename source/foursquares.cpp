#include "foursquares.h"
#include "game.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <ctime>
using namespace std; 

FourSquares::FourSquares()
{	
	srand( time( 0 ) );
		
	for( int i = 0; i < 4; i++ ){
		queueShapes[ i ] = rand() % 7;
	}
	queueIndex = 0;

	Juego_ActualizarVentana();

	FS_CargarElementos();

	// Initializes board
	tetroBlocksInit();

	// Updates viewport
	updateViewport();
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
	while( SDL_PollEvent( &gGameEvent ) != 0 ){
		if( gGameEvent.type == SDL_QUIT ){
			jSalir = true;
		}
        else if( gGameEvent.type == SDL_KEYDOWN ){
			if( gGameEvent.key.keysym.sym == SDLK_UP ){
				arribaPresionado = true;
			}
			else if( gGameEvent.key.keysym.sym == SDLK_RETURN ){
				FS_Pausar();
			}
			else if( gGameEvent.key.keysym.sym == SDLK_ESCAPE ){
				jPantallaCompleta = !jPantallaCompleta;
				Juego_EstablecerPreferencias();
				updateViewport();
			}
			else if( gGameEvent.key.keysym.sym == SDLK_x && pieceID != FIGURA_CUADRADO ){
				rotatePiece( 1 );
				if( pieceColisionDetected( piecePosX, piecePosY ) )
					rotatePiece( -1 );
				FS_ActualizarPosicionSombra();
				if( gameTimer.obtenerTicks() >= downSpeed[ contadorNivel - 1 ] ){
					pasosRealizados++;
					tiempoAdicional.reiniciar();
				}
			}
			else if( gGameEvent.key.keysym.sym == SDLK_z && pieceID != FIGURA_CUADRADO ){
				rotatePiece( -1 );
				if( pieceColisionDetected( piecePosX, piecePosY ) )
					rotatePiece( 1 );
				FS_ActualizarPosicionSombra();
				if( gameTimer.obtenerTicks() >= downSpeed[ contadorNivel - 1 ] ){
					pasosRealizados++;
					tiempoAdicional.reiniciar();
				}
			}
			else if( gGameEvent.key.keysym.sym == SDLK_LSHIFT || gGameEvent.key.keysym.sym == SDLK_RSHIFT ){
				if( allowedChange ){
					piecePosX = INITIAL_POS_X;
					piecePosY = INITIAL_POS_Y;
					if( pieceSaved ){
						int tempPiece = pieceID;
						pieceID = pieceSaved - 1;
						pieceSaved = tempPiece + 1;
					}
					else{
						pieceSaved = pieceID + 1;
						pieceID = queueGetNextShape();
					}
					
					setPieceShape();
					FS_ActualizarPosicionSombra();
					allowedChange = false;
					pasosRealizados = 0;
					tiempoAdicional.reiniciar();
					tiempoAdicional.pausar();
				}
			}
		}
		else if( gGameEvent.type == SDL_WINDOWEVENT ){
			if( gGameEvent.window.event == SDL_WINDOWEVENT_RESIZED ){
				Juego_ActualizarVentana();
				updateViewport();
			}
		}
	}
			
	keyboardState = SDL_GetKeyboardState( NULL );

	// Down
	if( keyboardState[ SDL_SCANCODE_DOWN ] ){
		if( inputTime.obtenerTicks() >= moveResponseTime[ responseLevel ] ){
			piecePosY++;
			if( pieceColisionDetected( piecePosX, piecePosY ) ){
				piecePosY--;
			}
			if( responseLevel < 2 ){
				responseLevel++;
			}
			inputTime.reiniciar();
		}
	}
	else if( keyboardState[ SDL_SCANCODE_LEFT ] ){
		if( inputTime.obtenerTicks() >= moveResponseTime[ responseLevel ] ){
			if( !pieceColisionDetected( piecePosX - 1, piecePosY ) ){
				piecePosX--;
			}
			if( responseLevel < 2 ){
				responseLevel++;
			}
			FS_ActualizarPosicionSombra();
			inputTime.reiniciar();
			if( gameTimer.obtenerTicks() >= downSpeed[ contadorNivel - 1 ] ){
				pasosRealizados++;
				tiempoAdicional.reiniciar();
			}
		}
	}
	else if( keyboardState[ SDL_SCANCODE_RIGHT ] ){
		if( inputTime.obtenerTicks() >= moveResponseTime[ responseLevel ] ){
			if( !pieceColisionDetected( piecePosX + 1, piecePosY ) ){
				piecePosX++;
			}
			if( responseLevel < 2 ){
				responseLevel++;
			}

			FS_ActualizarPosicionSombra();
			inputTime.reiniciar();
			if( gameTimer.obtenerTicks() >= downSpeed[ contadorNivel - 1 ] ){
				pasosRealizados++;
				tiempoAdicional.reiniciar();
			}
		}
	}
	else{
		responseLevel = 0;
	}
}

void FourSquares::estadoLogica()
{ 
	if( tempFPS.obtenerTicks() >= 1000 ){
		cout << "FPS: " << fps << endl;
		tempFPS.reiniciar();
		fps = 0;
	}

	// Si se presionó tecla arriba
	if( arribaPresionado ){
		piecePosY++;
		if( pieceColisionDetected( piecePosX, piecePosY ) ){
			piecePosY--;
		}
	}
	
	// Revisa si es momento de bajar la pieza
	if( ( gameTimer.obtenerTicks() >= downSpeed[ contadorNivel - 1 ] ) || arribaPresionado ){
		// Reinicia el tiempo
		gameTimer.reiniciar();

		// ¿No hay colisión en el siguiente paso?
		if( !pieceColisionDetected( piecePosX, piecePosY + 1 ) ){
			// Baja la pieza y sale
			piecePosY++;
			return;
		}

		// Brinda tiempo adicional para acomodar la pieza (útil para niveles rápidos)
		tiempoAdicional.reanudar();
		if( tiempoAdicional.obtenerTicks() < 850 && pasosRealizados < PASOS_MAXIMOS && !arribaPresionado ){
			return;
		}

		if( piecePosY == 1 ){
			jSalir = true;
		}
		savePiece( piecePosX, piecePosY );

		piecePosY = INITIAL_POS_Y;
		piecePosX = INITIAL_POS_X;
		pieceID = queueGetNextShape();
		setPieceShape();
		FS_ActualizarPosicionSombra();

		allowedChange = true;
		eraseLines();
		FS_ActualizarPuntaje();
		FS_ActualizarNivel();
		FS_ActualizarLineas();
		arribaPresionado = false;
		pasosRealizados = 0;
		tiempoAdicional.reiniciar();
		tiempoAdicional.pausar();
		FS_ActualizarPosicionSombra();
	}
}

void FourSquares::estadoRenderizado()
{
	// Limpia la pantalla
	SDL_SetRenderDrawColor( gPtrRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
	SDL_RenderClear( gPtrRenderer );
	
	// Dibuja el fondo
	tetroTexBackground.renderTexture( tetroBackground.getSrcRect(), tetroBackground.getDestRect() );
	tetroTexMargin.renderTexture( tetroMargin.getSrcRect(), tetroMargin.getDestRect() );
	
	// Dibuja el tablero
	tetroBoardDraw();
	
	// Dibuja la sombra de la pieza
	FS_DibujarPieza( sombraPosX, sombraPosY, shadowColor[ pieceID ] );

	// Dibuja la pieza
	FS_DibujarPieza( piecePosX, piecePosY, shapeColor[ pieceID ] );

	// Dibuja la cola de figuras
	tetroQueueDraw();
	
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

	// Updates screen
	SDL_RenderPresent( gPtrRenderer );
	fps++;
}

/* FUNCTIONS */
// Updates game's viewport if window is resized
void updateViewport()
{	
	SDL_Rect auxRect;
	
	// Board surface
	tetroBoardSurface.setRelativeX( ( gameViewport.w - tetroBoardSurface.getRelativeW() ) / 2  );
	
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

	// Texto renderizado
	FS_ActualizarTamanioFuente( fuenteArg, "../recursos/fuentes/Aaargh.ttf", 47.f );
	FS_ActualizarPuntaje();
	FS_ActualizarNivel();
	FS_ActualizarLineas();
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

	FS_ActualizarTamanioFuente( fuenteArg, "../recursos/fuentes/Aaargh.ttf", 47.f );
	FS_ActualizarPuntaje();
	FS_ActualizarNivel();
	FS_ActualizarLineas();
}

// Initializes the game
void tetroBlocksInit()
{
	// Initializes the board in 0s
	for( int rows = 0; rows < BOARD_HEIGHT; rows++ ){
		for( int cols = 0; cols < BOARD_WIDTH; cols++ ){
			tetroBoard[ rows ][ cols ] = 0;
		}
	}
	
	// The piece's position
	piecePosX = INITIAL_POS_X;
	piecePosY = INITIAL_POS_Y;
	pieceID = rand() % 7;
	setPieceShape();
	FS_ActualizarPosicionSombra();
	
	// Noc
	contadorNivel = 10;
	contadorLineas = 0;
	contadorPuntaje = 0;

	// Initializes the timer
	tempFPS.iniciar();
	tiempoPartida.iniciar();
	gameTimer.iniciar();
	inputTime.iniciar();
	animationTimer.iniciar();
	responseLevel = 0;
	allowedChange = true;
}

// Draws the board
void tetroBoardDraw()
{
	SDL_Rect auxRect = { 0, 0, tetroBlock.getDestRect() -> w, tetroBlock.getDestRect() -> h };
	int tempX = tetroBoardSurface.getDestRect() -> x;
	int tempY = tetroBoardSurface.getDestRect() -> y;

	for( int rows = 0; rows < BOARD_HEIGHT; rows++ ){
		for( int cols = 0; cols < BOARD_WIDTH; cols++ ){
			if( tetroBoard[ rows ][ cols ] ){
				auxRect.x = tempX + ( auxRect.w * cols );
				auxRect.y = tempY + ( auxRect.w * rows );
				tetroTexBlock.setColorMod( shapeColor[ tetroBoard[ rows ][ cols ] - 1 ] );
				tetroTexBlock.renderTexture( tetroBlock.getSrcRect(), &auxRect );
			}
		}
	}
}

// Draws the piece
void FS_DibujarPieza( int x, int y, SDL_Color color )
{
	SDL_Rect auxRect = { 0, 0, tetroBlock.getDestRect() -> w, tetroBlock.getDestRect() -> h };
	int tempX = tetroBoardSurface.getDestRect() -> x;
	int tempY = tetroBoardSurface.getDestRect() -> y;
	
	// Establece el color
	tetroTexBlock.setColorMod( color );

	// Dibuja la figura
	for( int i = 0; i < 4; i++ ){
		auxRect.x = tempX + ( ( x + piecePositions[ i ][ 0 ] ) * auxRect.w);
		auxRect.y = tempY + ( ( y + piecePositions[ i ][ 1 ] ) * auxRect.w);

		tetroTexBlock.renderTexture( tetroBlock.getSrcRect(), &auxRect );
	}
}

// Sets the piece positions
void setPieceShape()
{
	for( int i = 0; i < 4; i++ ){
		piecePositions[ i ][ 0 ] = RELATIVE_POSITIONS[ pieceID ][ i ][ 0 ];
		piecePositions[ i ][ 1 ] = RELATIVE_POSITIONS[ pieceID ][ i ][ 1 ];
	}
}

//Rota la figura
void rotatePiece( int direction )
{
    // Saves the new positions
    int rotatedPiece[4][2];

    for(int i = 0; i < 4; i++){
        /* NOTE: Cosine rounded of 90 degrees is 0 and sin of 90 degrees is rounded to 1 */
        //NX = ( VX * cos( 90 ) ) - ( VY * sin( 90 ) )
        rotatedPiece[ i ][ 0 ] = -( piecePositions[ i ][ 1 ] * direction );

        //NX = ( VX * sen( 90 ) ) - ( VY * cos( 90 ) )
        rotatedPiece[ i ][ 1 ] = ( piecePositions[ i ][ 0 ] * direction );
    }

    // Assignes the new positions to the piece
    for( int i = 0; i < 4; i++ ){
        for(int j = 0; j < 2; j++){
            piecePositions[i][j] = rotatedPiece[i][j];
        }
    }
}

// Check for collision
bool pieceColisionDetected( int x, int y  )
{
	for( int i = 0; i < 4; i++ ){
		if( boxIsUsed( x + piecePositions[ i ][ 0 ], y + piecePositions[ i ][ 1 ] ) || 
			x + piecePositions[ i ][ 0 ] > BOARD_WIDTH - 1 || 
			y + piecePositions[ i ][ 1 ] > BOARD_HEIGHT - 1 || 
			x + piecePositions[ i ][ 0 ] < 0 )
			return true;
	}
	
	return false;
}

// Box is being used?
bool boxIsUsed( int x, int y )
{
	if( x < 0 || y < 0 )
		return false;
	else {
		return tetroBoard[ y ][ x ] != 0;
	}
}

// Save the piece
void savePiece( int x, int y )
{
	for( int i = 0; i < 4; i++ ){
        tetroBoard[ y + piecePositions[i][1] ][ x + piecePositions[i][0] ] = pieceID + 1;
	}
}

// Delete lines
void eraseLines()
{
	int linesRealized = 0;
	int countBlocks = 0;
	int startLine = -1;
	int line;
	bool isALine[ BOARD_HEIGHT ];
	for( int row = 0; row < BOARD_HEIGHT && linesRealized < 4; row++ ){
		for( int col = 0; col < BOARD_WIDTH; col++ ){
			if( tetroBoard[ row ][ col ] )
				countBlocks++;
		}
		if( countBlocks == BOARD_WIDTH ){
			isALine[ row ] = true;
			contadorLineas++;
			if( contadorLineas % 10 == 0 ){
				if( contadorNivel < 14 ){
					contadorNivel++;
				}
			}
			linesRealized++;
			countBlocks = 0;
			startLine = row;
		}
		else{
			isALine[ row ] = false;
			countBlocks = 0;
		}
	}
	
	switch( linesRealized ){
		case 1: linesRealized =   40; break;
		case 2: linesRealized =  100; break;
		case 3: linesRealized =  300; break;
		case 4: linesRealized = 1200; break;
	}
	contadorPuntaje += linesRealized * contadorNivel;

	if( startLine >= 0 ){
		for( int col = 0; col < BOARD_WIDTH; col++ ){
			line = startLine;
			for( int row = startLine; row >= 0; row-- ){
				if( isALine [ row ] ){
					tetroBoard[ row ][ col ] = 0;
					line--;
				}
			}
			animationTimer.reiniciar();
			while( animationTimer.obtenerTicks() < 20 );
			estadoJuego -> estadoRenderizado();
		}
		for( int col = 0; col < BOARD_WIDTH; col++ ){
			line = startLine;
			for( int row = startLine; row >= 0; row-- ){
				if( !isALine [ row ] ){
					tetroBoard[ line ][ col ] = tetroBoard[ row ][ col ];
					line--;
				}
			}
		}
	}
}

int queueGetNextShape()
{
	int shape = queueShapes[ queueIndex ];
	queueShapes[ queueIndex ] = rand() % 7;
	queueIndex++;
	
	if( queueIndex >= 4 )
		queueIndex = 0;
	
	return shape;
}

void tetroQueueDraw()
{
	for( int i = 0; i < 4; i++ ){
		int figura = queueShapes[ queueIndex ];
		FS_DibujarFigura( figura,  tetroBoardSurface.getRelativeX() + tetroBoardSurface.getRelativeW() + 0.3, 0.68 + ( gFigura.getRelativeH() * i ) + (0.12 * i ) );
		queueIndex++;
		if( queueIndex >= 4 ){
			queueIndex = 0;
		}
	}
}

void FS_DibujarFigura( int figura, double x, double y ){
	gFigura.setRelativeX( x );
	gFigura.setRelativeY( y );
	tetroTexShapes.setColorMod( shapeColor[ figura ] );
	tetroTexShapes.renderTexture( &shapeRects[ figura ], gFigura.getDestRect() ); 
}

void FS_ActualizarTamanioFuente( TTF_Font *fuente, string archivo, double tamanioBase )
{
	// Fuente
	TTF_CloseFont( fuente );
	fuenteArg = TTF_OpenFont( archivo.c_str(), (int)( ( (float)jAltoPantalla / 1080.f ) * tamanioBase ) );
	if( fuente == nullptr ){
		cout << "Error al cargar la fuente. Error: " << TTF_GetError() << endl;
	}
}

void FS_ActualizarPuntaje( void )
{
	FS_ActualizarDatos( contadorPuntaje, puntajeTextura, puntaje, 7, fuenteArg, tetroMargin.getRelativeX() + 0.227, 2.33 );
}

void FS_ActualizarNivel( void )
{
	FS_ActualizarDatos( contadorNivel, nivelTextura, nivel, 2, fuenteArg, tetroMargin.getRelativeX() + 0.75f, 4.4f );
}

void FS_ActualizarLineas( void )
{
	FS_ActualizarDatos( contadorLineas, lineasTextura, lineas, 2, fuenteArg, tetroMargin.getRelativeX() + 0.75f, 5.4f );
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

void FS_ActualizarPosicionSombra( void )
{
	// Establece la posición original de la figura
	sombraPosX = piecePosX;
	sombraPosY = piecePosY;

	// Establece la posición donde estará
	while( !pieceColisionDetected( sombraPosX, sombraPosY ) ){
		sombraPosY++;
	}
	sombraPosY--;
}

void FS_Pausar()
{
	// Pausa el tiempo
	gameTimer.pausar();
	tiempoPartida.pausar();
	tiempoAdicional.pausar();

	bool reanudar = false;
	while( !reanudar ){
		while( SDL_PollEvent( &gGameEvent ) != 0 ){
			if( gGameEvent.type == SDL_QUIT ){
				jSalir = true;
				reanudar = true;
			}
			else if( gGameEvent.type == SDL_KEYDOWN ){
				if( gGameEvent.key.keysym.sym == SDLK_RETURN ){
					reanudar = true;
				}
			}
		}
	}

	// Reanuda el tiempo
	gameTimer.reanudar();
	tiempoPartida.reanudar();
	tiempoAdicional.reanudar();
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

// Tiempo del juego
Temporizador tiempoPartida;

// Game's timer
Temporizador tiempoAdicional;
Temporizador gameTimer;

// Input timer
int responseLevel;
Temporizador inputTime;

//
Temporizador animationTimer;

// Game's variables
int tetroBoard[ BOARD_HEIGHT ][ BOARD_WIDTH ];

// Pieza
int pieceID;
int piecePositions[ 4 ][ 2 ];
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
int contadorLineas;
int contadorNivel;
int contadorPuntaje;

// Fuentes
TTF_Font *fuenteArg;
TTF_Font *fuenteAllStar;

Object puntaje;
Texture puntajeTextura;

Object nivel;
Texture nivelTextura;

Object lineas;
Texture lineasTextura;

Object tiempo;
Texture tiempoTextura;