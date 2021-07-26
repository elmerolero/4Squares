#include "game.h"
#include <iostream>
#include <vector>
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

// Para el dibujao
SDL_Window * gPtrWindow;
SDL_Renderer * gPtrRenderer;
SDL_DisplayMode gDisplayInfo;
SDL_Event gGameEvent;

// Variables para viewport y tamaño de unidad
SDL_DRect gameViewport;

// Temporizador
Temporizador tempFPS;
int fps;

// Para el estado del juego
bool estadoSalir;
EstadoJuego *estadoCambiar;
EstadoJuego *estadoJuego;
vector< EstadoJuego * > estadosJuego;

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

bool esperarEvento;

// FPS textura
Object fpsObjeto;
Texture fpsTextura;

// Fuentes
TTF_Font *fuenteArg;
TTF_Font *fuenteAllStar;

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
	gPtrWindow = SDL_CreateWindow( nombre.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
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

	Texture::establecerContextoRender( gPtrRenderer );

	//Carga el ícono
	SDL_Surface * icono = IMG_Load("../recursos/img/icono.bmp");
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

	// Inicializa el temporizador de imagenes por segundo
	tempFPS.iniciar();

	return true;
}

void Juego_CargarPreferencias( void )
{
	jSalir = false;
	jPantallaCompleta = false;
	jHabilitarSincV = "0";
	jMostrarTasaCuadros = false;
	jCalidadJuego = "1";
	jAnchoPantalla = gDisplayInfo.w;
	jAltoPantalla = gDisplayInfo.h;
}

void Juego_EstablecerPreferencias( void )
{
	// Pantalla completa
	Juego_EstablecerPantallaCompleta( jPantallaCompleta );

	// Tamaño de la ventana
	Juego_ActualizarVentana();

	// Sincronización vertical
	SDL_SetHint( SDL_HINT_RENDER_VSYNC, jHabilitarSincV.c_str() );

	// Calidad del juegos
	SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, jCalidadJuego.c_str() );
}

void Juego_EstablecerPantallaCompleta( bool pantallaCompleta )
{
	// Establece pantalla completa
	jPantallaCompleta = pantallaCompleta;

	// Si se desea pantalla completa
	if( jPantallaCompleta ){
		// Establece las dimensiones del juego
		jAnchoPantalla = gDisplayInfo.w;
		jAltoPantalla = gDisplayInfo.h;
	}
	else{
		jAnchoPantalla = 1024;
		jAltoPantalla = 600;
	}

	// Establece el tamaño de la ventana
	SDL_SetWindowSize( gPtrWindow, jAnchoPantalla, jAltoPantalla );
	SDL_RenderSetLogicalSize( gPtrRenderer, jAnchoPantalla, jAltoPantalla );

	// Establece a pantalla completa
	SDL_SetWindowFullscreen( gPtrWindow, jPantallaCompleta );

	// Actualiza el tamaño de la unidad y las dimensiones del viewport
	gameUnitSize = jAltoPantalla / DISPLAY_UNIT;
	gameViewport.h = DISPLAY_UNIT;
	gameViewport.w = jAnchoPantalla / gameUnitSize;
	gameViewport.x = 0.0;
	gameViewport.y = 0.0;

	// Si hay un estado de juego establecido
	for( EstadoJuego *estadoJuego : estadosJuego ){
		estadoJuego -> actualizarViewport();
	}
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
	Objeto_ActualizarMagnitudUnidad( jAltoPantalla );
	gameViewport.h = UNIDAD_PANTALLA;
	gameViewport.w = jAnchoPantalla / gameUnitSize;
	gameViewport.x = 0.0;
	gameViewport.y = 0.0;

	// Si hay un estado de juego establecido
	for( EstadoJuego *estadoJuego : estadosJuego ){
		estadoJuego -> actualizarViewport();
	}
};

void Juego_Finalizar( void ){
	delete estadoJuego;
	
	// Deletes everything of the game
	SDL_DestroyRenderer( gPtrRenderer );
	SDL_DestroyWindow( gPtrWindow );
	SDL_Quit();
	gPtrWindow = NULL;
	gPtrRenderer = NULL;
}

void EstadoJuego_EstablecerEstado( vector< EstadoJuego * > &estadosJuego, EstadoJuego *estado ){
	// Limpia los estados del juego que se encuentren
	EstadoJuego_LimpiarEstados( estadosJuego );

	// Apila el estado del juego
	EstadoJuego_ApilarEstado( estadosJuego, estado );
}

