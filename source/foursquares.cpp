#include "foursquares.h"
#include "globales.h"
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
	// Inicializa la cola de figuras
	Cola_Inicializar( colaFiguras );
	
	// Establece una nueva pieza
	Pieza_NuevaPieza( piezaJugador, rand() % 7, tablero );

	// Inicializa el tablero
	Tablero_Inicializar( tablero );

	// Indicadores del juego
	pieceSaved = 0;
	contadorNivel = 1;
	contadorLineas = 0;
	contadorCombo = 0;
	comboMaximo = 0;
	contadorPuntaje = 0;
	columna = 0;

	// Initializa los temporizadores
	tiempoPartida.iniciar();
	nivelRespuestaLaterales = 0;
	arribaPresionado = false;
	permitirCambio = true;

	// Actualiza el viewport
	actualizarViewport();
}
 
FourSquares::~FourSquares()
{
	objBloque.destroyTexture();
	objFiguras.destroyTexture();
	objMargen.destroyTexture();
	objFondo.destroyTexture();
}

void FourSquares::estadoEventos( SDL_Event &gGameEvent ){
	if( gGameEvent.type == SDL_KEYDOWN && !arribaPresionado ){
		if( gGameEvent.key.keysym.sym == SDLK_UP ){
			arribaPresionado = true;
		}
		else if( gGameEvent.key.keysym.sym == SDLK_RETURN ){
			FS_PausarPartida();
			Juego_ApilarEstado( new Pausa() );
		}
		else if( gGameEvent.key.keysym.sym == SDLK_x && piezaJugador.tipo != FIGURA_CUADRADO ){
			Pieza_Alternar( piezaJugador, tablero, 1 );
			if( indicadorTiempo.obtenerTicks() >= downSpeed[ contadorNivel - 1 ] ){
				pasosRealizados++;
				tiempoAdicional.reiniciar();
			}
		}
		else if( gGameEvent.key.keysym.sym == SDLK_z && piezaJugador.tipo != FIGURA_CUADRADO ){
			Pieza_Alternar( piezaJugador, tablero, -1 );
			if( indicadorTiempo.obtenerTicks() >= downSpeed[ contadorNivel - 1 ] ){
				pasosRealizados++;
				tiempoAdicional.reiniciar();
			}
		}
		else if( permitirCambio && ( gGameEvent.key.keysym.sym == SDLK_LSHIFT || gGameEvent.key.keysym.sym == SDLK_RSHIFT ) ){
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
			Pieza_NuevaPieza( piezaJugador, pieza, tablero );

			// Establece las banderas y reinicia el tiempo
			permitirCambio = false;
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
	// Si el botón arriba está presionado o hay líneas realizadas
	if( arribaPresionado || !lineasJugador.empty() ){
		return;
	}
	
	// Lee el estado del teclado
	const Uint8 *keyboardState = SDL_GetKeyboardState( NULL );

	// Down
	if( keyboardState[ SDL_SCANCODE_DOWN ] && tiempoEntradaBajada.obtenerTicks() >= 50 ){
		if( Tablero_PermiteMover( tablero, piezaJugador, piezaJugador.figura.x, piezaJugador.figura.y + 1 ) ){
			piezaJugador.figura.y++;
		}
		
		tiempoEntradaBajada.reiniciar();
	}
	
	if( keyboardState[ SDL_SCANCODE_LEFT ] ){
		if( tiempoEntradaLaterales.obtenerTicks() >= moveResponseTime[ nivelRespuestaLaterales ] ){
			if( Tablero_PermiteMover( tablero, piezaJugador, piezaJugador.figura.x - 1, piezaJugador.figura.y ) ){
				piezaJugador.figura.x--;
			}

			tiempoEntradaLaterales.reiniciar();
			if( indicadorTiempo.obtenerTicks() >= downSpeed[ contadorNivel - 1 ] ){
				pasosRealizados++;
				tiempoAdicional.reiniciar();
			}

			if( nivelRespuestaLaterales < 2 ){
				nivelRespuestaLaterales++;
			}
		}
		Pieza_ActualizarSombra( piezaJugador, tablero );
	}
	else if( keyboardState[ SDL_SCANCODE_RIGHT ] ){
		if( tiempoEntradaLaterales.obtenerTicks() >= moveResponseTime[ nivelRespuestaLaterales ] ){
			if( Tablero_PermiteMover( tablero, piezaJugador, piezaJugador.figura.x + 1, piezaJugador.figura.y ) ){
				piezaJugador.figura.x++;
			}

			tiempoEntradaLaterales.reiniciar();
			if( indicadorTiempo.obtenerTicks() >= downSpeed[ contadorNivel - 1 ] ){
				pasosRealizados++;
				tiempoAdicional.reiniciar();
			}

			if( nivelRespuestaLaterales < 2 ){
				nivelRespuestaLaterales++;
			}
			Pieza_ActualizarSombra( piezaJugador, tablero );
		}
	}
	else{
		nivelRespuestaLaterales = 0;
	}
}

void FourSquares::estadoLogica()
{ 	
	if( !lineasJugador.empty() ){
		// 
		if( tiempoAnimacion.obtenerTicks() > 20 ){
			// Si no ha eliminado todas las columnas
			if( columna < BOARD_WIDTH ){
				// Elimina los cuadros de la columna dada
				for( size_t renglones = 0; renglones < lineasJugador.size(); ++renglones ){
					tablero[ lineasJugador[ renglones ] ][ columna ] = 0;
				}

				// Incrementa el número de columna
				columna++;

				// Reinicia el tiempo
				tiempoAnimacion.reiniciar();

				// Regresa
				return;
			}

			// Acomoda los elementos del tablero
			Tablero_Acomodar( tablero, lineasJugador );

			// Actualiza la sombra
			Pieza_ActualizarSombra( piezaJugador, tablero );

			// Limpia las líneas del jugador
			lineasJugador.clear();

			// Reinicia
			columna = 0;
		}

		// Regresa
		return;
	}

	if( tiempoPartida.obtenerTicks() > 2000 ){
		objYa.show( false );
	}

	// Revisa si es momento de bajar la pieza
	if( ( indicadorTiempo.obtenerTicks() >= downSpeed[ contadorNivel - 1 ] ) || arribaPresionado ){
		// Reinicia el tiempo
		indicadorTiempo.reiniciar();

		// ¿No hay colisión en el siguiente paso?
		if( Tablero_PermiteMover( tablero, piezaJugador, piezaJugador.figura.x, piezaJugador.figura.y + 1 ) ){
			piezaJugador.figura.y++;
			return;
		}

		// Brinda tiempo adicional para acomodar la pieza (útil para niveles rápidos)
		tiempoAdicional.reanudar();
		if( tiempoAdicional.obtenerTicks() < 750 && pasosRealizados < PASOS_MAXIMOS && !arribaPresionado ){
			return;
		}

		// Guarda la pieza en el tablero dado
		Pieza_Grabar( piezaJugador, tablero );

		// El jugador está en una posición 
		if( piezaJugador.figura.y == 1 ){
			tiempoPartida.pausar();
			indicadorTiempo.pausar();
			tiempoEntradaBajada.pausar();
			tiempoEntradaLaterales.pausar();
			Juego_ApilarEstado( new Derrota() );
			return;
		}
		
		// Obtiene las lineas a borrar
		Tablero_ObtenerLineas( tablero, lineasJugador );

		contadorCombo++;
		if( contadorCombo > comboMaximo ){
			comboMaximo = contadorCombo;
		}
				
		if( lineasJugador.empty() ){
			contadorCombo = 0;
		}

		// Actualiza la información del juego
		FS_ActualizarLineas( contadorLineas, lineasJugador, objLineas );
		FS_ActualizarNivel( contadorNivel, contadorLineas, objNivel );
		FS_ActualizarPuntaje( contadorPuntaje, lineasJugador, contadorNivel, contadorCombo, objPuntaje );

		// Obtiene la nueva pieza
		Pieza_NuevaPieza( piezaJugador, Cola_ObtenerSiguenteFigura( colaFiguras ), tablero );
		permitirCambio = true;
		arribaPresionado = false;
		pasosRealizados = 0;
		tiempoAdicional.reiniciar();
		tiempoAdicional.pausar();
	}
}

void FourSquares::estadoRenderizado()
{	
	// Dibuja el fondo
	objFondo.renderTexture( objFondo.leerDimensionesTextura(), objFondo.leerDimensionesEspacio() );
	objMargen.renderTexture( objMargen.leerDimensionesTextura(), objMargen.leerDimensionesEspacio() );
	
	// Dibuja el tablero
	Tablero_Dibujar( tablero );
	
	// Dibuja la sombra de la pieza
	Pieza_Dibujar( piezaJugador, piezaJugador.sombra.x, piezaJugador.sombra.y, shadowColor[ piezaJugador.tipo ] );
	
	// Dibuja la pieza
	Pieza_Dibujar( piezaJugador, piezaJugador.figura.x, piezaJugador.figura.y, shapeColor[ piezaJugador.tipo ] );

	// Dibuja la cola de figuras
	Cola_Dibujar( colaFiguras );

	// Dibuja la pieza guardada
	FS_DibujarFigura( pieceSaved - 1, ( objTablero.leerEspacioX() - 1.7f ), 0.7f );

	// Dibuja el puntaje
	objPuntaje.renderTexture( objPuntaje.leerDimensionesTextura(), objPuntaje.leerDimensionesEspacio() );

	// Dibuja el nivel
	objNivel.renderTexture( objNivel.leerDimensionesTextura(), objNivel.leerDimensionesEspacio() );

	// Dibuja el numero de lineas
	objLineas.renderTexture( objLineas.leerDimensionesTextura(), objLineas.leerDimensionesEspacio() );

	// Dibuja el letrero de ¡objYa!
	objYa.renderTexture( objYa.leerDimensionesTextura(), objYa.leerDimensionesEspacio() );

	// Dibuja el tiempo
	FS_DibujarTiempo( tiempoPartida.obtenerTicks(), objTiempo, fuenteTexto, objTablero.leerEspacioX() + objTablero.leerEspacioAncho() + 0.6, 5.43f );
	objTiempo.renderTexture( objTiempo.leerDimensionesTextura(), objTiempo.leerDimensionesEspacio() );
}

/* FUNCTIONS */
// Updates game's viewport if window is resized
void FourSquares::actualizarViewport()
{		
	// Background
	objFondo.escribirTexturaW( ( ( (float)espacioAncho / (float)espacioAlto ) / Objeto::RELACION_ASPECTO ) * (float)objFondo.getWidth() );
	objFondo.escribirTexturaX( ( objFondo.getWidth() - objFondo.leerTexturaW() ) / 2 );
	objFondo.escribirDimensionesEspacio( espacioX, espacioY, espacioAncho, espacioAlto );

	// Board surface
	objTablero.escribirEspacioX( ( espacioAncho - objTablero.leerEspacioAncho() ) / 2  );
	objTablero.escribirEspacioY( ( ( espacioAlto - objTablero.leerEspacioAlto() ) / 2 ) - objBloque.leerEspacioAncho() );
	
	// Margen
	objMargen.escribirEspacioX( ( espacioAncho - objMargen.leerEspacioAncho() ) / 2 );
	objMargen.actualizarDimensionesAbsolutas();

	// Figura
	objFiguras.actualizarDimensionesAbsolutas();

	//
	objBloque.actualizarDimensionesAbsolutas();

	// 
	objYa.escribirEspacioX( ( espacioAncho - objYa.leerEspacioAncho() ) / 2 );

	// Texto renderizado
	FS_ActualizarPuntaje( contadorPuntaje, lineasJugador, contadorNivel, contadorCombo, objPuntaje );
	FS_ActualizarNivel( contadorNivel, contadorLineas, objNivel );
	FS_ActualizarLineas( contadorLineas, lineasJugador, objLineas );
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

	if( !Tablero_PermiteMover( tablero, pieza, pieza.figura.x, pieza.figura.y ) ){
		// Puede moverse si se mueve a la derecha
		if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x + 1, pieza.figura.y ) ){
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
		else if( pieza.tipo == FIGURA_LINEA ){
			if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x + 2, pieza.figura.y ) ){
				pieza.figura.x += 2;
			}
			else if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x - 2, pieza.figura.y ) ){
				pieza.figura.x -= 2;
			}
			else if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x, pieza.figura.y - 2 ) ){
				pieza.figura.y -= 2;
			}
			else if( Tablero_PermiteMover( tablero, pieza, pieza.figura.x, pieza.figura.y + 2 ) ){
				pieza.figura.y += 2;
			}
			else{
				Pieza_Rotar( pieza, direccion * -1 );
			}
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
	SDL_Rect auxRect = { 0, 0, objBloque.leerAbsolutoAncho(), objBloque.leerAbsolutoAncho() };
	int tempX = objTablero.leerAbsolutoX();
	int tempY = objTablero.leerAbsolutoY();
	
	// Establece el color
	objBloque.setColorMod( color );

	// Dibuja la figura
	for( int i = 0; i < 4; i++ ){
		auxRect.x = tempX + ( ( posicionX + pieza.bloques[ i ].x ) * auxRect.w );
		auxRect.y = tempY + ( ( posicionY + pieza.bloques[ i ].y ) * auxRect.w );
		
		objBloque.renderTexture( objBloque.leerDimensionesTextura(), &auxRect );
	}
}

