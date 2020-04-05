#include "tetroblock.h"
#include "game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std; 

Tetroblock::Tetroblock()
{	
	srand( time( 0 ) );
	
	gameUpdateViewport();
		
	for( int i = 0; i < 4; i++ ){
		queueShapes[ i ] = rand() % 7;
	}
	queueIndex = 0;
		
	
	// Updates viewport
	updateViewport();

	initObjects();

	// Initializes board
	tetroBlocksInit();
	
	// Initializes the timer
	tempFPS.iniciar();
	gameTimer.iniciar();
	inputTime.iniciar();
	animationTimer.iniciar();
	responseLevel = 0;
	allowedChange = true;
}

Tetroblock::~Tetroblock()
{
	tetroTexBlock.destroyTexture();
	tetroTexShapes.destroyTexture();
	tetroTexMargin.destroyTexture();
	tetroTexBackground.destroyTexture();
}

void Tetroblock::gameStateEvents()
{
	while( SDL_PollEvent( &gGameEvent ) != 0 ){
		if( gGameEvent.type == SDL_QUIT ){
			setState( GAME_STATE_EXIT );
		}
        else if( gGameEvent.type == SDL_KEYDOWN ){
			if( gGameEvent.key.keysym.sym == SDLK_ESCAPE ){
				gDisplayMode = !gDisplayMode;
				setPreferences();
				updateWindowSize();
				gameUpdateViewport();
				updateViewport();
			}
			if( gGameEvent.key.keysym.sym == SDLK_UP){
				while( !pieceColisionDetected( piecePosX, piecePosY ) ){
					piecePosY++;
				}
				piecePosY--;
			}
			if( gGameEvent.key.keysym.sym == SDLK_x){
				rotatePiece( 1 );
				if( pieceColisionDetected( piecePosX, piecePosY ) )
					rotatePiece( -1 );
			}
			else if( gGameEvent.key.keysym.sym == SDLK_z ){
				rotatePiece( -1 );
				if( pieceColisionDetected( piecePosX, piecePosY ) )
					rotatePiece( 1 );
			}
			if( gGameEvent.key.keysym.sym == SDLK_LSHIFT){
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
					allowedChange = false;
				}
			}
		}
		else if( gGameEvent.type == SDL_WINDOWEVENT ){
			if( gGameEvent.window.event == SDL_WINDOWEVENT_RESIZED ){
				updateWindowSize();
				gameUpdateViewport();
				updateViewport();
			}
		}
	}
			
	keyboardState = SDL_GetKeyboardState( NULL );
	if( keyboardState[ SDL_SCANCODE_DOWN ] ){
		piecePosY++;
		if( pieceColisionDetected( piecePosX, piecePosY ) )
			piecePosY--;
	}
	
	// Left and right
	if( keyboardState[ SDL_SCANCODE_LEFT ] ){
		if( inputTime.obtenerTicks() >= moveResponseTime[ responseLevel ] ){
			piecePosX--;
			if( pieceColisionDetected( piecePosX, piecePosY ) ){
				piecePosX++;
			}
				
			if( responseLevel < 2 ){
				responseLevel++;
			}
			
			inputTime.reiniciar();
		}
	}
	else if( keyboardState[ SDL_SCANCODE_RIGHT ] ){
		if( inputTime.obtenerTicks() >= moveResponseTime[ responseLevel ] ){
			piecePosX++;
			if( pieceColisionDetected( piecePosX, piecePosY ) ){
				piecePosX--;
			}
			
			if( responseLevel < 2 ){
				responseLevel++;
			}
			
			inputTime.reiniciar();
		}
	}
	else{
		responseLevel = 0;
	}
}

