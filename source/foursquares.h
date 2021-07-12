#ifndef TETROBLOCK_H
#define TETROBLOCK_H
#include "gamestate.h"
#include "object.h"
#include "texture.h"

// World game state
class FourSquares : public EstadoJuego
{
	public:
		// Constructor and destructor
		FourSquares();
		~FourSquares();
		
		void estadoEntrada();
		void estadoLogica();
		void estadoRenderizado();
};

/* CONSTANTES */
#define BOARD_SIZE		5.7
#define BOARD_WIDTH 	10
#define BOARD_HEIGHT	21

// Initial piece's position
#define INITIAL_POS_X	4
#define INITIAL_POS_Y	0

// Shapes
#define FIGURA_LI	    0
#define FIGURA_L	    1
#define FIGURA_T	    2
#define FIGURA_S		3
#define FIGURA_Z		4
#define FIGURA_LINEA  	5
#define FIGURA_CUADRADO 6

// Numero de movimientos máximos permitidos cuando la pieza llegó a tope
#define PASOS_MAXIMOS	10

// Colores de las figuras (naranja, azul, morado, verde, rojo )
const SDL_Color shapeColor[ 7 ] = { { 255, 120,   5 }, {   0,  84, 247 }, {  92,  72, 255 }, { 198,   0,   0 },
									{  63, 158,  82 }, {  60, 143, 255 }, { 255, 216,   0 } };
						
// Colores de la sombra de cada figura							
const SDL_Color shadowColor[ 7 ] = { { 185,  80,   5 }, {   0,  51, 150 }, {  60,  10, 185 }, { 128,   0,   0 },
									 { 40,   90,  50 }, {  58, 119, 244 }, { 255, 221,  60 } };

// Velocidad de bajada por nivel
const Uint32 downSpeed[] = { 1000, 900, 800, 700, 500, 400, 300, 200, 100, 80, 60, 40, 30, 20, 10 };

// Response time
const Uint32 moveResponseTime[] = { 0, 200, 40 };

// Relative shape's positions
const int RELATIVE_POSITIONS[7][4][2] ={ { {  0,  0 }, { -1,  0 }, { -1, -1 }, {  1,  0 } }, // L invertida
                                         { {  0,  0 }, { -1,  0 }, {  1, -1 }, {  1,  0 } }, // L
                                         { {  0,  0 }, { -1,  0 }, {  0, -1 }, {  1,  0 } }, // T
                                         { {  0,  0 }, { -1,  0 }, {  0, -1 }, {  1, -1 } }, // S
                                         { {  0,  0 }, { -1, -1 }, {  0, -1 }, {  1,  0 } }, // Z 
                                         { {  0,  0 }, {  2,  0 }, {  1,  0 }, { -1,  0 } }, // Línea
                                         { {  0,  0 }, {  1,  0 }, {  0, -1 }, {  1, -1 } } }; // Cuadrado

// Shapes rects
const SDL_Rect shapeRects[ 7 ] = { {  0,  0, 236, 141 }, { 237,  0, 236, 141 }, {  473,  0, 236, 141 }, { 709,  0, 236, 141 }, 
								   { 945,  0, 236, 141 }, { 1181, 0, 236, 141 }, { 1416, 0, 236, 141 } };
								   

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

// Shapes queue
extern Texture tetroTexShapes;
extern Object gFigura;

// Variables del jugador
extern Temporizador tiempoPartida;	// Tiempo transcurrido en el juego
extern int contadorLineas;			// Número de líneas
extern int contadorNivel;			// Nivel actual
extern int contadorPuntaje;			// Puntaje

// Game's timer
extern Temporizador tiempoAdicional;
extern Temporizador gameTimer;

// Input timer
extern int responseLevel;
extern Temporizador inputTime;

// Board
extern int tetroBoard[ BOARD_HEIGHT ][ BOARD_WIDTH ];

// For erasing line animation
extern Temporizador animationTimer;

// Pieza
extern int pieceID;
extern int piecePositions[ 4 ][ 2 ];
extern int piecePosX;
extern int piecePosY;

// Sombra
extern int sombraPosX;
extern int sombraPosY;

// Cola
extern int queueIndex;
extern int queueShapes[ 4 ];

// Piece saved
extern bool allowedChange;
extern int pieceSaved;
extern bool arribaPresionado;
extern int pasosRealizados;

// Fuentes 
extern TTF_Font *fuenteArg;
extern TTF_Font *fuenteAllStar;

// Puntaje
extern Object puntaje;
extern Texture puntajeTextura;

// Nivel
extern Object nivel;
extern Texture nivelTextura;

// Lineas
extern Object lineas;
extern Texture lineasTextura;

// Tiempo
extern Object tiempo;
extern Texture tiempoTextura;

void FS_CargarElementos( void );

// Funtions
void updateViewport();
void tetroBlocksInit();
void tetroPieceInit();
void tetroBoardDraw();
void FS_DibujarPieza( int x, int y, SDL_Color color );
void tetroShapeDraw( int shape, int position );
void setPieceShape();
void rotatePiece( int );
bool boxIsUsed( int x, int y );
bool pieceColisionDetected( int x, int y );
void savePiece( int x, int y );
void eraseLines();
void FS_ActualizarPuntaje( void );
void FS_ActualizarNivel( void );
int queueGetNextShape();
void tetroQueueDraw();
void FS_DibujarFigura( int figura, double x, double y );
void FS_ActualizarTamanioFuente( TTF_Font *fuente, std::string archivo, double tamanioBase );
void FS_ActualizarLineas( void );
void FS_ActualizarPosicionSombra( void );
void FS_AlternarPieza( int direccion );
void FS_Pausar( void );
void FS_DibujarTiempo( Uint32 tiempo, Texture &textura, Object &objeto, TTF_Font *fuente, double x, double y );
void FS_ActualizarDatos( int dato, Texture &textura, Object &objeto, int relleno, TTF_Font *fuente, double x, double y );

#endif