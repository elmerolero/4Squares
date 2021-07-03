#ifndef GAME_H
#define GAME_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_TTF.h>
#include <string>
#include "gamestate.h"
#include "texture.h"
#include "temporizador.h"

// For floating point operations
struct SDL_DRect
{
	double x;
	double y;
	double w;
	double h;
};

// Display constants
#define DISPLAY_UNIT	6.13
#define ASPECT_RATIO    1.77

// Para renderizar
extern SDL_Window * gPtrWindow;			// Ventana del juego
extern SDL_Renderer * gPtrRenderer;		// Superficie de renderizado
extern SDL_DisplayMode gDisplayInfo; 	// Información sobre la pantalla
extern SDL_Event gGameEvent;			// Eventos del juego
extern const Uint8 * keyboardState;		// Estado del teclado

// Rectángulo para la vista relativa y el tamaño de la unidad
extern SDL_DRect gameViewport;
extern double gameUnitSize;

// Permite contabilizar la tasa de imágenes por segundo
extern Temporizador tempFPS;
extern int fps;

// Opciones del juego
extern bool jSalir;
extern Uint32 jPantallaCompleta;
extern std::string jHabilitarSincV;
extern std::string jCalidadJuego;
extern bool jMostrarTasaCuadros;
extern int  jAnchoPantalla;
extern int 	jAltoPantalla;
extern EstadoJuego *estadoJuego;


// Funciones del juego
bool Juego_Iniciar( std::string nombre );
void Juego_CargarPreferencias( void );
void Juego_EstablecerPreferencias( void );
void Juego_ActualizarVentana( void );
void Juego_Finalizar( void );


/* GAMESTATES OPTIONS */
// Game states
enum GameStates{
	GAME_STATE_NULL = 0,
	GAME_STATE_EXIT,
	GAME_STATE_GAME
};

#endif
