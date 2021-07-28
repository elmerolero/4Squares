#include "foursquares.h"
#include "componentes.h"
#include "pausa.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <vector>
#include <ctime>
using namespace std; 

FourSquares::FourSquares()
{
	// Carga los elementos del juego
	FS_CargarElementos();

	// Inicializa la cola de figuras
	Cola_Inicializar( colaFiguras );
	
	// Establece una nueva pieza
	Pieza_NuevaPieza( piezaJugador, rand() % 7, tetroBoard );

	// Inicializa el tablero
	Tablero_Inicializar( tetroBoard );

	// Indicadores del juego
	pieceSaved = 0;
	contadorNivel = 1;
	contadorLineas = 0;
	contadorCombo = 0;
	comboMaximo = 0;
	contadorPuntaje = 0;

	// Initializa los temporizadores
	tiempoPartida.iniciar();
	nivelRespuestaLaterales = 0;
	arribaPresionado = false;
	allowedChange = true;

	// Actualiza el viewport
	actualizarViewport();
}
 
FourSquares::~FourSquares()
{
	tetroBlock.destroyTexture();
	tetroShapes.destroyTexture();
	tetroMargin.destroyTexture();
	tetroBackground.destroyTexture();
}

void FourSquares::estadoEventos( SDL_Event &gGameEvent ){
	if( gGameEvent.type == SDL_KEYDOWN && !arribaPresionado ){
		if( gGameEvent.key.keysym.sym == SDLK_UP ){
			arribaPresionado = true;
		}
		else if( gGameEvent.key.keysym.sym == SDLK_RETURN ){
			FS_PausarPartida();
			fourSquares.apilarEstado( new Pausa() );
		}
		else if( gGameEvent.key.keysym.sym == SDLK_x && piezaJugador.tipo != FIGURA_CUADRADO ){
			Pieza_Alternar( piezaJugador, tetroBoard, 1 );
			if( gameTimer.obtenerTicks() >= downSpeed[ contadorNivel - 1 ] ){
				pasosRealizados++;
				tiempoAdicional.reiniciar();
			}
		}
		else if( gGameEvent.key.keysym.sym == SDLK_z && piezaJugador.tipo != FIGURA_CUADRADO ){
			Pieza_Alternar( piezaJugador, tetroBoard, -1 );
			if( gameTimer.obtenerTicks() >= downSpeed[ contadorNivel - 1 ] ){
				pasosRealizados++;
				tiempoAdicional.reiniciar();
			}
		}
		else if( allowedChange && ( gGameEvent.key.keysym.sym == SDLK_LSHIFT || gGameEvent.key.keysym.sym == SDLK_RSHIFT ) ){
			int pieza;
			
			// ¿Hay una pieza guardada?
			if( pieceSaved ){
				// Obtiene la pieza guardada
				pieza = pieceSaved - 1;
			}
			else{
				// Obtiene una nueva pieza de la cola
				pieza = Cola_ObtenerSiguenteFigura( colaFiguras );
			}

			// Guarda la pieza en juego
			pieceSaved = piezaJugador.tipo + 1;

			// Establece los nuevos datos de la pieza
			Pieza_NuevaPieza( piezaJugador, pieza, tetroBoard );

			// Establece las banderas y reinicia el tiempo
			allowedChange = false;
			pasosRealizados = 0;
			tiempoAdicional.reiniciar();
			tiempoAdicional.pausar();
		}
	}
	else if( gGameEvent.type == SDL_WINDOWEVENT ){
		if( gGameEvent.type == SDL_WINDOW_MINIMIZED ){

		}
	}
}

