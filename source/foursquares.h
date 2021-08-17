#ifndef TETROBLOCK_H
#define TETROBLOCK_H
#include "estadojuego.h"
#include "temporizador.h"
#include "objeto.h"
#include "juego.h"
#include "derrota.h"
#include <vector>

// Estado del juego
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

// Pieza
struct Pieza{
	int tipo;					// Indica si es un cuadrado, una línea, una t, etc.
	SDL_Point bloques[ 4 ];		// Posición relativa de cada uno de los bloques que componen la figura
	SDL_Point figura;			// Posición de la figura en el tablero
	SDL_Point sombra;			// Posición de la sombra en el tablero
};

/* CONSTANTES */
#define BOARD_SIZE		5.7

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
const Uint32 downSpeed[] = { 1000, 900, 800, 700, 600, 500, 400, 300, 200, 100, 80, 60, 40, 30, 20 };

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
struct Jugador{
	Pieza pieza;					// Pieza con que tiene el jugador en juego
	int piezaReservada;					// Pieza que reservó el jugador

	int tablero[ BOARD_HEIGHT ][ BOARD_WIDTH ]; // Tablero del jugador

	int lineas;
	int nivel;
	int puntaje;
	int combo;
	int comboMaximo;
	std::vector< int >lineasRealizadas;
	int colaFiguras[ 4 ];

	int nivelRespuesta;				// Indica a qué velocidad mover la pieza hacia los lados
	Temporizador tiempoCambio;		// Tiempo en el que llega el momento de bajar la pieza
	Temporizador tiempoAgregado;	// Tiempo extra para mover la pieza una vez que ha bajado
	Temporizador tiempoBajada;		// Tiempo en el que es posible bajar la pieza
	Temporizador tiempoLaterales;	// Tiempo en el que es posible mover la pieza hacia los lados
	Temporizador tiempoAnimacion;	// Tiempo que anima la eliminación de líneas

	// 
	int columnaActual;				// Columna actual que se elimina

	bool soltarPieza;				// Indica que el jugador soltó la pieza de golpe
	bool permiteReserva;			// Indica que el jugador puede guardar o intercambiar la pieza en juego
	int pasosRealizados;			// Indica el número de movimientos dados despues del sobrepasar el tiempo límite
	bool finalizo;					// Indica que finalizó la partida
};

struct Control{
	SDL_GameController *control;
	SDL_Joystick *joystick;
	SDL_JoystickID id;
	SDL_GameControllerButton soltarPieza;
    SDL_GameControllerButton bajarPieza;
    SDL_GameControllerButton moverDerecha;
    SDL_GameControllerButton moverIzquierda;
    SDL_GameControllerButton rotarDerecha;
    SDL_GameControllerButton rotarIzquierda;
    SDL_GameControllerButton reservarPieza;
    SDL_GameControllerButton lanzarAtaque;
	SDL_GameControllerButton pausarJuego;
};

struct Teclado{
	SDL_Keycode soltarPieza;
    SDL_Keycode bajarPieza;
    SDL_Keycode moverDerecha;
    SDL_Keycode moverIzquierda;
    SDL_Keycode rotarDerecha;
    SDL_Keycode rotarIzquierda;
    SDL_Keycode reservarPieza;
    SDL_Keycode lanzarAtaque;
	SDL_Keycode pausarJuego;
};

// Permite un número máximo de cuatro jugadores
extern int numeroJugadores;
extern Jugador jugadores[ NUMERO_JUGADORES ];
extern Control controles[ NUMERO_JUGADORES ];

// Variables del jugador
extern int contadorCombo;				 // Combo realizado por el jugador
extern int comboMaximo;					 // Combo máximo
extern int contadorLineas;				 // Número de líneas
extern int contadorNivel;				 // Nivel actual
extern int contadorPuntaje;				 // Puntaje
extern std::vector< int > lineasJugador; // Lineas realizadas por el jugador
extern int colaFiguras[ 4 ];			 // Cola de figuras
extern int columna;

// Game's timer
extern Temporizador tiempoPartida;		 // Tiempo transcurrido en el juego
extern int nivelRespuestaLaterales;
extern Temporizador tiempoAdicional;
extern Temporizador indicadorTiempo;
extern Temporizador tiempoEntradaBajada;
extern Temporizador tiempoEntradaLaterales;
extern Temporizador tiempoAnimacion;

// Tablero
extern int tablero[ BOARD_HEIGHT ][ BOARD_WIDTH ];

// Pieza
extern Pieza piezaJugador;

// Piece saved
extern bool permitirCambio;
extern int pieceSaved;
extern bool arribaPresionado;
extern int pasosRealizados;

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
void Tablero_Acomodar( int tablero[ BOARD_HEIGHT ][ BOARD_WIDTH ], std::vector< int > &lineas );
void Tablero_Dibujar( int tablero[ BOARD_HEIGHT ][ BOARD_WIDTH ] );
void Tablero_EstablecerColorRenglon( int tablero[ BOARD_HEIGHT ][ BOARD_WIDTH ], int renglon, int color );

void Cola_Inicializar( int colaFiguras[ 4 ] );
int Cola_ObtenerSiguenteFigura( int colaFiguras[ 4 ] );
void Cola_Dibujar( int colaFiguras[ 4 ] );

unsigned int factorial( unsigned int numero );

#endif