// Inicializa el tablero en ceros
void Tablero_Inicializar( int tablero[ BOARD_HEIGHT ][ BOARD_WIDTH ] ){
	for( size_t renglones = 0; renglones < BOARD_HEIGHT; renglones++ ){
		for( int columnas = 0; columnas < BOARD_WIDTH; columnas++ ){
			tablero[ renglones ][ columnas ] = 0;
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
	// Rect que 
	SDL_Rect auxRect = { 0, 0, objBloque.leerAbsolutoAncho(), objBloque.leerAbsolutoAlto() };
	int tempX = objTablero.leerAbsolutoX();
	int tempY = objTablero.leerAbsolutoY();
	
	for( int renglones = 0; renglones < BOARD_HEIGHT; renglones++ ){
		for( int columnas = 0; columnas < BOARD_WIDTH; columnas++ ){
			if( tablero[ renglones ][ columnas ] ){
				auxRect.x = tempX + ( auxRect.w * columnas );
				auxRect.y = tempY + ( auxRect.w * renglones );
				objBloque.setColorMod( shapeColor[ tablero[ renglones ][ columnas ] - 1 ] );
				objBloque.renderTexture( objBloque.leerDimensionesTextura(), &auxRect );
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
		FS_DibujarFigura( figura,  objTablero.leerEspacioX() + objTablero.leerEspacioAncho() + 0.3, 0.68 + ( objFiguras.leerEspacioAlto() * contador ) + (0.12 * contador ) );
	}
}

void FS_DibujarFigura( int figura, double x, double y ){
	objFiguras.escribirEspacioX( x );
	objFiguras.escribirEspacioY( y );
	objFiguras.setColorMod( shapeColor[ figura ] );
	objFiguras.renderTexture( &shapeRects[ figura ], objFiguras.leerDimensionesEspacio() ); 
}

void FS_ActualizarLineas( int &lineasJugador, vector< int > &lineasRealizadas, Objeto &objeto ){
	// Suma la cantidad de líneas realizadas
	lineasJugador += lineasRealizadas.size();

	// Actualiza la textura con el nuevo número de líneas
	Fuente_ActualizarTexto( to_string( lineasJugador ), fuenteTexto, objeto );
	objeto.escribirEspacioX( objMargen.leerEspacioX() + 0.75f );
	objeto.escribirEspacioY( 5.4f );
}

void FS_ActualizarNivel( int &nivelJugador, int &lineasJugador, Objeto &objeto ){
	// Determina el nivel del jugador 
	nivelJugador = ( lineasJugador / 10 ) + 1;
	if( nivelJugador > 15 ){
		nivelJugador = 15;
	}

	Fuente_ActualizarTexto( to_string( nivelJugador ), fuenteTexto, objeto );
	objeto.escribirEspacioX( objMargen.leerEspacioX() + 0.75f );
	objeto.escribirEspacioY( 4.4f );
}

void FS_ActualizarPuntaje( int &puntaje, vector< int > &lineas, int nivel, int combo, Objeto &objeto )
{
	// Obtiene el nuevo puntaje
	int nuevoPuntaje = ( lineas.empty() ? 0 : 50 );

	// Actualiza el puntaje
	for( size_t i = 1; i < lineas.size(); ++i ){
		nuevoPuntaje = nuevoPuntaje * i * nivel; 
	}
	
	// Multiplica el puntaje de acuerdo al combo
	puntaje += nuevoPuntaje * combo;
	
	// Actualiza la textura del puntaje
	Fuente_ActualizarTexto( to_string( puntaje ), fuenteTexto, objeto );
	objeto.escribirEspacioX( objMargen.leerEspacioX() + 0.227 );
	objeto.escribirEspacioY( 2.33 );
}

void FS_DibujarTiempo( Uint32 tiempo, Objeto &objeto, Fuente &fuente, double x, double y )
{
	stringstream tiempoStr;
	tiempoStr << (tiempo / 60000) % 60 << ":" << std::setfill('0') << std::setw(2) << (tiempo / 1000) % 60 << ":" << (tiempo % 1000) / 10;

	// Fuente
	SDL_Color color = { 255, 255, 255 };
	if( !objeto.crearTexturaDesdeTextoSolido( tiempoStr.str().c_str(), color, fuente.fuente ) ){
		Juego_FinalizarEstado();
		return;
	}

	objeto.escribirDimensionesTextura( 0, 0, objeto.getWidth(), objeto.getHeight() );
	objeto.escribirDimensionesEspacio( x, y, (float)objeto.getWidth() / Objeto::leerMagnitudUnidad(), (float)objeto.getHeight() / Objeto::leerMagnitudUnidad() );
}

void FS_PausarPartida(){
	// Pausa el tiempo
	indicadorTiempo.pausar();
	tiempoPartida.pausar();
	tiempoAdicional.pausar();
	tiempoEntradaBajada.pausar();
	tiempoEntradaLaterales.pausar();

	// Evita que se muestren algunas cosas
	objBloque.show( false );
}

void FS_ReanudarPartida( void ){
	// Reanuda el tiempo
	tiempoPartida.reanudar();
	tiempoAdicional.reanudar();
	tiempoEntradaBajada.reanudar();
	tiempoEntradaLaterales.reanudar();
	indicadorTiempo.reanudar();

	// Vuelve a mostrar lo que se había ocultado
	objBloque.show( true );
}

/* VARIABLES */
// Tiempo del juego
Temporizador tiempoPartida;

// Game's timer
Temporizador tiempoAdicional;
Temporizador indicadorTiempo;

// Input timer
int nivelRespuestaLaterales;
Temporizador tiempoEntradaBajada;
Temporizador tiempoEntradaLaterales;

//
Temporizador tiempoAnimacion;

//
vector< int > lineasJugador;

// Game's variables
int tablero[ BOARD_HEIGHT ][ BOARD_WIDTH ];

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
bool permitirCambio;
int pieceSaved;
int pasosRealizados = 0;
bool arribaPresionado = false;

// Puntaje
int contadorCombo;
int comboMaximo;
int contadorLineas;
int contadorNivel;
int contadorPuntaje;

int columna;
