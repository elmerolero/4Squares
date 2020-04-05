#include "game.h"
#include "tetroblock.h"
#include <iostream>
using namespace std;

// Display attributes
int gDisplayWidth;
int gDisplayHeight;

// For rendering
SDL_Window * gPtrWindow;
SDL_Renderer * gPtrRenderer;
SDL_DisplayMode gDisplayInfo;
SDL_Event gGameEvent;

// Variables for viewport and unit size
SDL_DRect gameViewport;
double gameUnitSize;

// Temporizador
Temporizador tempFPS;
int fps;

// Keyboard state
const Uint8 * keyboardState;

// Game display options
bool gGameChange;
ImageQuality gImageQuality;
ImageResolution gImageResolution;
bool gDisplayMode;
bool gVsync;
bool showFPS;

// Game state options
GameState * gGameState;
GameStates gActualState;

bool init( void )
{
	bool sucess = true;
	
	// Initializes SDL lib
	cout << "======================= Four Squares =======================";
	cout << "\nInitializing SDL lib. ";
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 || (IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG) != IMG_INIT_PNG ){
		cout << "Error initilizing SDL. Details SDL: " << SDL_GetError() << " IMG: " << IMG_GetError() << endl; 
		sucess = false;
	}
	else{
		// Intializes medisplay Options
		cout << "Intilializing video. " << endl;
		
		// Gets display options and check display was not too small
		if( SDL_GetCurrentDisplayMode( 0, &gDisplayInfo ) != 0 && ( gDisplayInfo.w < MIN_WIDTH || gDisplayInfo.h < MIN_HEIGHT ) ){
			if( gDisplayInfo.w < MIN_WIDTH || gDisplayInfo.h < MIN_HEIGHT )
				cout << "Error, display's resolution is lower than the minimum resolution. :(" << endl;
			else
				cout << "\nError getting current display mode. Error: " << SDL_GetError() << endl;
			sucess = false;
		}
		else{
			cout << "\nVideo initialized with current display properties:\n"
				  << "Width: " << gDisplayInfo.w << endl
				  << "Height: " << gDisplayInfo.h << endl
				  << "Refresh: " << gDisplayInfo.refresh_rate << endl;
			
			// Creates window
			cout << "Creating Window. ";
			gPtrWindow = SDL_CreateWindow( "Four Squares", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, gDisplayInfo.w, gDisplayInfo.h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
			if( gPtrWindow == NULL ){
				cout << "There was an error creating window. Details: " << SDL_GetError() << endl;
				sucess = false;
			}
			else{
				cout << "\nCreating renderer context. " << endl;
				gPtrRenderer = SDL_CreateRenderer( gPtrWindow, -1, SDL_RENDERER_ACCELERATED );
				if( gPtrRenderer == NULL ){
					cout << "Renderer could not be created! Details: " << SDL_GetError() << endl;
					sucess = false;
				}
				else{
					// Enables  blend mode
					SDL_SetRenderDrawBlendMode( gPtrRenderer, SDL_BLENDMODE_BLEND );
					
					//Initializes renderer color
					SDL_SetRenderDrawColor( gPtrRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
					
					//Carga el ícono
					SDL_Surface * icono = IMG_Load("Recursos/Icono.bmp");
					if( icono != NULL )
					{
						SDL_SetWindowIcon( gPtrWindow, icono );
						SDL_FreeSurface(icono);
					}
					
					// Loads user's preferences or initializes the game options
					loadPreferences();
			
					// Sets preferences loaded
					setPreferences();
				
					setState( GAME_STATE_GAME );
					changeState();
				}
			}
		}
	}
	
	return sucess;
}

void loadPreferences()
{
	/* INSERTE AQUÌ CÒDIGO PARA CARGAR PREFERENCIAS DEL USUARIO DESDE ARCHIVO */
	gGameChange = true;
	gImageResolution = RES_800_480;
	gImageQuality = QUALITY_NORMAL;
	gDisplayMode = false;;
	gVsync = true;
	showFPS = false;
}

void setPreferences()
{
	if( gGameChange ){
		// Sets image quality
		switch( gImageQuality ){
			case QUALITY_LOW:
				SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0" );
				break;
			case QUALITY_NORMAL:
				SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" );
				break;
		}
		
		// Sets resolution
		switch( gImageResolution ){
			case RES_1920_1080:
				gDisplayWidth = 1920;
				gDisplayHeight = 1080;
				break;
			case RES_1600_900:
				gDisplayWidth = 1600;
				gDisplayHeight = 900;
				break;
			case RES_1366_768:
				gDisplayWidth = 1366;
				gDisplayHeight = 768;
				break;
			case RES_1260_720:
				gDisplayWidth = 1260;
				gDisplayHeight = 720;
				break;
			case  RES_1024_768:
				gDisplayWidth = 1024;
				gDisplayHeight = 768;
				break;
			case RES_800_480:
				gDisplayWidth = 800;
				gDisplayHeight = 480;
				break;
			case RES_AUTO:
				gDisplayWidth = gDisplayInfo.w;
				gDisplayHeight = gDisplayInfo.h;
				break;
		}
		
		// Set new window's size
		SDL_SetWindowSize( gPtrWindow, gDisplayWidth, gDisplayHeight );
		
		// Update render context size
		SDL_RenderSetLogicalSize( gPtrRenderer, gDisplayWidth, gDisplayHeight );
		
		// Sets display mode
		if( gDisplayMode ){
			// Activates fullscreen
			SDL_SetWindowFullscreen( gPtrWindow, SDL_WINDOW_FULLSCREEN );
		}
		else{
			// Sets windowing mode
			SDL_SetWindowFullscreen( gPtrWindow, 0 );
		}
		
		// Sets vsync
		if( gVsync ){
			if( !SDL_SetHint( SDL_HINT_RENDER_VSYNC, "1" ) )
				;// Inserte código de error aquí
		}
		else{
			if( !SDL_SetHint( SDL_HINT_RENDER_VSYNC, "0" ) )
				;// Inserte código de error aquí
		}
	}
}

void updateWindowSize( void )
{
	// Get window size
	SDL_GetWindowSize( gPtrWindow, &gDisplayWidth, &gDisplayHeight );
	
	//  Make sure resolution is more or equal to minimum width
	if( gDisplayWidth < MIN_WIDTH ){
		gDisplayWidth = MIN_WIDTH;
	}
	
	// Make sure resolution is more or equal to minimum height
	if( gDisplayHeight < MIN_HEIGHT ){
		gDisplayHeight = MIN_HEIGHT;
	}
	
	// Set new window's size
	SDL_SetWindowSize( gPtrWindow, gDisplayWidth, gDisplayHeight );
	
	// Update render context size
	SDL_RenderSetLogicalSize( gPtrRenderer, gDisplayWidth, gDisplayHeight );
	
	// Updates resolution flag
	gImageResolution = RES_AUTO;
};

void gameUpdateViewport()
{
	// Updates display's unit
	gameUnitSize = gDisplayHeight / DISPLAY_UNIT;
	
	// Updates viewport dimensions
	gameViewport.h = DISPLAY_UNIT;
	gameViewport.w = gDisplayWidth / gameUnitSize;
	gameViewport.x = 0.0;
	gameViewport.y = 0.0;
}

void close( void )
{
	delete gGameState;
	
	// Deletes everything of the game
	SDL_DestroyRenderer( gPtrRenderer );
	SDL_DestroyWindow( gPtrWindow );
	gPtrWindow = NULL;
	gPtrRenderer = NULL;
}

void setState( GameStates state )
{
	if( gActualState != GAME_STATE_EXIT || state == GAME_STATE_EXIT ){
		gActualState = state;
	}
}

void changeState( void )
{
	if( gActualState != GAME_STATE_NULL ){
		if( gActualState != GAME_STATE_EXIT ){
			delete gGameState;
		}
		switch( gActualState ){
			case GAME_STATE_GAME:
				gGameState = new Tetroblock();
				gActualState = GAME_STATE_NULL;
				break;
		}
	}
}