void FourSquares::estadoEntrada(){
	if( arribaPresionado ){
		return;
	}
			
	const Uint8 *keyboardState = SDL_GetKeyboardState( NULL );

	// Down
	if( keyboardState[ SDL_SCANCODE_DOWN ] && tiempoEntradaBajada.obtenerTicks() >= 50 ){
		if( Tablero_PermiteMover( tetroBoard, piezaJugador, piezaJugador.figura.x, piezaJugador.figura.y + 1 ) ){
			piezaJugador.figura.y++;
		}
		
		tiempoEntradaBajada.reiniciar();
	}
	
	if( keyboardState[ SDL_SCANCODE_LEFT ] ){
		if( tiempoEntradaLaterales.obtenerTicks() >= moveResponseTime[ nivelRespuestaLaterales ] ){
			if( Tablero_PermiteMover( tetroBoard, piezaJugador, piezaJugador.figura.x - 1, piezaJugador.figura.y ) ){
				piezaJugador.figura.x--;
			}

			tiempoEntradaLaterales.reiniciar();
			if( gameTimer.obtenerTicks() >= downSpeed[ contadorNivel - 1 ] ){
				pasosRealizados++;
				tiempoAdicional.reiniciar();
			}

			if( nivelRespuestaLaterales < 2 ){
				nivelRespuestaLaterales++;
			}
		}
		Pieza_ActualizarSombra( piezaJugador, tetroBoard );
	}
	else if( keyboardState[ SDL_SCANCODE_RIGHT ] ){
		if( tiempoEntradaLaterales.obtenerTicks() >= moveResponseTime[ nivelRespuestaLaterales ] ){
			if( Tablero_PermiteMover( tetroBoard, piezaJugador, piezaJugador.figura.x + 1, piezaJugador.figura.y ) ){
				piezaJugador.figura.x++;
			}

			tiempoEntradaLaterales.reiniciar();
			if( gameTimer.obtenerTicks() >= downSpeed[ contadorNivel - 1 ] ){
				pasosRealizados++;
				tiempoAdicional.reiniciar();
			}

			if( nivelRespuestaLaterales < 2 ){
				nivelRespuestaLaterales++;
			}
			Pieza_ActualizarSombra( piezaJugador, tetroBoard );
		}
	}
	else{
		nivelRespuestaLaterales = 0;
	}
}

void FourSquares::estadoLogica()
{ 	
	if( tiempoPartida.obtenerTicks() > 2000 ){
		ya.show( false );
	}

	// Revisa si es momento de bajar la pieza
	if( ( gameTimer.obtenerTicks() >= downSpeed[ contadorNivel - 1 ] ) || arribaPresionado ){
		// Reinicia el tiempo
		gameTimer.reiniciar();

		// ¿No hay colisión en el siguiente paso?
		if( Tablero_PermiteMover( tetroBoard, piezaJugador, piezaJugador.figura.x, piezaJugador.figura.y + 1 ) ){
			piezaJugador.figura.y++;
			return;
		}

		// Brinda tiempo adicional para acomodar la pieza (útil para niveles rápidos)
		tiempoAdicional.reanudar();
		if( tiempoAdicional.obtenerTicks() < 750 && pasosRealizados < PASOS_MAXIMOS && !arribaPresionado ){
			return;
		}

		// Guarda la pieza en el tablero dado
		Pieza_Grabar( piezaJugador, tetroBoard );

		// El jugador está en una posición 
		if( piezaJugador.figura.y == 1 ){
			tiempoPartida.pausar();
			gameTimer.pausar();
			tiempoEntradaBajada.pausar();
			tiempoEntradaLaterales.pausar();
			fourSquares.apilarEstado( new Derrota() );
			return;
		}
		
		Tablero_ObtenerLineas( tetroBoard, lineasJugador );
		Tablero_EliminarLineas( tetroBoard, lineasJugador );
		Tablero_Acomodar( tetroBoard, lineasJugador );
		
		contadorCombo++;
		if( contadorCombo > comboMaximo ){
			comboMaximo = contadorCombo;
		}
		
		if( lineasJugador.empty() ){
			contadorCombo = 0;
		}

		FS_ActualizarLineas( contadorLineas, lineasJugador, lineas );
		FS_ActualizarNivel( contadorNivel, contadorLineas, nivel );
		FS_ActualizarPuntaje( contadorPuntaje, lineasJugador, contadorCombo, puntaje );

		Pieza_NuevaPieza( piezaJugador, Cola_ObtenerSiguenteFigura( colaFiguras ), tetroBoard );
		allowedChange = true;
		arribaPresionado = false;
		pasosRealizados = 0;
		tiempoAdicional.reiniciar();
		tiempoAdicional.pausar();
	}
}

