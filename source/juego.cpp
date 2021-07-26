#include "juego.h"
#include "database.h"
#include <stdexcept>
#include <iostream>
#include "texture.h"
#include "componentes.h"
using namespace std;

// Constructor
Juego::Juego(): jVentana( nullptr ), jRender( nullptr ), estadoActual( nullptr ){
	// Nada por hacer
}

Juego::Juego( string nombre, string traducciones ){
	// Inicia el juego
	this -> iniciar( nombre, traducciones );

	// Estado actual
	estadoActual = nullptr;
}

void Juego::iniciar( string nombre, string traducciones ){
	// Carga las traducciones
	cargarValoresMensaje( traducciones );

    // Inicializa SDL
	cout << "\n" << leerValor( "inicializacion_sdl" ) << endl;
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 || (IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG) != IMG_INIT_PNG || TTF_Init() < 0 ){
		throw runtime_error( leerValor( "error_inicializacion_sdl" ) + SDL_GetError() + " IMG: " + IMG_GetError() );
	}
	
	// Gets display options and check display was not too small
	if( SDL_GetCurrentDisplayMode( 0, &jVistaInfo ) != 0 ){
        throw runtime_error( leerValor( "error_inicializacion_pantalla" ) + SDL_GetError() + " IMG: " + IMG_GetError() );
	}
	
	cout << "\n" + leerValor( "exitoso_inicializacion_pantalla" ) << endl
		 << leerValor( "literal_ancho" ) << jVistaInfo.w << endl
		 << leerValor( "literal_alto" ) << jVistaInfo.h << endl
		 << leerValor( "literal_tasa_refresco" ) << jVistaInfo.refresh_rate << endl;
			
	// Crea una ventana
	cout << leerValor( "crear_ventana" ) << endl;
	jVentana = SDL_CreateWindow( nombre.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
	if( jVentana == nullptr ){
        throw runtime_error( leerValor( "error_crear_ventana" ) + SDL_GetError() + " IMG: " + IMG_GetError() );
	}

	cout << leerValor( "crear_render" ) << endl;
	jRender = SDL_CreateRenderer( jVentana, -1, SDL_RENDERER_ACCELERATED );
	if( jRender == nullptr ){
        throw runtime_error( leerValor( "error_crear_render" ) + SDL_GetError() );
	}

	//Carga el ícono
	SDL_Surface * icono = IMG_Load("../recursos/img/icono.bmp");
	if( icono != NULL ){
		SDL_SetWindowIcon( jVentana, icono );
		SDL_FreeSurface(icono);
	}

	Texture::establecerContextoRender( jRender );
					
	// Habilita el mode de mezcla
	SDL_SetRenderDrawBlendMode( jRender, SDL_BLENDMODE_BLEND );

	// Inicializa el temporizador de imagenes por segundo
	temporizadorFPS.iniciar();

	// Loads user's preferences or initializes the game options
	//cargarPreferencias();

	// Fuente
	fuenteTexto.archivo = "../recursos/fuentes/Aaargh.ttf";
	fuenteTexto.magnitud = 47;

	// Actualiza la ventana
	actualizarVentana();
}

// libera los recursos del juego
void Juego::cerrar( void ){
	while( !estadosPendientes.empty() ){
		Pendiente *aux = estadosPendientes.back();
		delete aux;
		estadosPendientes.pop_back();
	}

	while( !jEstados.empty() ){
		EstadoJuego *aux = jEstados.back();
		delete aux;
		estadosPendientes.pop_back();
	}

	// Deletes everything of the game
	SDL_DestroyRenderer( jRender );
	SDL_DestroyWindow( jVentana );
	SDL_Quit();
	jRender = NULL;
	jVentana = NULL;
}

// Lee la entrada del juego
void Juego::entrada( void ){
	// Revisa la cola de eventos
	if( SDL_PollEvent( &jEvento ) != 0 ){
		// Ejecuta sus funciones base
		if( jEvento.type == SDL_QUIT ){
			salir( true );
		}
        else if( jEvento.type == SDL_KEYDOWN ){
			if( jEvento.key.keysym.sym == SDLK_ESCAPE ){
				//Juego_EstablecerPantallaCompleta( !jPantallaCompleta );
			}
			else if( jEvento.key.keysym.sym == SDLK_f ){
				jMostrarTasaCuadros = !jMostrarTasaCuadros;
			}
		}
		else if( jEvento.type == SDL_WINDOWEVENT ){
			if( jEvento.window.event == SDL_WINDOWEVENT_RESIZED ){
				actualizarVentana();
			}
		}

		// Llama a estado eventos
		if( estadoActual != nullptr ){
			estadoActual -> estadoEventos( jEvento );	
		}
	}

	// Llama a estado entrada
	if( estadoActual != nullptr ){
		estadoActual -> estadoEntrada();
	}
}

