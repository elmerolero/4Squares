#include "juego.h"
#include "globales.h"
#include "database.h"
#include <stdexcept>
#include <iostream>
#include "texture.h"
using namespace std;

void Juego_Iniciar( string nombre, string mensajes ){
	// Carga las traducciones
	Juego_CargarValoresMensaje( mensajes );

    // Inicializa SDL
	cout << "\n" << valoresMensaje[ "inicializacion_sdl" ] << endl;
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 || (IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG) != IMG_INIT_PNG || TTF_Init() < 0 ){
		throw runtime_error( valoresMensaje[ "error_inicializacion_sdl" ] + SDL_GetError() + " IMG: " + IMG_GetError() );
	}
	
	// Gets display options and check display was not too small
	if( SDL_GetCurrentDisplayMode( 0, &gVistaInfo ) != 0 ){
        throw runtime_error( valoresMensaje[ "error_inicializacion_pantalla" ] + SDL_GetError() + " IMG: " + IMG_GetError() );
	}
	
	cout << "\n" + valoresMensaje[ "exitoso_inicializacion_pantalla" ] << endl
		 << valoresMensaje[ "literal_ancho" ] << gVistaInfo.w << endl
		 << valoresMensaje[ "literal_alto" ] << gVistaInfo.h << endl
		 << valoresMensaje[ "literal_tasa_refresco" ] << gVistaInfo.refresh_rate << endl;
			
	// Crea una ventana
	cout << valoresMensaje[ "crear_ventana" ] << endl;
	gVentana = SDL_CreateWindow( nombre.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
	if( gVentana == nullptr ){
        throw runtime_error( valoresMensaje[ "error_crear_ventana" ] + SDL_GetError() + " IMG: " + IMG_GetError() );
	}

	cout << valoresMensaje[ "crear_render" ] << endl;
	gRender = SDL_CreateRenderer( gVentana, -1, SDL_RENDERER_ACCELERATED );
	if( gRender == nullptr ){
        throw runtime_error( valoresMensaje[ "error_crear_render" ] + SDL_GetError() );
	}

	//Carga el ícono
	SDL_Surface * icono = IMG_Load( "../recursos/img/icono.bmp" );
	if( icono != NULL ){
		SDL_SetWindowIcon( gVentana, icono );
		SDL_FreeSurface(icono);
	}

	Texture::establecerContextoRender( gRender );
					
	// Habilita el mode de mezcla
	SDL_SetRenderDrawBlendMode( gRender, SDL_BLENDMODE_BLEND );

	// Inicializa el temporizador de imagenes por segundo
	temporizadorFPS.iniciar();

	// Loads user's preferences or initializes the game options
	//cargarPreferencias();

	// Fuente
	fuenteTexto.archivo = "../recursos/fuentes/fredoka.ttf";
	fuenteTexto.magnitud = 47;

	// Intenta cargar un control de juego
	if( SDL_NumJoysticks() > 0 ){
		for( unsigned int index = 0; index < SDL_NumJoysticks(); ++index ){
			if( SDL_IsGameController( index ) ){
				joystickConectado = true;
				cout << "Encontré un joystick." << endl;
				// Abre el control
				controles[ index ].control = SDL_GameControllerOpen( index );
				controles[ index ].joystick = SDL_GameControllerGetJoystick( controles[ 0 ].control );
				controles[ index ].id = SDL_JoystickInstanceID( controles[ 0 ].joystick );

				// Establece las opciones
				controles[ index ].soltarPieza = SDL_CONTROLLER_BUTTON_DPAD_UP;
				controles[ index ].bajarPieza = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
				controles[ index ].moverDerecha = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
				controles[ index ].moverIzquierda = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
				controles[ index ].rotarDerecha = SDL_CONTROLLER_BUTTON_A;
				controles[ index ].rotarIzquierda = SDL_CONTROLLER_BUTTON_X;
				controles[ index ].reservarPieza = SDL_CONTROLLER_BUTTON_B;
				controles[ index ].pausarJuego = SDL_CONTROLLER_BUTTON_START;

				SDL_JoystickEventState( 1 );
			}
		}
	}

	// Establece las opciones del teclado
	teclado.soltarPieza = SDLK_UP;
	teclado.bajarPieza = SDL_SCANCODE_DOWN;
	teclado.moverDerecha = SDL_SCANCODE_RIGHT;
	teclado.moverIzquierda = SDL_SCANCODE_LEFT;
	teclado.rotarDerecha = SDLK_x;
	teclado.rotarIzquierda = SDLK_z;
	teclado.reservarPieza = SDLK_LSHIFT;
	teclado.pausarJuego = SDLK_RETURN;

	// Actualiza la ventana
	Juego_ActualizarVentana();
}

