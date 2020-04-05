#ifndef GAME_H
#define GAME_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "gamestate.h"
#include "temporizador.h"

extern bool quit;

/* DISPLAY OPTIONS */
// Display constants
#define DISPLAY_UNIT	6.13
#define ASPECT_RATIO    1.77
#define MIN_WIDTH		600
#define MIN_HEIGHT		480

// For floating point operations
struct SDL_DRect
{
	double x;
	double y;
	double w;
	double h;
};

// Display attributes
extern int    gDisplayWidth;
extern int 	  gDisplayHeight;

// For rendering
extern SDL_Window * gPtrWindow;
extern SDL_Renderer * gPtrRenderer;
extern SDL_DisplayMode gDisplayInfo; 
extern SDL_Event gGameEvent;

// Variables for viewport and unit size
extern SDL_DRect gameViewport;
extern double gameUnitSize;

// FPS's
extern Temporizador tempFPS;
extern int fps;

// Keyboard state
extern const Uint8 * keyboardState;

// Image Quality
enum ImageQuality{
	QUALITY_LOW = 0,
	QUALITY_NORMAL
};

// Resolution
enum ImageResolution{
	RES_1920_1080 = 0,
	RES_1600_900,
	RES_1366_768,
	RES_1260_720,
	RES_1024_768,
	RES_800_480,
	RES_AUTO
};

// For game options
extern bool gGameChange;
extern ImageQuality gImageQuality;
extern ImageResolution gImageResolution;
extern bool gDisplayMode;
extern bool gVsync;
extern bool showFPS;

// Game functions
bool init( void );				// Initializes the game
void loadPreferences( void );	// Load user preferences
void setPreferences( void );	// Set user preferences
void updateWindowSize( void );	// Update window when resize	
void gameUpdateViewport( void );// Update the game's viewport
void close( void );				// Finishes the game

/* GAMESTATES OPTIONS */
// Game states
enum GameStates{
	GAME_STATE_NULL = 0,
	GAME_STATE_EXIT,
	GAME_STATE_GAME
};

// Game state attributes
extern GameState * gGameState;
extern GameStates gActualState;

// Game state functions
void setState( GameStates estate );
void changeState( void );

#endif