// Ejecuta la lógica del juego
void Juego::logica( void ){
	// Reinica la tasa de fotogramas
	if( temporizadorFPS.obtenerTicks() >= 1000 ){
		if( jMostrarTasaCuadros ){
			Fuente_ActualizarTexto( to_string( contadorFPS ), fuenteTexto, fpsTexto );
		}
		temporizadorFPS.reiniciar();
		contadorFPS = 0;
	}

	// Solo ejecuta la lógica del estado actual
	if( estadoActual != nullptr ){
		estadoActual -> estadoLogica();
	}
}

// Realiza el renderizado
void Juego::renderizar( void ){
	// Limpia la pantalla
	SDL_SetRenderDrawColor( jRender, 0xFF, 0xFF, 0xFF, 0xFF );
	SDL_RenderClear( jRender );
		
	// Dibuja los estados del juego
	for( EstadoJuego *estado : jEstados ){
		if( estado != nullptr ){
			estado -> estadoRenderizado();
		}
	}

	// Dibuja la tasa de fotogramas por segundo
	if( jMostrarTasaCuadros ){
		fpsTexto.renderTexture( fpsTexto.leerDimensionesTextura(), fpsTexto.leerDimensionesEspaciales() );
	}

	// Actualiza la pantalla
	SDL_RenderPresent( jRender );

	// Incrementa el numero de fps
	contadorFPS++;
}

// Dibuja un fondo negro transparente
void Juego::fondoNegro( void ){
	SDL_Rect rect = { 0, 0, jPantallaAncho, jPantallaAlto }; 
	SDL_SetRenderDrawColor( jRender, 0x00, 0x00, 0x00, 0x88 );
    SDL_RenderFillRect( jRender, &rect );
}

// Obtiene el contexto de renderizado
SDL_Renderer *Juego::leerConextoRender( void ){
	return jRender;
}

// Actualiza la ventana
void Juego::actualizarVentana( void ){
	// Obtiene el tamaño de la ventana
	SDL_GetWindowSize( jVentana, &jPantallaAncho, &jPantallaAlto );

	// Establece el nuevo tamaño de la ventana
	SDL_SetWindowSize( jVentana, jPantallaAncho, jPantallaAlto );
	
	// Actualiza el tamaño del área de renderizado
	SDL_RenderSetLogicalSize( jRender, jPantallaAncho, jPantallaAlto );

	// Actualiza el tamaño de la unidad y las dimensiones del viewport
	Objeto::actualizarMagnitudUnidad( jPantallaAlto );
	escribirEspacioX( 0.f );
	escribirEspacioY( 0.f );
	escribirEspacioAlto( Objeto::UNIDAD_PANTALLA );
	escribirEspacioAncho( jPantallaAncho / Objeto::leerMagnitudUnidad() );

	// Actualiza el tamaño de la fuente
	Fuente_ActualizarTamanio( fuenteTexto );

	// Si hay un estado de juego establecido
	for( EstadoJuego *estadoJuego : jEstados ){
		if( estadoJuego != nullptr ){
			estadoJuego -> actualizarViewport();
		}
	}
}

// Finaliza el estado actual que se está ejecutando
void Juego::finalizarEstado( void ){
	// Cree un nuevo pendiente
	Pendiente *pendiente = new Pendiente;

	// Indica que el estado del juego actual debe ser eliminado
	pendiente -> accion = ESTADO_FINALIZAR;

	// Establece un puntero nulo
	pendiente -> estado = nullptr;

	// Lo agrega en la cola de pendientes
	estadosPendientes.push_back( pendiente );
}

// Agrega un estado que se ejecutará sobre el estado actual del juego
void Juego::apilarEstado( EstadoJuego *estado ){
	// Verifica que el estado que se desea apilar no es nulo
	if( estado == nullptr ){
		throw invalid_argument( leerValor( "apilacion_elemento_nulo" ) );
	}

	// Crea un pendiente nuevo
	Pendiente *pendiente = new Pendiente;

	// Indica que se hará una apilación de estado
	pendiente -> accion = ESTADO_APILAR;

	// Establece el estado que se apilará en el juego
	pendiente -> estado = estado;

	// Lo agrega a la cola
	estadosPendientes.push_back( pendiente );
}

// Establece un estado de juego que removerá todos los estados actuales
void Juego::establecerEstado( EstadoJuego *estado ){
	// Verifica que no se desea establecer un estado vacío
	if( estado == nullptr ){
		throw invalid_argument( leerValor( "establecimiento_elemento_nulo" ) );
	}

	// Crea un pendiente
	Pendiente *pendiente = new Pendiente;

	// Indica que se establecerá un nuevo estado
	pendiente -> accion = ESTADO_ESTABLECER;

	// Indica el estado que se establecera
	pendiente -> estado = estado;

	// Lo agrega a la cola
	estadosPendientes.push_back( pendiente );
}