// libera los recursos del juego
void Juego_Cerrar( void ){
	// Elimina los estados pendientes
	Juego_LimpiarPendientes();

	// Elimina los estados en ejecución
	Juego_LimpiarEstados();

	// Libera los recursos del juego
	SDL_DestroyRenderer( gRender );
	SDL_DestroyWindow( gVentana );
	SDL_Quit();
	gRender = NULL;
	gVentana = NULL;
}

// Lee la entrada del juego
void Juego_Entrada( void ){
	// Revisa la cola de eventos
	if( SDL_PollEvent( &gEvento ) != 0 ){
		// Ejecuta sus funciones base
		if( gEvento.type == SDL_QUIT ){
			salir = true;
		}
        else if( gEvento.type == SDL_KEYDOWN ){
			if( gEvento.key.keysym.sym == SDLK_ESCAPE ){
				pantallaCompleta = !pantallaCompleta;
				SDL_SetWindowSize( gVentana, 800, 480 );
				if( pantallaCompleta ){
					SDL_SetWindowSize( gVentana, gVistaInfo.w, gVistaInfo.h );
				}
				Juego_ActualizarVentana();
				//Juego_EstablecerPantallaCompleta( !pantallaCompleta );
			}
			else if( gEvento.key.keysym.sym == SDLK_f ){
				mostrarFPS = !mostrarFPS;
			}
			else if( estadoActual != nullptr ){
				estadoActual -> estadoEventos( gEvento );
			}
		}
		else if( gEvento.type == SDL_WINDOWEVENT ){
			if( gEvento.window.event == SDL_WINDOWEVENT_RESIZED ){
				Juego_ActualizarVentana();
			}
		}
		else{
			if( estadoActual != nullptr )
				estadoActual -> estadoEventos( gEvento );
		}
	}

	// Llama a estado entrada
	if( estadoActual != nullptr ){
		estadoActual -> estadoEntrada();
	}
}