void FourSquares::estadoRenderizado()
{	
	// Dibuja el fondo
	tetroBackground.renderTexture( tetroBackground.leerDimensionesTextura(), tetroBackground.leerDimensionesEspaciales() );
	tetroMargin.renderTexture( tetroMargin.leerDimensionesTextura(), tetroMargin.leerDimensionesEspaciales() );
	
	// Dibuja el tablero
	Tablero_Dibujar( tetroBoard );
	
	// Dibuja la sombra de la pieza
	Pieza_Dibujar( piezaJugador, piezaJugador.sombra.x, piezaJugador.sombra.y, shadowColor[ piezaJugador.tipo ] );
	
	// Dibuja la pieza
	Pieza_Dibujar( piezaJugador, piezaJugador.figura.x, piezaJugador.figura.y, shapeColor[ piezaJugador.tipo ] );

	// Dibuja la cola de figuras
	Cola_Dibujar( colaFiguras );

	// Dibuja la pieza guardada
	FS_DibujarFigura( pieceSaved - 1, ( tetroBoardSurface.leerEspacialX() - 1.7f ), 0.7f );

	// Dibuja el puntaje
	puntaje.renderTexture( puntaje.leerDimensionesTextura(), puntaje.leerDimensionesEspaciales() );

	// Dibuja el nivel
	nivel.renderTexture( nivel.leerDimensionesTextura(), nivel.leerDimensionesEspaciales() );

	// Dibuja el numero de lineas
	lineas.renderTexture( lineas.leerDimensionesTextura(), lineas.leerDimensionesEspaciales() );

	// Dibuja el letrero de ¡Ya!
	ya.renderTexture( ya.leerDimensionesTextura(), ya.leerDimensionesEspaciales() );

	// Dibuja el tiempo
	FS_DibujarTiempo( tiempoPartida.obtenerTicks(), tiempo, fuenteTexto, tetroBoardSurface.leerEspacialX() + tetroBoardSurface.leerEspacialAncho() + 0.6, 5.43f );
	tiempo.renderTexture( tiempo.leerDimensionesTextura(), tiempo.leerDimensionesEspaciales() );
}

/* FUNCTIONS */
// Updates game's viewport if window is resized
void FourSquares::actualizarViewport()
{	
	// Board surface
	tetroBoardSurface.escribirEspacialX( ( fourSquares.leerEspacioAncho() - tetroBoardSurface.leerEspacialAncho() ) / 2  );
	tetroBoardSurface.escribirEspacialY( ( ( fourSquares.leerEspacioAlto() - tetroBoardSurface.leerEspacialAlto() ) / 2 ) - tetroBlock.leerEspacialAncho() );
	
	// Background
	tetroBackground.escribirTexturaW( ( ( (float)fourSquares.leerVistaAncho() / (float)fourSquares.leerVistaAlto() ) / Objeto::RELACION_ASPECTO ) * (float)tetroBackground.getWidth() );
	tetroBackground.escribirTexturaH( ( Objeto::RELACION_ASPECTO / ( (float)fourSquares.leerVistaAncho() / (float)fourSquares.leerVistaAlto()) ) * (float)tetroBackground.getHeight() );
	tetroBackground.escribirTexturaX( ( tetroBackground.getWidth() - tetroBackground.leerTexturaW() ) / 2 );
	tetroBackground.escribirTexturaY( ( tetroBackground.getHeight() - tetroBackground.leerTexturaH() ) / 2 );
	tetroBackground.escribirDimensionesEspaciales( fourSquares.leerEspacioX(), fourSquares.leerEspacioY(), fourSquares.leerEspacioAncho(), fourSquares.leerEspacioAlto() );
	
	// Margen
	tetroMargin.escribirEspacialX( ( fourSquares.leerEspacioAncho() - tetroMargin.leerEspacialAncho() ) / 2 );
	tetroMargin.actualizarDimensionesAbsolutas();

	// Figura
	gFigura.actualizarDimensionesAbsolutas();

	//
	tetroBlock.actualizarDimensionesAbsolutas();

	// 
	ya.escribirEspacialX( ( fourSquares.leerEspacioAncho() - ya.leerEspacialAncho() ) / 2 );

	// Texto renderizado
	FS_ActualizarPuntaje( contadorPuntaje, lineasJugador, contadorCombo, puntaje );
	FS_ActualizarNivel( contadorNivel, contadorLineas, nivel );
	FS_ActualizarLineas( contadorLineas, lineasJugador, lineas );
}