// Actualiza el estado de juego
void Juego::actualizarEstado( void ){
	// Sale si no hay cambios que hacer
	if( estadosPendientes.empty() ){
		return;
	}

	while( !estadosPendientes.empty() ){
		// Obtiene el estado pendiente del frente
		Pendiente *pendiente = estadosPendientes.front();

		// Apila el estado si así se indica
		if( pendiente -> accion == ESTADO_APILAR ){
			// Apila el estado pendiente
			jEstados.push_back( pendiente -> estado );
		}
		// Establece el estado si así se indica
		else if( pendiente -> accion == ESTADO_ESTABLECER ){
			// Mientras haya estados en la pila
			while( !jEstados.empty() ){
				// Retira el estado de la pila
				estadoActual = jEstados.back();
				delete estadoActual;
				jEstados.pop_back();
			}

			// Apila el nuevo estado
			jEstados.push_back( pendiente -> estado );
		}
		// Retira el estado actual si desea finalizar
		else if( pendiente -> accion == ESTADO_FINALIZAR ){
			// Si la pila no esta vacía
			if( !jEstados.empty() ){
				// Lo retira de la pila de estados
				estadoActual = jEstados.back();
				delete estadoActual;
				jEstados.pop_back();
			}
		}

		// Elimina el pendiente
		delete pendiente;
		estadosPendientes.pop_front();
		pendiente = nullptr;
	}

	// Si la pila de estados se encuenta vacía
	if( jEstados.empty() ){
		estadoActual = nullptr;
		jSalir = true;
		return;
	}

	// Establece el estado actual
	estadoActual = jEstados.back();
}

// Indica que hay que slair
void Juego::salir( bool salir ){
	jSalir = salir;
}

// Indica que saldrá
bool Juego::salir( void ) const{
	return jSalir;
}

// Obtiene las dimensiones de la pantalla
int Juego::leerVistaX( void ) const{
	return jPantallaX;
}

int Juego::leerVistaY( void ) const{
	return jPantallaY;
}

int Juego::leerVistaAncho( void ) const{
	return jPantallaAncho;
}

int Juego::leerVistaAlto( void ) const{
	return jPantallaAlto;
}

// Obtiene las dimensiones espaciales del juego
void Juego::escribirEspacioX( double x ){
	jEspacioX = x;
}

void Juego::escribirEspacioY( double y ){
	jEspacioY = y;
}
void Juego::escribirEspacioAncho( double ancho ){
	jEspacioAncho = ancho;
}

void Juego::escribirEspacioAlto( double alto ){
	jEspacioAlto = alto;
}

double Juego::leerEspacioX( void ) const{
	return jEspacioX;
}

double Juego::leerEspacioY( void ) const{
	return jEspacioY;
}

double Juego::leerEspacioAncho( void ) const{
	return jEspacioAncho;
}

double Juego::leerEspacioAlto( void ) const{
	return jEspacioAlto;
}

// Obtiene el tamaño de la unidad (se utiliza magnitud porque tamaño tiene ñ)
double Juego::leerMagnitudUnidad( void ) const{
	return magnitudUnidad;
}

// Para manejar las variables String
void Juego::cargarValoresMensaje( string archivo ){
	// Archivo donde guardará la información
	Database database;

	// Abre la base de datos
	database.open( archivo );
	database.query( "select * from mensajes" );
	database.close();

	// Establece los valores
	if( results.size() > 0 ){
		// Para cada elemento de los resultados
		for( auto elemento : results ){
			// Agrega el elemento a la lista de valores string
			establecerMensaje( (* elemento)[ "nombre" ], (* elemento)[ "valor" ] );
		}
	}
}

void Juego::establecerMensaje( string valor, string mensaje ){
	valoresMensajes.insert( pair< string, string >( valor, mensaje ) );
}

string Juego::leerValor( string valorMensaje ){
    return valoresMensajes[ valorMensaje ];
}

void Fuente_ActualizarTamanio( Fuente &fuente )
{
	// Fuente
	TTF_CloseFont( fuente.fuente );
	fuente.fuente = TTF_OpenFont( fuente.archivo.c_str(), (int)( ( (float)fourSquares.leerVistaAlto() / 1080.f ) * fuente.magnitud ) );
	if( fuente.fuente == nullptr ){
		cout << "Error al cargar la fuente. Error: " << TTF_GetError() << endl;
	}
}

void Fuente_ActualizarTexto( string texto, Fuente &fuente, Objeto &objeto )
{
	// Fuente
	SDL_Color color = { 255, 255, 255 };
	if( !objeto.crearTexturaDesdeTextoSolido( texto.c_str(), color, fuente.fuente ) ){
		throw runtime_error( "Ocurrió un error al crear la textura de texto." );
	}

	// Establece las dimensiones de textura y de espacio
	objeto.escribirDimensionesTextura( 0, 0, objeto.getWidth(), objeto.getHeight() );
	objeto.escribirDimensionesEspaciales( 0, 0, (float)objeto.getWidth() / Objeto::leerMagnitudUnidad(), (float)objeto.getHeight() / Objeto::leerMagnitudUnidad() );
}

Fuente fuenteTexto;
Objeto fpsTexto;