void EstadoJuego_CambiarEstado( EstadoJuego *estado ){
	if( estado != nullptr ){
		estadoCambiar = estado;
	}
}

// Elimina todos los estados de juego que existan en una pila
void EstadoJuego_LimpiarEstados( vector< EstadoJuego * > &estadosJuego ){
	// Mientras haya estados de juego apilados
	while( !estadosJuego.empty() ){
		// Obtiene el estado de juego a eliminar
		EstadoJuego *estado = estadosJuego.back();

		// Elimina ese elemento
		delete estado;
		
		// Lo elimina de la pila
		estadosJuego.pop_back();
	}
}

void EstadoJuego_ApilarEstado( vector< EstadoJuego * > &estadosJuego, EstadoJuego *estado ){
	// Apila el estado de juego solicitado
	estadosJuego.push_back( estado );
	
	// Lo establece como el actual
	estadoJuego = estadosJuego.back();
}

void EstadoJuego_Salir( void ){
	estadoSalir = true;
}

void EstadoJuego_Salir( vector< EstadoJuego * > &estadosJuego ){
	if( !estadoSalir ){
		return;
	}

	if( estadosJuego.size() > 1 ){
		// Retira el estado de juego actual
		estadosJuego.pop_back();

		// Regresa al estado de juego anterior
		estadoJuego = estadosJuego.back();
	}

	estadoSalir = false;
}

void EstadoJuego_Entrada( void ){
	if( SDL_PollEvent( &gGameEvent ) != 0 ){
		// Ejecuta sus funciones base
		if( gGameEvent.type == SDL_QUIT ){
			jSalir = true;
		}
        else if( gGameEvent.type == SDL_KEYDOWN ){
			if( gGameEvent.key.keysym.sym == SDLK_ESCAPE ){
				Juego_EstablecerPantallaCompleta( !jPantallaCompleta );
			}
			else if( gGameEvent.key.keysym.sym == SDLK_f ){
				jMostrarTasaCuadros = !jMostrarTasaCuadros;
			}
		}
		else if( gGameEvent.type == SDL_WINDOWEVENT ){
			if( gGameEvent.window.event == SDL_WINDOWEVENT_RESIZED ){
				Juego_ActualizarVentana();
			}
		}

		// Llama a estado eventos
		estadoJuego -> estadoEventos();	
	}

	// Llama a estado entrada
	estadoJuego -> estadoEntrada();
}

void EstadoJuego_Logica( void ){
	// Reinica la tasa de fotogramas
	if( tempFPS.obtenerTicks() >= 1000 ){
		FS_ActualizarDatos( fps, fpsTextura, fpsObjeto, 2, fuenteArg, 0.f, 0.f );
		tempFPS.reiniciar();
		fps = 0;
	}

	// Solo ejecuta la lógica del estado actual
	estadoJuego -> estadoLogica();
}

void EstadoJuego_Renderizar( void ){
	// Limpia la pantalla
	SDL_SetRenderDrawColor( gPtrRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
	SDL_RenderClear( gPtrRenderer );
		
	// Dibuja los estados del juego
	for( EstadoJuego *estado : estadosJuego ){
		estado -> estadoRenderizado();
	}

	// Dibuja la tasa de fotogramas por segundo
	if( jMostrarTasaCuadros ){
		fpsTextura.renderTexture( fpsObjeto.getSrcRect(), fpsObjeto.getDestRect() );
	}

	// Actualiza la pantalla
	SDL_RenderPresent( gPtrRenderer );

	// Incrementa el numero de fps
	fps++;
}

void EstadoJuego_Actualizar( void ){
	if( estadoCambiar != nullptr ){
		EstadoJuego_EstablecerEstado( estadosJuego, estadoCambiar );
	}
}

void FS_ActualizarDatos( int dato, Texture &textura, Object &objeto, int relleno, TTF_Font *fuente, double x, double y )
{
	stringstream datoStr;
	datoStr << setfill( '0' ) << setw( relleno ) << dato;

	// Fuente
	SDL_Color color = { 255, 255, 255 };
	if( !textura.crearTexturaDesdeTextoSolido( datoStr.str().c_str(), color, fuente ) ){
		jSalir = true;
		return;
	}

	SDL_Rect acoords = { 0, 0, textura.getWidth(), textura.getHeight() };
	SDL_DRect rcoords = { x, y, (float)textura.getWidth() / gameUnitSize, (float)textura.getHeight() / gameUnitSize };
	objeto.setRelativeCoords( rcoords );
	objeto.setTextureCoords( acoords );
}