void FS_CargarElementos( void )
{
	SDL_DRect auxDRect;
	SDL_Rect auxRect;
	
	try{
		// Board surface
		tetroBoardSurface.leerDimensionesDesdeArchivo( "../recursos/coord/board.crd" );
		
		// Margin
		tetroMargin.loadFileTexture( "../recursos/img/bloques/margen.png" );
		tetroMargin.escribirDimensionesEspaciales( ( fourSquares.leerEspacioAncho() - tetroMargin.leerEspacialAncho() ) / 2, 0, ( (float)tetroMargin.getWidth() * 6.13 ) / 1080, ( (float)tetroMargin.getHeight() * 6.13 ) / 1080 );
		tetroMargin.escribirDimensionesTextura( 0, 0, tetroMargin.getWidth(), tetroMargin.getHeight() );

		// Background
		tetroBackground.loadFileTexture( "../recursos/img/fondos/Space.png" );
		
		// Blocks
		tetroBlock.loadFileTexture( "../recursos/img/bloques/bloque.png" );
		tetroBlock.leerDimensionesDesdeArchivo( "../recursos/coord/block.crd" );
		
		// Queue shapes
		tetroShapes.loadFileTexture( "../recursos/img/bloques/figuras.png" );

		// Shapes rect (black rects for the shapes queue	
		gFigura.escribirDimensionesEspaciales( 0, 0, ( 236.f * 6.13 ) / 1080, ( 141.f * 6.13 ) / 1080 );

		// Textura ya
		ya.loadFileTexture( "../recursos/img/texto/ya.png" );
		ya.escribirDimensionesTextura( 0, 0, ya.getWidth(), ya.getHeight() );
		ya.escribirDimensionesEspaciales( ( fourSquares.leerEspacioAncho() - ya.leerEspacialAncho() ) / 2, 1, ( (float)ya.leerTexturaW() * 6.13 ) / 1080, ( (float)ya.leerTexturaH() * 6.13 ) / 1080 );
	}
	catch( invalid_argument &ia ){
		cout << ia.what() << endl;
		fourSquares.salir( true );
	}

	/*FS_ActualizarTamanioFuente( fuenteArg, "../recursos/fuentes/Aaargh.ttf", 47.f );
	FS_ActualizarDatos( fps, fpsTextura, fpsObjeto, 2, fuenteArg, 0.f, 0.f );*/
}

void Pieza_NuevaPieza( Pieza &pieza, int figura, int tablero[ 21 ][ 10 ] ){
	// Establece el tipo de pieza
	pieza.tipo = figura;

	// Establece los bloques correspondientes
	for( int i = 0; i < 4; i++ ){
		pieza.bloques[ i ].x = RELATIVE_POSITIONS[ pieza.tipo ][ i ][ 0 ];
		pieza.bloques[ i ].y = RELATIVE_POSITIONS[ pieza.tipo ][ i ][ 1 ];
	}

	// Establece la posición inicial
	pieza.figura.x = INITIAL_POS_X;
	pieza.figura.y = INITIAL_POS_Y;

	// Actualiza la posicion de la sombra
	Pieza_ActualizarSombra( pieza, tablero );
}

