#ifndef GAME_H
#define GAME_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_TTF.h>
#include <string>
#include <vector>
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

// Para el estado del juego
extern EstadoJuego *estadoJuego;
extern std::vector< EstadoJuego * > estadosJuego;

// Opciones del juego
extern bool jSalir;
extern Uint32 jPantallaCompleta;
extern std::string jHabilitarSincV;
extern std::string jCalidadJuego;
extern bool jMostrarTasaCuadros;
extern int  jAnchoPantalla;
extern int 	jAltoPantalla;

// Funciones del juego
bool Juego_Iniciar( std::string nombre );
void Juego_CargarPreferencias( void );
void Juego_EstablecerPreferencias( void );
void Juego_EstablecerPantallaCompleta( bool pantallaCompleta );
void Juego_ActualizarVentana( void );
void Juego_Finalizar( void );

// Establece el estado de juegos dado
void EstadoJuego_EstablecerEstado( std::vector< EstadoJuego * > &estadosJuego, EstadoJuego *estado );

// Elimina todos los estados de juego que existan en una pila
void EstadoJuego_LimpiarEstados( std::vector< EstadoJuego * > &estadosJuego );

// Apila un estado de juego dado
void EstadoJuego_ApilarEstado( std::vector< EstadoJuego * > &estadosJuego, EstadoJuego *estado );
void EstadoJuego_Salir( void );
void EstadoJuego_Salir( std::vector< EstadoJuego * > &estadosJuego );

void EstadoJuego_Entrada( void );
void EstadoJuego_Logica( void );
void EstadoJuego_Renderizar( void );

void EstadoJuego_EsperarParaEvento( bool opcion );

/* Fuentes del juego */
// Fuentes 
extern TTF_Font *fuenteArg;
extern TTF_Font *fuenteAllStar;

/* COLORES */
const SDL_Color COLOR_BLANCO{ 255, 255, 255 };
const SDL_Color COLOR_NEGRO{ 0, 0, 0 };

extern bool esperarEvento;

#endif
