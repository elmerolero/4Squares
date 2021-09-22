#ifndef GLOBALES_H
#define GLOBALES_H
#include "estadojuego.h"
#include "temporizador.h"
#include "objeto.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <json-c/json.h>
#include <unordered_map>
#include <vector>
#include <deque>

// Constantes
#define NUMERO_JUGADORES 4		// Número máximo de jugadores
#define BOARD_WIDTH 	10      // Numero de columnas
#define BOARD_HEIGHT	22      // Número de renglones
#define JUGADOR_UNO      0		// Jugador 1

// Estructuras
// Pieza
struct Pieza{
	int tipo;					// Indica si es un cuadrado, una línea, una t, etc.
	SDL_Point bloques[ 4 ];		// Posición relativa de cada uno de los bloques que componen la figura
	SDL_Point figura;			// Posición de la figura en el tablero
	SDL_Point sombra;			// Posición de la sombra en el tablero
};

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

struct Fuente{
	TTF_Font *fuente;
	std::string archivo;
	double magnitud;
};

// Componentes SDL
extern SDL_Window *gVentana;           // Ventana
extern SDL_Renderer *gRender;          // Contexto de renderizado
extern SDL_DisplayMode gVistaInfo;     // Modo de la pantalla
extern SDL_Event gEvento;              // Evento del juego

// Opciones del juego
extern bool salir;                     // Indica si salir del juego
extern bool pantallaCompleta;          // Pantalla completa
extern std::string vSinc;              // Sincronizacion vertical               
extern bool mostrarFPS;                // Mostrar fotogramas por segundo   
extern bool calidadImagen;             // Calidad de imagen del juego

// Pantalla
extern double unidadMedida;            // Unidad de medida
extern int pantallaX;                  // Posicion X de la pantalla
extern int pantallaY;                  // Posicion Y de la pantalla
extern int pantallaAncho;              // Ancho de la vista
extern int pantallaAlto;               // Alto de la vista
extern double espacioX;                // Posición espacial X de la pantalla
extern double espacioY;                // Posición espacial Y de la pantalla
extern double espacioAncho;            // Ancho espacial de la pantalla
extern double espacioAlto;             // Alto espacial de la pantalla

// Permite un número máximo de cuatro jugadores
extern bool joystickConectado;
extern int numeroJugadores;
extern Jugador jugadores[ NUMERO_JUGADORES ];
extern Teclado teclado;
extern Control controles[ NUMERO_JUGADORES ];

// Estados del juego
extern EstadoJuego *estadoActual;
extern std::deque< Pendiente * > estadosPendientes;
extern std::vector< EstadoJuego * > estadosJuego;

// Mensajes del juego
extern std::unordered_map< std::string, std::string > valoresMensaje;

// FPS
extern Temporizador temporizadorFPS;
extern int contadorFPS;

// Objetos gráficos
extern Fuente fuenteTexto;
extern Objeto fpsTexto;

// Objetos visibles del juego
extern Objeto objFondoInicio; // Fondo de inicio
extern Objeto objFondo;		// Fondo de pantalla
extern Objeto objMargen;	// Margen		
extern Objeto objTablero;	// Superficie del tablero
extern Objeto objBloque;	// Bloque
extern Objeto objFiguras;	// Figuras
extern Objeto objYa;		// Textura con el texto ya
extern Objeto objPuntaje; 	// Puntaje
extern Objeto objNivel;		// Nivel
extern Objeto objLineas;	// Lineas
extern Objeto objTiempo;	// Tiempos
extern Objeto objCuentaRegresiva; // Cuenta regresiva

#endif