void Pieza_ActualizarSombra( Pieza &pieza, int tablero[ 21 ][ 10 ] ){
	// Establece la posición de la sombra
	pieza.sombra.x = pieza.figura.x;
	pieza.sombra.y = pieza.figura.y;

	// Mientra permita moverse a lo largo del tablero dado
	while( Tablero_PermiteMover( tablero, pieza, pieza.sombra.x, pieza.sombra.y + 1 ) ){
		pieza.sombra.y++;
	}
}

void Pieza_Rotar( Pieza &pieza, int direccion ){
	// Saves the new positions
    int piezaRotada[ 4 ][ 2 ];

    for(int i = 0; i < 4; i++){
        // NX = ( VX * cos( 90 ) ) - ( VY * sin( 90 ) )
        piezaRotada[ i ][ 0 ] = -( pieza.bloques[ i ].y * direccion );

        // NX = ( VX * sen( 90 ) ) - ( VY * cos( 90 ) )
        piezaRotada[ i ][ 1 ] = ( pieza.bloques[ i ].x * direccion );
    }

    // Assignes the new positions to the piece
    for( int i = 0; i < 4; i++ ){
        pieza.bloques[ i ].x = piezaRotada[ i ][ 0 ];
		pieza.bloques[ i ].y = piezaRotada[ i ][ 1 ];
	}
}

void Pieza_Alternar( Pieza &pieza, int tablero[ 21 ][ 10 ], int direccion  ){
	// Rota la figura
	Pieza_Rotar( pieza, direccion );

	if( pieza.tipo != FIGURA_LINEA ){
		if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x, pieza.figura.y ) ){
			Pieza_ActualizarSombra( pieza, tablero );
			return;
		}
		// Puede moverse si se mueve a la derecha
		else if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x + 1, pieza.figura.y ) ){
			pieza.figura.x++;
		}
		// Puede moverse si se mueve hacia la izquierda
		else if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x - 1, pieza.figura.y ) ){
			pieza.figura.x--;
		}
		// Puede moverse si se sube
		else if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x, pieza.figura.y - 1 ) ){
			pieza.figura.y--;
		}
		else{
			Pieza_Rotar( pieza, direccion * -1 );
		}
	}
	else{
		if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x, pieza.figura.y ) ){
			Pieza_ActualizarSombra( pieza, tablero );
			return;
		}
		// Puede moverse si se mueve a la derecha
		else if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x + 1, pieza.figura.y ) ){
			pieza.figura.x++;
		}
		else if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x + 2, pieza.figura.y ) ){
			pieza.figura.x += 2;
		}
		// Puede moverse si se mueve hacia la izquierda
		else if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x - 1, pieza.figura.y ) ){
			pieza.figura.x--;
		}
		else if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x - 2, pieza.figura.y ) ){
			pieza.figura.x -= 2;
		}
		// Puede moverse si se sube
		else if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x, pieza.figura.y - 1 ) ){
			pieza.figura.y--;
		}
		else if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x, pieza.figura.y - 2 ) ){
			pieza.figura.y -= 2;
		}
		// Puede moverse si se baja
		else if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x, pieza.figura.y + 1 ) ){
			pieza.figura.y++;
		}
		else{
			Pieza_Rotar( pieza, direccion * -1 );
		}
	}

	Pieza_ActualizarSombra( pieza, tablero );
}

void Pieza_Grabar( Pieza &pieza, int tablero[ BOARD_HEIGHT ][ BOARD_WIDTH ] ){
	for( int i = 0; i < 4; i++ ){
		int posX = pieza.figura.x + pieza.bloques[ i ].x;
		int posY = pieza.figura.y + pieza.bloques[ i ].y;
		
        tablero[ posY ][ posX ] = pieza.tipo + 1;
	}
}

