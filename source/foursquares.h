#ifndef TETROBLOCK_H
#define TETROBLOCK_H
#include "estadojuego.h"
#include "temporizador.h"
#include "objeto.h"
#include "juego.h"
#include "derrota.h"
#include <vector>

// World game state
class FourSquares : public EstadoJuego
{
	public:
		// Constructor and destructor
		FourSquares();
		~FourSquares();
		
		void estadoEntrada();
		void estadoEventos( SDL_Event &gGameEvent );
		void estadoLogica();
		void estadoRenderizado();
		void actualizarViewport();
};

/* CONSTANTES */
#define BOARD_SIZE		5.7
#define BOARD_WIDTH 	10
#define BOARD_HEIGHT	22

// Initial piece's position
#define INITIAL_POS_X	4
#define INITIAL_POS_Y	1

// Shapes
#define FIGURA_LI	    0
#define FIGURA_L	    1
#define FIGURA_T	    2
#define FIGURA_S		3
#define FIGURA_Z		4
#define FIGURA_LINEA  	5
#define FIGURA_CUADRADO 6
#define DERROTA  7

// Numero de movimientos máximos permitidos cuando la pieza llegó a tope
#define PASOS_MAXIMOS	13

// Colores de las figuras (naranja, azul, morado, verde, rojo )
const SDL_Color shapeColor[ 8 ] = { { 255, 120,   5 }, {   0,  84, 247 }, {  92,  72, 255 }, { 198,   0,   0 },
									{  63, 158,  82 }, {  60, 143, 255 }, { 255, 221,  60 }, { 150, 150, 150 } };
						
// Colores de la sombra de cada figura							
const SDL_Color shadowColor[ 7 ] = { { 185,  80,   5 }, {   0,  51, 150 }, {  60,  10, 185 }, { 128,   0,   0 },
									 { 40,   90,  50 }, {  58, 119, 244 }, { 192, 157, 39 } };

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
// Variables del jugador
extern Temporizador tiempoPartida;	// Tiempo transcurrido en el juego
extern int contadorCombo;			// Combo realizado por el jugador
extern int comboMaximo;				// Combo máximo
extern int contadorLineas;			// Número de líneas
extern int contadorNivel;			// Nivel actual
extern int contadorPuntaje;			// Puntaje

// Game's timer
extern Temporizador tiempoAdicional;
extern Temporizador gameTimer;

// 
extern int nivelRespuestaLaterales;
extern Temporizador tiempoEntradaBajada;
extern Temporizador tiempoEntradaLaterales;

// Board
extern int tetroBoard[ BOARD_HEIGHT ][ BOARD_WIDTH ];

// For erasing line animation
extern Temporizador animationTimer;

struct Pieza{
	int tipo;					// Indica si es un cuadrado, una línea, una t, etc.
	SDL_Point bloques[ 4 ];		// Posición relativa de cada uno de los bloques que componen la figura
	SDL_Point figura;			// Posición de la figura en el tablero
	SDL_Point sombra;			// Posición de la sombra en el tablero
};

// Pieza
extern Pieza piezaJugador;

// Cola
extern int colaFiguras[ 4 ];

// Piece saved
extern bool allowedChange;
extern int pieceSaved;
extern bool arribaPresionado;
extern int pasosRealizados;

// Objetos visibles del juego
extern Objeto tetroBackground;	 // Fondo de pantalla
extern Objeto tetroMargin;		 // Margen		
extern Objeto tetroBoardSurface; // Superficie del tablero
extern Objeto tetroBlock;		 // Bloque
extern Objeto gFigura;			 // Cola de figuras
extern Objeto tetroShapes;		 // Figuras
extern Objeto ya;				 // Textura con el texto ya

extern Objeto puntaje; // Puntaje
extern Objeto nivel;   // Nivel
extern Objeto lineas;  // Lineas
extern Objeto tiempo;  // Tiempos

extern std::vector< int > lineasJugador;

void FS_CargarElementos( void );

// Funciones
void FS_ActualizarLineas( int &lineasJugador, std::vector< int > &lineasRealizadas, Objeto &objeto );
void FS_ActualizarNivel( int &nivelJugador, int &lineasJugador, Objeto &objeto );
void FS_ActualizarPuntaje( int &puntajeJugador, std::vector< int > &lineasRealizadas, int nivel, int combo, Objeto &objeto );
void FS_DibujarFigura( int figura, double x, double y );
void FS_DibujarTiempo( Uint32 tiempo, Objeto &objeto, Fuente &fuente, double x, double y );
void FS_PausarPartida( void );
void FS_ReanudarPartida( void );

void Pieza_NuevaPieza( Pieza &pieza, int figura, int tablero[ 21 ][ 10 ] );
void Pieza_ActualizarSombra( Pieza &pieza, int tablero[ 21 ][ 10 ] );
void Pieza_Rotar( Pieza &pieza, int direccion );
void Pieza_Alternar( Pieza &pieza, int tablero[ 21 ][ 10 ], int direccion );
void Pieza_Grabar( Pieza &pieza, int tablero[ BOARD_HEIGHT ][ BOARD_WIDTH ] );
void Pieza_Dibujar( Pieza &pieza, int posicionX, int posicionY, SDL_Color color );

void Tablero_Inicializar( int tablero[ BOARD_HEIGHT ][ BOARD_WIDTH ] );
bool Tablero_PermiteMover( int tablero[ BOARD_HEIGHT ][ BOARD_WIDTH ], Pieza &pieza, int movimientoX, int movimientoY );
bool Tablero_CasillaUtilizada( int tablero[ BOARD_HEIGHT ][ BOARD_WIDTH ], int posicionX, int posicionY );
void Tablero_ObtenerLineas( int tablero[ BOARD_HEIGHT ][ BOARD_WIDTH ], std::vector< int > &lineas );
void Tablero_EliminarLineas( int tablero[ BOARD_HEIGHT ][ BOARD_WIDTH ], std::vector< int > &lineas );
void Tablero_Acomodar( int tablero[ BOARD_HEIGHT ][ BOARD_WIDTH ], std::vector< int > &lineas );
void Tablero_Dibujar( int tablero[ BOARD_HEIGHT ][ BOARD_WIDTH ] );
void Tablero_EstablecerColorRenglon( int tablero[ BOARD_HEIGHT ][ BOARD_WIDTH ], int renglon, int color );

void Cola_Inicializar( int colaFiguras[ 4 ] );
int Cola_ObtenerSiguenteFigura( int colaFiguras[ 4 ] );
void Cola_Dibujar( int colaFiguras[ 4 ] );

#endif
