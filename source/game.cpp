#include "game.h"
#include "foursquares.h"
#include <iostream>
using namespace std;

/* Notas importantes */
// Para obtener el tamaño (t) en unidades relativas se utiliza la siguiente fórmula:
//     t = ( p / u ) * a
// Donde 'p' es el tamaño en pixeles del objeto, 'u' es el tamaño en unidades de la 
// altura de la pantalla y 'a' es la altura de la pantalla en pixeles.
//
// La nueva medida en pixeles (p) al cambiar el tamaño de la pantalla se obtiene de 
// siguiente forma:
//     p = t * u
// Donde 't' es el tamaño o posición relativo y 'u' es el tamaño de la unidad en pixeles. 

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
bool jSalir;
Uint32 jPantallaCompleta;
string jHabilitarSincV;
string jCalidadJuego;
bool jMostrarTasaCuadros;
int  jAnchoPantalla;
int  jAltoPantalla;
EstadoJuego *estadoJuego;

bool Juego_Iniciar( std::string nombre )
{
	// Inicializa SDL
	cout << "\nInitializing SDL lib. ";
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 || (IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG) != IMG_INIT_PNG || TTF_Init() < 0 ){
		cout << "Error initilizing SDL. Details SDL: " << SDL_GetError() << " IMG: " << IMG_GetError() << endl; 
		return false;
	}
	
	// Gets display options and check display was not too small
	if( SDL_GetCurrentDisplayMode( 0, &gDisplayInfo ) != 0 ){
		cout << "\nError getting current display mode. Error: " << SDL_GetError() << endl;
		
		return false;
	}
	
	cout << "\nVideo initialized with current display properties:\n"
		 << "Width: " << gDisplayInfo.w << endl
		 << "Height: " << gDisplayInfo.h << endl
		 << "Refresh: " << gDisplayInfo.refresh_rate << endl;
			
	// Crea una ventana
	cout << "Creating Window. ";
	gPtrWindow = SDL_CreateWindow( nombre.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
	if( gPtrWindow == NULL ){
		cout << "There was an error creating window. Details: " << SDL_GetError() << endl;
		return false;
	}

	cout << "\nCreating renderer context. " << endl;
	gPtrRenderer = SDL_CreateRenderer( gPtrWindow, -1, SDL_RENDERER_ACCELERATED );
	if( gPtrRenderer == NULL ){
		cout << "Renderer could not be created! Details: " << SDL_GetError() << endl;
		return false;
	}

	//Carga el ícono
	SDL_Surface * icono = IMG_Load("recursos/img/icono.bmp");
	if( icono != NULL ){
		SDL_SetWindowIcon( gPtrWindow, icono );
		SDL_FreeSurface(icono);
	}
					
	// Habilita el mode de mezcla
	SDL_SetRenderDrawBlendMode( gPtrRenderer, SDL_BLENDMODE_BLEND );
					
	// Loads user's preferences or initializes the game options
	Juego_CargarPreferencias();

	// Sets preferences loaded
	Juego_EstablecerPreferencias();

	estadoJuego = new FourSquares();

	return true;
}

void Juego_CargarPreferencias( void )
{
	jSalir = false;
	jPantallaCompleta = false;
	jHabilitarSincV = "0";
	jMostrarTasaCuadros = false;
	jCalidadJuego = "1";
	jAnchoPantalla = 800;
	jAltoPantalla = 480;
}

void Juego_EstablecerPreferencias( void )
{
	// Pantalla completa deshabilitada
	SDL_SetWindowFullscreen( gPtrWindow, jPantallaCompleta );

	// Sincronización vertical
	SDL_SetHint( SDL_HINT_RENDER_VSYNC, jHabilitarSincV.c_str() );

	// Calidad del juegos
	SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, jCalidadJuego.c_str() );

	// Tamaño de la ventana
	Juego_ActualizarVentana();
}

// Actualiza el tamaño de la ventana
void Juego_ActualizarVentana( void )
{
	// Obtiene el tamaño de la ventana
	SDL_GetWindowSize( gPtrWindow, &jAnchoPantalla, &jAltoPantalla );

	// Establece el nuevo tamaño de la ventana
	SDL_SetWindowSize( gPtrWindow, jAnchoPantalla, jAltoPantalla );
	
	// Actualiza el tamaño del área de renderizado
	SDL_RenderSetLogicalSize( gPtrRenderer, jAnchoPantalla, jAltoPantalla );

	// Actualiza el tamaño de la unidad y las dimensiones del viewport
	gameUnitSize = jAltoPantalla / DISPLAY_UNIT;
	gameViewport.h = DISPLAY_UNIT;
	gameViewport.w = jAnchoPantalla / gameUnitSize;
	gameViewport.x = 0.0;
	gameViewport.y = 0.0;
};

void Juego_Finalizar( void )
{
	delete estadoJuego;
	
	// Deletes everything of the game
	SDL_DestroyRenderer( gPtrRenderer );
	SDL_DestroyWindow( gPtrWindow );
	SDL_Quit();
	gPtrWindow = NULL;
	gPtrRenderer = NULL;
}