// Dibuja la pieza
void Pieza_Dibujar( Pieza &pieza, int posicionX, int posicionY, SDL_Color color )
{
	SDL_Rect auxRect = { 0, 0, tetroBlock.leerAbsolutoAncho(), tetroBlock.leerAbsolutoAncho() };
	int tempX = tetroBoardSurface.leerAbsolutoX();
	int tempY = tetroBoardSurface.leerAbsolutoY();
	
	// Establece el color
	tetroBlock.setColorMod( color );

	// Dibuja la figura
	for( int i = 0; i < 4; i++ ){
		auxRect.x = tempX + ( ( posicionX + pieza.bloques[ i ].x ) * auxRect.w );
		auxRect.y = tempY + ( ( posicionY + pieza.bloques[ i ].y ) * auxRect.w );
		
		tetroBlock.renderTexture( tetroBlock.leerDimensionesTextura(), &auxRect );
	}
}

// Inicializa el tablero en ceros
void Tablero_Inicializar( int tablero[ BOARD_HEIGHT ][ BOARD_WIDTH ] ){
	for( size_t renglones = 0; renglones < BOARD_HEIGHT; renglones++ ){
		for( int columnas = 0; columnas < BOARD_WIDTH; columnas++ ){
			tetroBoard[ renglones ][ columnas ] = 0;
		}
	}
}

// Indica si dentro del tablero dado se puede mover una pieza dada
bool Tablero_PermiteMover( int tablero[ BOARD_HEIGHT ][ BOARD_WIDTH ], Pieza &pieza, int posicionX, int posicionY )
{
	for( int i = 0; i < 4; i++ ){
		int posX = posicionX + pieza.bloques[ i ].x;
		int posY = posicionY + pieza.bloques[ i ].y;
		if( posX >= BOARD_WIDTH || posY >= BOARD_HEIGHT || posX < 0 || posY < 0 ||
			Tablero_CasillaUtilizada( tablero, posX, posY ) ){
			return false;
		}
	}
	
	return true;
}

// Indica si la casilla indicada ya contiene un bloque
bool Tablero_CasillaUtilizada( int tablero[ BOARD_HEIGHT ][ BOARD_WIDTH ], int posicionX, int posicionY ){
	return tablero[ posicionY ][ posicionX ] != 0;
}

// Obtiene el número de líneas realizadas en un tablero dado
void Tablero_ObtenerLineas( int tablero[ BOARD_HEIGHT ][ BOARD_WIDTH ], vector< int > &lineas ){
	// Contador de casillas llenas por renglon
	int casillasLlenas = 0;

	// Para cada renglón
	for( size_t renglon = 0; renglon < BOARD_HEIGHT; ++renglon ){
		// Revisar que la casilla de cada columna
		for( size_t columna = 0; columna < BOARD_WIDTH; ++columna ){
			// Si contiene un número distinto de cero (está ocupada)
			if( tablero[ renglon ][ columna ] > 0 ){
				// Se incrementa el número de casillas llenas para ese renglón
				casillasLlenas++;
			}
		}
		
		// Si el renglón tiene llena todas las casillas, entonces se agregar al vector de líneas realizadas
		if( casillasLlenas == BOARD_WIDTH ){
			lineas.push_back( renglon );
		}

		// Reinicia la variable de casillas llenas
		casillasLlenas = 0;
	}
}

// Elimina las líneas dadas
void Tablero_EliminarLineas( int tablero[ BOARD_HEIGHT ][ BOARD_WIDTH ], vector< int > &lineas )
{
	for( size_t columnas = 0; columnas < BOARD_WIDTH; ++columnas ){
		for( size_t renglones = 0; renglones < lineas.size(); ++renglones ){
			tablero[ lineas[ renglones ] ][ columnas ] = 0;
		}

		fourSquares.renderizar();
		SDL_Delay( 20 );
	}
}


void Tablero_Acomodar( int tablero[ BOARD_HEIGHT ][ BOARD_WIDTH ], vector< int > &lineas )
{
	for( int linea : lineas ){
		// A partir de esa linea copiamos el contenido de las casillas hacia arriba
		for( size_t i = linea; i > 0; --i ){
			for( size_t j = 0; j < BOARD_WIDTH; ++j ){
				tablero[ i ][ j ] = tablero[ i - 1 ][ j ];
				tablero[ i - 1 ][ j ] = 0;
			}
		}
	}
}

