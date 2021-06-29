#ifndef TETROBLOCK_H
#define TETROBLOCK_H
#include "gamestate.h"
#include "object.h"
#include "texture.h"
 
// World game state
class Tetroblock : public GameState
{
	public:
		// Constructor and destructor
		Tetroblock();
		~Tetroblock();
		
		void gameStateEvents();
		void gameStateLogic();
		void gameStateRender();
};

// Shape colors (orange, blue, purple, green, red )
const SDL_Color shapeColor[ 7 ] = { { 255, 120,   5 }, {   0,  84, 247 }, {  92,  72, 255 }, { 198,   0,   0 },
									{  63, 158,  82 }, {  60, 143, 255 }, { 255, 216,   0 } };
						
// Shape shadows							
const SDL_Color shadowColor[ 7 ] = { { 185,  80,   5 }, {   0,  51, 150 }, {  60,  10, 185 }, { 128,   0,   0 },
									 { 40,   90,  50 }, {  58, 119, 244 }, { 255, 221,  60 } };
									 

/* CONSTANTES */
#define BOARD_SIZE		5.7
#define BOARD_WIDTH 	10
#define BOARD_HEIGHT	21

// Initial piece's position
#define INITIAL_POS_X	4
#define INITIAL_POS_Y	0

// Shapes
#define SHAPE_SQUARE 6
#define SHAPE_LINE   5

// Max moves allowed
#define MAX_MOVEMENTS	10

// Down speed per level
const Uint32 downSpeed[] = { 1000, 900, 800, 700, 500, 400, 300, 200, 100, 80, 60, 40, 30, 20, 10 };

// Response time
const Uint32 moveResponseTime[] = { 0, 200, 40 };

// Relative shape's positions
const int RELATIVE_POSITIONS[7][4][2] ={ { {  0,  0 }, { -1,  0 }, { -1, -1 }, {  1,  0 } }, // Inverted L
                                         { {  0,  0 }, { -1,  0 }, {  1, -1 }, {  1,  0 } }, // L
                                         { {  0,  0 }, { -1,  0 }, {  0, -1 }, {  1,  0 } }, // T
                                         { {  0,  0 }, { -1,  0 }, {  0, -1 }, {  1, -1 } }, // S
                                         { {  0,  0 }, { -1, -1 }, {  0, -1 }, {  1,  0 } }, // Z 
                                         { {  0,  0 }, {  2,  0 }, {  1,  0 }, { -1,  0 } }, // Line
                                         { {  0,  0 }, {  1,  0 }, {  0, -1 }, {  1, -1 } } }; // Square

// Shapes rects
const SDL_Rect shapeRects[ 7 ] = { {  51,  0, 153, 102 }, { 102,  0, 153, 102 }, {  0,  0, 153, 102 }, { 153,  0, 153, 102 }, 
								   { 306,  0, 153, 102 }, {   0, 51, 204, 102 }, { 510, 0, 102, 102 } };
								   

/* VARIABLES */

// Background
extern Texture tetroTexBackground;
extern Object  tetroBackground;

// Margin
extern Texture tetroTexMargin;
extern Object tetroMargin;

// Board surface
extern Object tetroBoardSurface;

// Block
extern Texture tetroTexBlock;
extern Object  tetroBlock;

// QUeue container
extern Texture tetroTexQueueCont;
extern Object tetroQueueCont;
extern Object tetroQueueRects[ 4 ];

// Shapes queue
extern Texture tetroTexShapes;
extern Object  tetroShapes[ 7 ];

// Game's timer
extern Temporizador gameTimer;

// Input timer
extern int responseLevel;
extern Temporizador inputTime;

// Board
extern int tetroBoard[ BOARD_HEIGHT ][ BOARD_WIDTH ];

// For erasing line animation
extern Temporizador animationTimer;

// Piece
extern int pieceID;
extern int piecePositions[ 4 ][ 2 ];
extern int piecePosX;
extern int piecePosY;

// Queue
extern int 	queueIndex;
extern int queueShapes[ 4 ];

// Piece saved
extern bool allowedChange;
extern int pieceSaved;

// Score
extern int countLines;
extern int countLevel;
extern int countScore;

// Pieces rect
extern SDL_DRect piecesRect;

// Fuentes
extern TTF_Font *fuenteArg;
extern TTF_Font *fuenteAllStar;

// Puntaje
extern Object puntaje;
extern Texture puntajeTextura;

// Funtions
void initObjects();
void updateViewport();
void tetroBlocksInit();
void tetroPieceInit();
void tetroBoardDraw();
void drawPiece( int x, int y );
void tetroShadowDraw( int x, int y );
void tetroShapeDraw( int shape, int position );
void setPieceShape();
void rotatePiece( int );
bool boxIsUsed( int x, int y );
bool pieceColisionDetected( int x, int y );
void savePiece( int x, int y );
void eraseLines();
void FS_ActualizarPuntaje( void );
int queueGetNextShape();
void tetroQueueDraw();
void FS_DibujarFigura( int figura, double x, double y );

#endif