void Tetroblock::gameStateLogic()
{ 
	if( tempFPS.obtenerTicks() >= 1000 ){
		cout << "FPS: " << fps << endl;
		tempFPS.reiniciar();
		fps = 0;
	}
	
	// Checks for time elapsed
	if( gameTimer.obtenerTicks() >= downSpeed[ countLevel - 1 ] ){
		piecePosY++;
		
		if( pieceColisionDetected( piecePosX, piecePosY ) ){
			if( piecePosY == 1 ){
				//setState( GAME_STATE_EXIT );
			}
			piecePosY--;
			savePiece( piecePosX, piecePosY );
			piecePosY = INITIAL_POS_Y;
			piecePosX = INITIAL_POS_X;
			pieceID = queueGetNextShape();
			setPieceShape();
			allowedChange = true;
			eraseLines();
		}
		gameTimer.reiniciar();
	}
}

void Tetroblock::gameStateRender()
{
	// Clear screen
	SDL_SetRenderDrawColor( gPtrRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
	SDL_RenderClear( gPtrRenderer );
	
	tetroTexBackground.renderTexture( tetroBackground.getSrcRect(), tetroBackground.getDestRect() );
	tetroTexMargin.renderTexture( tetroMargin.getSrcRect(), tetroMargin.getDestRect() );
	tetroBoardDraw();
	
	// Draw the shadow
	tetroShadowDraw( piecePosX, piecePosY );
	
	// Draw the piece
	tetroTexBlock.setColorMod( shapeColor[ pieceID ] );
	drawPiece( piecePosX, piecePosY );
	
	SDL_Color color = { 255, 200, 59 };
	tetroTexQueueCont.setColorMod( color );
	tetroTexQueueCont.renderTexture( tetroQueueCont.getSrcRect(), tetroQueueCont.getDestRect() );
	
	tetroQueueDraw();
	
	// Updates screen
	SDL_RenderPresent( gPtrRenderer );
	fps++;
}

/* FUNCTIONS */

void initObjects()
{
	SDL_DRect auxDRect;
	SDL_Rect auxRect;
	
	// Board surface
	if( !tetroBoardSurface.loadCoordinatesFromFile( "Recursos/Coords/board.crd" ) )
		setState( GAME_STATE_EXIT );
	else{
		tetroBoardSurface.setRelativeX( ( gameViewport.w - tetroBoardSurface.getRelativeW() ) / 2 );
		tetroBoardSurface.updateAbsCoords();
	}
	
	// Margin
	if( tetroTexMargin.loadFileTexture( "Recursos/Margen.png" ) < 0 )
		setState( GAME_STATE_EXIT );
	else{
		tetroMargin.loadCoordinatesFromFile( "Recursos/Coords/margin.crd" );
		tetroMargin.setRelativeX( tetroBoardSurface.getRelativeX() - 0.204545 );
		tetroMargin.updateAbsCoords();
	}
	
	// Background
	if( tetroTexBackground.loadFileTexture( "Recursos/Backgrounds/Stonehenge.png" ) < 0 )
		setState( GAME_STATE_EXIT );
	else{
		auxRect.w = ( ( (float)gDisplayWidth / (float)gDisplayHeight) / ASPECT_RATIO ) * (float)tetroTexBackground.getWidth();
		auxRect.h = ( ASPECT_RATIO / ( (float)gDisplayWidth / (float)gDisplayHeight) ) * (float)tetroTexBackground.getHeight();
		auxRect.x = ( tetroTexBackground.getWidth() - auxRect.w ) / 2;
		auxRect.y = ( tetroTexBackground.getHeight() - auxRect.h ) / 2;
		
		tetroBackground.setRelativeCoords( gameViewport );
		tetroBackground.setTextureCoords( auxRect );
		tetroBackground.updateAbsCoords();
	}
	
	// Blocks
	if( tetroTexBlock.loadFileTexture( "Recursos/Bloques/block.png" ) < 0 ){
		setState( GAME_STATE_EXIT );
	}
	else{
		tetroBlock.loadCoordinatesFromFile( "Recursos/Coords/block.crd" );
		tetroBlock.updateAbsCoords();
	}	
	
	// Queue container
	if( tetroTexQueueCont.loadFileTexture( "Recursos/queue.png" ) < 0 )
		setState( GAME_STATE_EXIT );
	else{
		tetroQueueCont.loadCoordinatesFromFile("Recursos/Coords/queue.crd" );
		tetroQueueCont.setRelativeX( tetroBoardSurface.getRelativeX() + tetroBoardSurface.getRelativeW() + 0.18 );
		tetroQueueCont.updateAbsCoords();
	}
	
	// Queue rects
	for( int i = 0; i < 4; i++ ){
		tetroQueueRects[ i ].setRelativeY( 1 );
		tetroQueueRects[ i ].setRelativeX( tetroQueueCont.getRelativeX() + 0.8 );
		tetroQueueRects[ i ].updateAbsCoords();
	}
	
	
	// Queue shapes
	if( tetroTexShapes.loadFileTexture( "Recursos/Bloques/shapes.png" ) < 0 ){
		setState( GAME_STATE_EXIT );
	}
	else{
		// Shapes rect (black rects for the shapes queue
		SDL_DRect rect = { tetroBoardSurface.getRelativeX(), tetroBoardSurface.getRelativeY(), tetroBoardSurface.getRelativeW(), tetroBoardSurface.getRelativeH() };
		for( int i = 0; i < 7; i++ ){
			switch( i ){
				case SHAPE_LINE:
					auxDRect.w = tetroBlock.getRelativeW() * 4;
					auxDRect.h = tetroBlock.getRelativeW() * 1;
					break;
				case SHAPE_SQUARE:
					auxDRect.w = tetroBlock.getRelativeW() * 2;
					auxDRect.h = tetroBlock.getRelativeW() * 2;
					break;
				default:
					auxDRect.w = tetroBlock.getRelativeW() * 3;
					auxDRect.h = tetroBlock.getRelativeW() * 2;
					break;
			}
			auxDRect.x = ( 0 );
			auxDRect.y = ( 0/*( (tetroBlock.getRelativeW() * 3) - auxDRect.h ) / 2 */ );
			
			tetroShapes[ i ].setRelativeCoords( auxDRect );
			tetroShapes[ i ].setTextureCoords( shapeRects[ i ] );
			tetroShapes[ i ].updateAbsCoords();
		}
	}
	
}

// Updates game's viewport if window is resized
void updateViewport()
{	
	SDL_Rect auxRect;
	
	tetroBoardSurface.setRelativeX( ( gameViewport.w - tetroBoardSurface.getRelativeW() ) / 2  );
	tetroBoardSurface.updateAbsCoords();
	
	tetroBackground.setRelativeCoords( gameViewport );
	auxRect.w = ( ( (float)gDisplayWidth / (float)gDisplayHeight) / ASPECT_RATIO ) * (float)tetroTexBackground.getWidth();
	auxRect.h = ( ASPECT_RATIO / ( (float)gDisplayWidth / (float)gDisplayHeight) ) * (float)tetroTexBackground.getHeight();
	auxRect.x = ( tetroTexBackground.getWidth() - auxRect.w ) / 2;
	auxRect.y = ( tetroTexBackground.getHeight() - auxRect.h ) / 2;
	tetroBackground.setTextureCoords( auxRect );
	tetroBackground.updateAbsCoords();
	
	tetroMargin.setRelativeX( tetroBoardSurface.getRelativeX() - 0.204545 );
	tetroMargin.updateAbsCoords();
	
	tetroBlock.updateAbsCoords();
	
	for(int i = 0; i < 7; i++){
		tetroShapes[ i ].updateAbsCoords();
	}
	
	tetroQueueCont.setRelativeX( tetroBoardSurface.getRelativeX() + tetroBoardSurface.getRelativeW() + 0.18 );
	tetroQueueCont.updateAbsCoords();
	
	for( int i = 0; i < 4; i++ ){
		tetroQueueRects[ i ].setRelativeX( tetroQueueCont.getRelativeX() + 0.8 );
		tetroQueueRects[ i ].updateAbsCoords();
		cout << tetroQueueRects[ i ].getRelativeX() << endl;
	}
	
	//shapeSpaces[ 4 ].x = shapeSpaces[ 3 ].w - 0.3;
	//shapeSpaces[ 4 ].y = 0.5;
	//shapeSpaces[ 4 ].w = 1.3;
	//shapeSpaces[ 4 ].h = 0.82;

	
	piecesRect.x = 0.19;
	piecesRect.y = 0.0;
	piecesRect.w = tetroBlock.getRelativeW() * 5 + 0.25;
	piecesRect.h = 4.5; 
			
	cout << "Pieces rect: " << piecesRect.x * gameUnitSize << " " << piecesRect.y * gameUnitSize << " " << piecesRect.w * gameUnitSize << " " << piecesRect.h * gameUnitSize << endl;
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
	
	// Noc
	countLevel = 1;
	countLines = 0;
	countScore = 0;
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
void drawPiece( int x, int y )
{
	SDL_Rect auxRect = { 0, 0, tetroBlock.getDestRect() -> w, tetroBlock.getDestRect() -> h };
	int tempX = tetroBoardSurface.getDestRect() -> x;
	int tempY = tetroBoardSurface.getDestRect() -> y;
	
	// Draw the shape
	for( int i = 0; i < 4; i++ ){
		auxRect.x = tempX + ( ( x + piecePositions[ i ][ 0 ] ) * auxRect.w);
		auxRect.y = tempY + ( ( y + piecePositions[ i ][ 1 ] ) * auxRect.w);

		tetroTexBlock.renderTexture( tetroBlock.getSrcRect(), &auxRect );
	}
}

// Draws the shadow
void tetroShadowDraw( int x, int y )
{	
	// Sends the piece to the bottom
	while( !pieceColisionDetected( x, y ) ){
		y++;
	}
	y--;
	
	// Set piece's color
	tetroTexBlock.setColorMod( shadowColor[ pieceID ] );
	
	// Draws the shadow
	drawPiece( x, y );
}

void tetroShapeDraw( int shape, int position )
{	
	if( shape < 7 && shape >= 0 ){
		tetroShapes[ shape ].setRelativeX( tetroQueueRects[ position ].getRelativeX() + ( ( tetroQueueRects[ position ].getRelativeW() - tetroShapes[ shape ].getRelativeW() ) / 2 ) ) ;
		tetroShapes[ shape ].setRelativeY( tetroQueueRects[ position ].getRelativeY() + ( ( tetroQueueRects[ position ].getRelativeH() - tetroShapes[ shape ].getRelativeH() ) / 2 ) );
		tetroShapes[ shape ].updateAbsCoords();
		tetroTexShapes.setColorMod( shapeColor[ shape ] );
		tetroTexShapes.renderTexture( tetroShapes[ shape ].getSrcRect(), tetroShapes[ shape ].getDestRect() ); 
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
		if( y + piecePositions[i][1] >= 0 ){
            tetroBoard[ y + piecePositions[i][1] ][ x + piecePositions[i][0] ] = pieceID + 1;
        }
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
			countLines++;
			if( countLines % 10 == 0 )
				if( countLevel < 14 )
					countLevel++;
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
	countScore += linesRealized * countLevel;
	
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
			gGameState -> gameStateRender();
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
		tetroShapeDraw( queueShapes[ queueIndex ], i );
		queueIndex++;
		if( queueIndex >= 4 ){
			queueIndex = 0;
		}
	}
}

/* VARIABLES */

// Background
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

// Queue container
Texture tetroTexQueueCont;
Object tetroQueueCont;
Object tetroQueueRects[ 4 ];

// Shapes attributes
Texture tetroTexShapes;
Object  tetroShapes[ 7 ];

// Game's timer
Temporizador gameTimer;

// Input timer
int responseLevel;
Temporizador inputTime;

//
Temporizador animationTimer;

// Game's variables
int tetroBoard[ BOARD_HEIGHT ][ BOARD_WIDTH ];

// Piece
int pieceID;
int piecePositions[ 4 ][ 2 ];
int piecePosX;
int piecePosY;

// Queue
int queueIndex;
int queueShapes[ 4 ];

//
bool allowedChange;
int pieceSaved;

// Score
int countLines;
int countLevel;
int countScore;

// Pieces rect
SDL_DRect piecesRect;
