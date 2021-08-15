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
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 || (IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG) != IMG_INIT_PNG || TTF_Init() < 0 ){
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
	SDL_Surface * icono = IMG_Load("../recursos/img/icono.bmp");
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
	fuenteTexto.archivo = "../recursos/fuentes/Aaargh.ttf";
	fuenteTexto.magnitud = 47;

	// Actualiza la ventana
	Juego_ActualizarVentana();
}

// libera los recursos del juego
void Juego_Cerrar( void ){
	while( !estadosPendientes.empty() ){
		Pendiente *aux = estadosPendientes.back();
		delete aux;
		estadosPendientes.pop_back();
	}

	while( !estadosJuego.empty() ){
		EstadoJuego *aux = estadosJuego.back();
		delete aux;
		estadosPendientes.pop_back();
	}

	// Deletes everything of the game
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
		}
		else if( gEvento.type == SDL_WINDOWEVENT ){
			if( gEvento.window.event == SDL_WINDOWEVENT_RESIZED ){
				Juego_ActualizarVentana();
			}
		}

		// Llama a estado eventos
		if( estadoActual != nullptr ){
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

// Finaliza el estado actual que se está ejecutando
void Juego_FinalizarEstado( void ){
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
void Juego_ApilarEstado( EstadoJuego *estado ){
	// Verifica que el estado que se desea apilar no es nulo
	if( estado == nullptr ){
		throw invalid_argument( valoresMensaje[ "apilacion_elemento_nulo" ] );
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
void Juego_EstablecerEstado( EstadoJuego *estado ){
	// Verifica que no se desea establecer un estado vacío
	if( estado == nullptr ){
		throw invalid_argument( valoresMensaje[ "establecimiento_elemento_nulo" ] );
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
void Juego_ActualizarEstado( void ){
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
			estadosJuego.push_back( pendiente -> estado );
		}
		// Establece el estado si así se indica
		else if( pendiente -> accion == ESTADO_ESTABLECER ){
			// Mientras haya estados en la pila
			while( !estadosJuego.empty() ){
				// Retira el estado de la pila
				estadoActual = estadosJuego.back();
				delete estadoActual;
				estadosJuego.pop_back();
			}

			// Apila el nuevo estado
			estadosJuego.push_back( pendiente -> estado );
		}
		// Retira el estado actual si desea finalizar
		else if( pendiente -> accion == ESTADO_FINALIZAR ){
			// Si la pila no esta vacía
			if( !estadosJuego.empty() ){
				// Lo retira de la pila de estados
				estadoActual = estadosJuego.back();
				delete estadoActual;
				estadosJuego.pop_back();
			}
		}

		// Elimina el pendiente
		delete pendiente;
		estadosPendientes.pop_front();
		pendiente = nullptr;
	}

	// Si la pila de estados se encuenta vacía
	if( estadosJuego.empty() ){
		estadoActual = nullptr;
		salir = true;
		return;
	}

	// Establece el estado actual
	estadoActual = estadosJuego.back();
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
		objFondo.leerObjetoDesdeArchivo( "../recursos/imagenes/fondos/Space.png" ); 	// Fondo
		objTablero.escribirDimensionesEspacio( 0.f, 0.2214, 2.73, 5.73 );				// Tamaño del tablero de la superficie
		objMargen.leerObjetoDesdeArchivo( "../recursos/imagenes/bloques/margen.png" );	// Margen
		objBloque.leerObjetoDesdeArchivo( "../recursos/imagenes/bloques/bloque.png" );	// Bloque
		objBloque.escribirDimensionesEspacio( 0, 0, objTablero.leerEspacioAncho() / (float)BOARD_WIDTH, objTablero.leerEspacioAncho() / (float)BOARD_WIDTH );
		objFiguras.leerObjetoDesdeArchivo( "../recursos/imagenes/bloques/figuras.png" ); // Cola de figuras
		objFiguras.escribirDimensionesEspacio( 0, 0, ( 236.f * 6.13 ) / 1080, ( 141.f * 6.13 ) / 1080 );
		objYa.leerObjetoDesdeArchivo( "../recursos/imagenes/texto/ya.png" ); // Textura objYa
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