// Ejecuta la lógica del juego
void Juego_Logica( void ){
	// Reinica la tasa de fotogramas
	if( temporizadorFPS.obtenerTicks() >= 1000 ){
		if( mostrarFPS ){
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
void Juego_Renderizar( void ){
	// Limpia la pantalla
	SDL_SetRenderDrawColor( gRender, 0xFF, 0xFF, 0xFF, 0xFF );
	SDL_RenderClear( gRender );
		
	// Dibuja los estados del juego
	for( EstadoJuego *estado : estadosJuego ){
		if( estado != nullptr ){
			estado -> estadoRenderizado();
		}
	}

	// Dibuja la tasa de fotogramas por segundo
	if( mostrarFPS ){
		fpsTexto.renderTexture( fpsTexto.leerDimensionesTextura(), fpsTexto.leerDimensionesEspacio() );
	}

	/*SDL_Rect malla = { 0, 0, Objeto::leerMagnitudUnidad(), Objeto::leerMagnitudUnidad() };
	int ancho = floor( espacioAncho );
	int alto = floor( espacioAlto );
	for( size_t i = 0; i < ancho; i++ ){
		for( size_t j = 0; j < alto; j++ ){
			malla.x = malla.w * i;
			malla.y = malla.w * j;
			SDL_SetRenderDrawColor( gRender, 255, 255, 255, 255 );
			SDL_RenderDrawRect( gRender, &malla );
		}
	}*/

	// Actualiza la pantalla
	SDL_RenderPresent( gRender );

	// Incrementa el numero de fps
	contadorFPS++;

	// Retrasa un milisegundo
	SDL_Delay( 1 );
}

// Dibuja un fondo negro transparente
void Juego_FondoNegro( void ){
	SDL_Rect rect = { 0, 0, pantallaAncho, pantallaAlto }; 
	SDL_SetRenderDrawColor( gRender, 0x00, 0x00, 0x00, 0x88 );
    SDL_RenderFillRect( gRender, &rect );
}

// Actualiza la ventana
void Juego_ActualizarVentana( void ){
	// Obtiene el tamaño de la ventana
	SDL_GetWindowSize( gVentana, &pantallaAncho, &pantallaAlto );

	// Establece el nuevo tamaño de la ventana
	SDL_SetWindowSize( gVentana, pantallaAncho, pantallaAlto );
	
	// Actualiza el tamaño del área de renderizado
	SDL_RenderSetLogicalSize( gRender, pantallaAncho, pantallaAlto );

	// Establece en pantalla completa
	SDL_SetWindowFullscreen( gVentana, pantallaCompleta );

	// Actualiza el tamaño de la unidad y las dimensiones del viewport
	Objeto::actualizarMagnitudUnidad( pantallaAlto );
	espacioX = 0.f;
	espacioY = 0.f;
	espacioAlto = Objeto::UNIDAD_PANTALLA;
	espacioAncho = pantallaAncho / Objeto::leerMagnitudUnidad();

	// Actualiza el tamaño de la fuente
	Fuente_ActualizarTamanio( fuenteTexto );

	// Si hay un estado de juego establecido
	for( EstadoJuego *estadoJuego : estadosJuego ){
		if( estadoJuego != nullptr ){
			estadoJuego -> actualizarViewport();
		}
	}
}

// Limpia los estados de juego actual
void Juego_LimpiarEstados( void ){
	// Elimina los estados del juego pendientes
	while( !estadosJuego.empty() ){
		// Obtiene el último estado registrado y lo elimina
		EstadoJuego *auxiliar = estadosJuego.back();
		if( auxiliar != nullptr ){
			delete auxiliar;
			auxiliar = nullptr;
		}

		// Lo elimina de la pila de estados
		estadosJuego.pop_back();
	}
}

// Limpia los estados pendientes por agregar
void Juego_LimpiarPendientes( void ){
	// Mientras existan estados pendientes
	while( !estadosPendientes.empty() ){
		// Obtiene el último estado pendiente y lo elimina
		Pendiente *auxiliar = estadosPendientes.back();
		if( auxiliar != nullptr ){
			delete auxiliar;
			auxiliar = nullptr;
		}

		// Retirar el apuntador de la cola
		estadosPendientes.pop_back();
	}
}

void Juego_EstablecerEstado( EstadoJuego *estado, EstadoAccion accion ){
	// Crea un pendiente
	Pendiente *pendiente = new Pendiente;
	if( pendiente == nullptr ){
		throw runtime_error( "Ocurrió un error al intentar crear un nuevo estado." );
	}

	// Indica el estado que se establecerá
	pendiente -> estado = estado;

	// Indica que el tipo de estado que se desea establecer
	pendiente -> accion = accion;

	// Lo agrega a la cola
	estadosPendientes.push_back( pendiente );
}

void Juego_ActualizarEstado( void ){
	// Mientras existan estados pendientes
	while( !estadosPendientes.empty() ){
		// Obtiene un estado pendiente
		Pendiente * pendiente = estadosPendientes.front();
		if( pendiente != nullptr ){
			// Selecciona la opción más conveniente
			switch( pendiente -> accion ){
				case ESTADO_APILAR:
					cout << "apilar" << endl;
					estadosJuego.push_back( pendiente -> estado );
					estadoActual = estadosJuego.back();
					break;
				case ESTADO_ESTABLECER:
					cout << "establecer" << endl;
					Juego_LimpiarEstados();
					estadosJuego.push_back( pendiente -> estado );
					estadoActual = estadosJuego.back();
					break;
				case ESTADO_FINALIZAR:
					cout << "finalizar" << endl;
					if( !estadosJuego.empty() ){
						EstadoJuego *auxiliar = estadosJuego.back();
						if( auxiliar != nullptr ){
							delete auxiliar;
							auxiliar = nullptr;
						}
						estadosJuego.pop_back();
						estadoActual = estadosJuego.back();
					}
					break;
				case ESTADO_SALIR:
					cout << "salir" << endl;
					salir = true;
					break;
				default:
					break;
			}

			// Elimina el pendiente
			delete pendiente;
			estadosPendientes.pop_front();
			pendiente = nullptr;
		}
	}
}

// Indica que hay que slair
void Juego_salir( bool salir ){
	salir = salir;
}

// Para manejar las variables String
void Juego_CargarValoresMensaje( string archivo ){
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
			valoresMensaje.insert( pair< string, string >( (* elemento)[ "nombre" ], (* elemento)[ "valor" ] ) );
		}
	}
}

void Juego_CargarMedia( void ){	
	try{
		objFondoInicio.leerObjetoDesdeArchivo( "../recursos/imagenes/Intro Screen.png" );
		objFondo.leerObjetoDesdeArchivo( "../recursos/imagenes/fondos/lasers.png" ); 	// Fondo
		objTablero.escribirDimensionesEspacio( 0.f, 0.f, 4.178, 8.3 );
		objMargen.leerObjetoDesdeArchivo( "../recursos/imagenes/1080p/UI/HDUISP01.png" );	// Margen
		objBloque.leerObjetoDesdeArchivo( "../recursos/imagenes/bloques/bloques.png" );	// Bloque
		objBloque.escribirDimensionesEspacio( 0, 0, objTablero.leerEspacioAncho() / (float)BOARD_WIDTH, objTablero.leerEspacioAncho() / (float)BOARD_WIDTH );
		objFiguras.leerObjetoDesdeArchivo( "../recursos/imagenes/bloques/figuras.png" ); // Cola de figuras
		objFiguras.escribirDimensionesEspacio( 0, 0, ( 236.f * 10 ) / 1080, ( 141.f * 10 ) / 1080 );
		objYa.leerObjetoDesdeArchivo( "../recursos/imagenes/texto/ya.png" ); // Textura objYa
		objCuentaRegresiva.leerObjetoDesdeArchivo( "../recursos/imagenes/texto/cuenta.png" ); // Cuenta regresiva
    	objCuentaRegresiva.escribirDimensionesTextura( 0, 0, 82, 157 );
    	objCuentaRegresiva.escribirEspacioY( 1 );
	}
	catch( invalid_argument &ia ){
		cout << ia.what() << endl;
		salir = true;
	}
} 

void Fuente_ActualizarTamanio( Fuente &fuente )
{
	// Fuente
	TTF_CloseFont( fuente.fuente );
	fuente.fuente = TTF_OpenFont( fuente.archivo.c_str(), ( ( (float)pantallaAlto / 1080.f ) * (float)fuente.magnitud ) );
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
	objeto.escribirDimensionesEspacio( 0, 0, (float)objeto.getWidth() / Objeto::leerMagnitudUnidad(), (float)objeto.getHeight() / Objeto::leerMagnitudUnidad() );
}

void Fuente_ActualizarTexto( string texto, Fuente &fuente, Objeto &objeto, double x, double y ){
	// Fuente
	SDL_Color color = { 255, 255, 255 };
	if( !objeto.crearTexturaDesdeTextoSolido( texto.c_str(), color, fuente.fuente ) ){
		throw runtime_error( "Ocurrió un error al crear la textura de texto." );
	}

	// Establece las dimensiones de textura y de espacio
	objeto.escribirDimensionesTextura( 0, 0, objeto.getWidth(), objeto.getHeight() );
	objeto.escribirDimensionesEspacio( x, y, (float)objeto.getWidth() / Objeto::leerMagnitudUnidad(), (float)objeto.getHeight() / Objeto::leerMagnitudUnidad() );
}