// Dibuja el tablero
void Tablero_Dibujar( int tablero[ BOARD_HEIGHT ][ BOARD_WIDTH ] )
{
	SDL_Rect auxRect = { 0, 0, tetroBlock.leerAbsolutoAncho(), tetroBlock.leerAbsolutoAlto() };
	int tempX = tetroBoardSurface.leerAbsolutoX();
	int tempY = tetroBoardSurface.leerAbsolutoY();

	for( int renglones = 0; renglones < BOARD_HEIGHT; renglones++ ){
		for( int columnas = 0; columnas < BOARD_WIDTH; columnas++ ){
			if( tablero[ renglones ][ columnas ] ){
				auxRect.x = tempX + ( auxRect.w * columnas );
				auxRect.y = tempY + ( auxRect.w * renglones );
				tetroBlock.setColorMod( shapeColor[ tetroBoard[ renglones ][ columnas ] - 1 ] );
				tetroBlock.renderTexture( tetroBlock.leerDimensionesTextura(), &auxRect );
			}
		}
	}
}

void Tablero_EstablecerColorRenglon( int tablero[ BOARD_HEIGHT ][ BOARD_WIDTH ], int renglon, int color ){
	for( int columna = 0; columna < BOARD_WIDTH; ++columna ){
		if( tablero[ renglon ][ columna ] ){
			tablero[ renglon ][ columna ] = color + 1;
		}
	}
}

// Inicializa la cola de figuras dada
void Cola_Inicializar( int colaFiguras[ 4 ] ){
	for( size_t i = 0; i < 4; ++i ){
		colaFiguras[ i ] = rand() % 7;
	}
}

// Obtiene la figura de una cola dada
int Cola_ObtenerSiguenteFigura( int colaFiguras[ 4 ] ){
	// Lee la figura a devolver
	int figura = colaFiguras[ 0 ];

	// Recorre las piezas
	for( size_t contador = 1; contador < 4; ++contador ){
		colaFiguras[ contador - 1 ] = colaFiguras[ contador ];
	}

	// Define la nueva figura
	colaFiguras[ 3 ] = rand() % 7;

	return figura;
}

// Dibuja la cola de figuras
void Cola_Dibujar( int colaFiguras[ 4 ] ){
	for( int contador = 0; contador < 4; ++contador ){
		int figura = colaFiguras[ contador ];
		FS_DibujarFigura( figura,  tetroBoardSurface.leerEspacialX() + tetroBoardSurface.leerEspacialAncho() + 0.3, 0.68 + ( gFigura.leerEspacialAlto() * contador ) + (0.12 * contador ) );
	}
}

void FS_DibujarFigura( int figura, double x, double y ){
	gFigura.escribirEspacialX( x );
	gFigura.escribirEspacialY( y );
	tetroShapes.setColorMod( shapeColor[ figura ] );
	tetroShapes.renderTexture( &shapeRects[ figura ], gFigura.leerDimensionesEspaciales() ); 
}

void FS_ActualizarLineas( int &lineasJugador, vector< int > &lineasRealizadas, Objeto &objeto ){
	// Suma la cantidad de líneas realizadas
	lineasJugador += lineasRealizadas.size();

	// Actualiza la textura con el nuevo número de líneas
	Fuente_ActualizarTexto( to_string( lineasJugador ), fuenteTexto, objeto );
	objeto.escribirEspacialX( tetroMargin.leerEspacialX() + 0.75f );
	objeto.escribirEspacialY( 5.4f );
}

void FS_ActualizarNivel( int &nivelJugador, int &lineasJugador, Objeto &objeto ){
	// Determina el nivel del jugador 
	nivelJugador = ( lineasJugador / 10 ) + 1;
	if( nivelJugador > 15 ){
		nivelJugador = 15;
	}

	Fuente_ActualizarTexto( to_string( nivelJugador ), fuenteTexto, objeto );
	objeto.escribirEspacialX( tetroMargin.leerEspacialX() + 0.75f );
	objeto.escribirEspacialY( 4.4f );
}

void FS_ActualizarPuntaje( int &puntaje, vector< int > &lineas, int &combo, Objeto &objeto )
{
	// Obtiene el nuevo puntaje
	int nuevoPuntaje = ( lineas.empty() ? 0 : 50 );

	// Actualiza el puntaje
	while( !lineas.empty() ){
		nuevoPuntaje = nuevoPuntaje * lineas.size();
		lineas.pop_back();
	}
	
	// Multiplica el puntaje de acuerdo al combo
	puntaje += nuevoPuntaje * combo;
	
	// Actualiza la textura del puntaje
	Fuente_ActualizarTexto( to_string( puntaje ), fuenteTexto, objeto );
	objeto.escribirEspacialX( tetroMargin.leerEspacialX() + 0.227 );
	objeto.escribirEspacialY( 2.33 );
}

void FS_DibujarTiempo( Uint32 tiempo, Objeto &objeto, Fuente &fuente, double x, double y )
{
	stringstream tiempoStr;
	tiempoStr << (tiempo / 60000) % 60 << ":" << std::setfill('0') << std::setw(2) << (tiempo / 1000) % 60 << ":" << (tiempo % 1000) / 10;

	// Fuente
	SDL_Color color = { 255, 255, 255 };
	if( !objeto.crearTexturaDesdeTextoSolido( tiempoStr.str().c_str(), color, fuente.fuente ) ){
		fourSquares.finalizarEstado();
		return;
	}

	objeto.escribirDimensionesTextura( 0, 0, objeto.getWidth(), objeto.getHeight() );
	objeto.escribirDimensionesEspaciales( x, y, (float)objeto.getWidth() / Objeto::leerMagnitudUnidad(), (float)objeto.getHeight() / Objeto::leerMagnitudUnidad() );
}

void FS_PausarPartida(){
	// Pausa el tiempo
	gameTimer.pausar();
	tiempoPartida.pausar();
	tiempoAdicional.pausar();
	tiempoEntradaBajada.pausar();
	tiempoEntradaLaterales.pausar();

	// Evita que se muestren algunas cosas
	tetroBlock.show( false );
}

void FS_ReanudarPartida( void ){
	// Reanuda el tiempo
	tiempoPartida.reanudar();
	tiempoAdicional.reanudar();
	tiempoEntradaBajada.reanudar();
	tiempoEntradaLaterales.reanudar();
	gameTimer.reanudar();

	// Vuelve a mostrar lo que se había ocultado
	tetroBlock.show( true );
}

/* VARIABLES */
// Tiempo del juego
Temporizador tiempoPartida;

// Game's timer
Temporizador tiempoAdicional;
Temporizador gameTimer;

// Input timer
int nivelRespuestaLaterales;
Temporizador tiempoEntradaBajada;
Temporizador tiempoEntradaLaterales;

//
Temporizador animationTimer;

//
vector< int > lineasJugador;

// Game's variables
int tetroBoard[ BOARD_HEIGHT ][ BOARD_WIDTH ];

// Pieza
Pieza piezaJugador;
int piezaCodigo;
int pieza[ 4 ][ 2 ];
int piecePosX;
int piecePosY;

// Sombra
int sombraPosX;
int sombraPosY;

// Queue
int colaFiguras[ 4 ];

//
bool allowedChange;
int pieceSaved;
int pasosRealizados = 0;
bool arribaPresionado = false;

// Puntaje
int contadorCombo;
int comboMaximo;
int contadorLineas;
int contadorNivel;
int contadorPuntaje;


Objeto tetroBackground; // Fondo
Objeto tetroMargin; // Margin
Objeto tetroBoardSurface; // Board surface
Objeto tetroBlock; // Blocks attributes
Objeto gFigura; // Shapes attributes
Objeto puntaje;
Objeto nivel;
Objeto lineas;
Objeto tiempo;
Objeto ya;
Objeto tetroShapes;
