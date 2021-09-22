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

void Jugador_Iniciar( Jugador &jugador ){
	// Inicializa la pieza en juego
	Pieza_NuevaPieza( jugador.pieza, rand() % 7, jugador.tablero );
	jugador.piezaReservada = 0;

	// Inicializa el tablero
	Tablero_Inicializar( jugador.tablero );

	// Inicializa contadores a cero
	jugador.lineas = 0;
	jugador.nivel = 1;
	jugador.puntaje = 0;
	jugador.combo = 0;
	jugador.comboMaximo = 0;

	// Inicializa la cola de figuras del jugador
	Cola_Inicializar( jugador.colaFiguras );

	// Establece el nivel de respuesta a cero
	jugador.nivelRespuesta = 0;
	jugador.pasosRealizados = 0;
	jugador.columnaActual = 0;
	jugador.soltarPieza = false;
	jugador.permiteReserva = true;
	jugador.finalizo = false;

	// Inicializa los temporizadores
	jugador.tiempoCambio.iniciar();
	jugador.tiempoAgregado.iniciar();
	jugador.tiempoBajada.iniciar();
	jugador.tiempoLaterales.iniciar();
	jugador.tiempoAnimacion.iniciar();
}

FourSquares::FourSquares(): nombre( "Gameplay" )
{
	numeroJugadores = 1;

	// Actualiza el viewport
	actualizarViewport();
}

// Actualiza lineas jugador
bool FS_ActualizarLineasJugador( Jugador &jugador ){
	// 
	if( !jugador.lineasRealizadas.empty() && jugador.tiempoAnimacion.obtenerTicks() > 20 ){
		// Si no ha eliminado todas las columnas
		if( jugador.columnaActual < BOARD_WIDTH ){
			// Elimina los cuadros de la columna dada
			for( size_t renglones = 0; renglones < jugador.lineasRealizadas.size(); ++renglones ){
				int renglon = jugador.lineasRealizadas[ renglones ];
				int columna = jugador.columnaActual;
				jugador.tablero[ renglon  ][ columna ] = 0;
			}

			// Incrementa el número de columna
			jugador.columnaActual++;

			// Reinicia el tiempo
			jugador.tiempoAnimacion.reiniciar();
		}
		else{
			// Acomoda los elementos del tablero
			Tablero_Acomodar( jugador.tablero, jugador.lineasRealizadas );

			// Actualiza la sombra
			Pieza_ActualizarSombra( jugador.pieza, jugador.tablero );

			// Limpia las líneas del jugador
			jugador.lineasRealizadas.clear();

			// Reinicia
			jugador.columnaActual = 0;
		}
	}

	return jugador.lineasRealizadas.empty();
}

void FS_ActualizarEstadoJugador( Jugador &jugador ){
	// Revisa si es momento de bajar la pieza
	if( ( jugador.tiempoCambio.obtenerTicks() >= downSpeed[ jugador.nivel - 1 ] ) || jugador.soltarPieza ){
		// Reinicia el tiempo
		jugador.tiempoCambio.reiniciar();
		
		// ¿No hay colisión en el siguiente paso?
		if( Tablero_PermiteMover( jugador.tablero, jugador.pieza, jugador.pieza.figura.x, jugador.pieza.figura.y + 1 ) ){
			jugador.pieza.figura.y++;
			return;
		}

		// Brinda tiempo adicional para acomodar la pieza (útil para niveles rápidos)
		jugador.tiempoAgregado.reanudar();
		if( jugador.tiempoAgregado.obtenerTicks() < 750 && jugador.pasosRealizados < PASOS_MAXIMOS && !jugador.soltarPieza ){
			return;
		}

		// Guarda la pieza en el tablero dado
		Pieza_Grabar( jugador.pieza, jugador.tablero );

		// ¿El jugador ya se encuentra en el tope del tablero?
		if( jugador.pieza.figura.y == 1 ){
			jugador.tiempoCambio.pausar();
			jugador.tiempoBajada.pausar();
			jugador.tiempoLaterales.pausar();
			jugador.finalizo = true;
			return;
		}
		
		// Obtiene las lineas a borrar
		Tablero_ObtenerLineas( jugador.tablero, jugador.lineasRealizadas );

		jugador.combo++;
		if( jugador.combo > jugador.comboMaximo ){
			jugador.comboMaximo = jugador.combo;
		}
				
		if( jugador.lineasRealizadas.empty() ){
			jugador.combo = 0;
		}

		// Actualiza el número de líneas realizadas
		jugador.lineas += jugador.lineasRealizadas.size();

		// Actualiza el nivel del jugador
		jugador.nivel = ( jugador.lineas / 10 ) + 1;				// Nivel = numeroLineas / 10
		jugador.nivel = jugador.nivel > 15 ? 15 : jugador.nivel;    // Asegura que nivel no rebase 15

		// Actualiza el puntaje
		jugador.puntaje = 50 * factorial( jugador.lineasRealizadas.size() );

		// Obtiene la nueva pieza
		Pieza_NuevaPieza( jugador.pieza, Cola_ObtenerSiguenteFigura( jugador.colaFiguras ), jugador.tablero );
		jugador.permiteReserva = true;
		jugador.soltarPieza = false;
		jugador.pasosRealizados = 0;
		jugador.tiempoAgregado.reiniciar();
		jugador.tiempoAgregado.pausar();

		// Actualiza la información del juego
		Fuente_ActualizarTexto( to_string( jugador.lineas ), fuenteTexto, objLineas, objMargen.leerEspacioX() + 0.75f, 5.4 );
		Fuente_ActualizarTexto( to_string( jugador.nivel ), fuenteTexto, objNivel, objMargen.leerEspacioX() + 0.75f, 4.4 );
		Fuente_ActualizarTexto( to_string( jugador.puntaje ), fuenteTexto, objPuntaje, objMargen.leerEspacioX() + 0.75f, 2.33 );
	}
}
 
FourSquares::~FourSquares()
{
	objBloque.destroyTexture();
	objFiguras.destroyTexture();
	objMargen.destroyTexture();
	objFondo.destroyTexture();
}

void FourSquares::estadoEventos( SDL_Event &gGameEvent ){
	if( gGameEvent.type == SDL_JOYBUTTONDOWN && joystickConectado ){
		for( size_t contador = 0; contador < numeroJugadores; ++contador ){
			if( gGameEvent.cbutton.which == controles[ contador ].id ){	
				FS_LeerEventosControlJugador( jugadores[ contador ], controles[ contador ], gGameEvent.cbutton.button );
			}
		}
	}
	else if( gGameEvent.type == SDL_KEYDOWN && !jugadores[ JUGADOR_UNO ].soltarPieza ){
		FS_LeerEventosTecladoJugador( jugadores[ JUGADOR_UNO ], teclado, gGameEvent.key.keysym.sym );
	}
}

void FourSquares::estadoEntrada(){
	// Lee el estado del teclado
	const Uint8 *keyboardState = SDL_GetKeyboardState( NULL );
	
	if( joystickConectado ){
		for( size_t contador = 0; contador < numeroJugadores; ++contador ){
			if( !jugadores[ contador ].soltarPieza || jugadores[ contador ].lineasRealizadas.empty() ){
				FS_LeerEntradaControlJugador( jugadores[ contador ], controles[ contador ] );
			}
		}
	}
	else{
		if( !jugadores[ JUGADOR_UNO ].soltarPieza || jugadores[ JUGADOR_UNO ].lineasRealizadas.empty() ){
			FS_LeerEntradaTecladoJugador( jugadores[ JUGADOR_UNO ], teclado, keyboardState );
		}
	}
}

void FourSquares::estadoLogica()
{ 	
	// Decide si mostrar o no el letrero ya
	if( tiempoPartida.obtenerTicks() > 2000 ){
		objYa.show( false );
	}

	for( size_t contador = 0; contador < numeroJugadores; ++contador ){
		if( FS_ActualizarLineasJugador( jugadores[ contador ] ) ){
			FS_ActualizarEstadoJugador( jugadores[ contador ] );
		}
	}
}

void FourSquares::estadoRenderizado()
{
	// Dibuja el fondo
	objFondo.renderTexture( objFondo.leerDimensionesTextura(), objFondo.leerDimensionesEspacio() );
	objMargen.renderTexture( objMargen.leerDimensionesTextura(), objMargen.leerDimensionesEspacio() );
	
	Tablero_Dibujar( jugadores[ 0 ].tablero );
	Pieza_Dibujar( jugadores[ 0 ].pieza, jugadores[ 0 ].pieza.sombra.x, jugadores[ 0 ].pieza.sombra.y, shadowColor[ jugadores[ 0 ].pieza.tipo ] );
	Pieza_Dibujar( jugadores[ 0 ].pieza, jugadores[ 0 ].pieza.figura.x, jugadores[ 0 ].pieza.figura.y, shapeColor[ jugadores[ 0 ].pieza.tipo ] );
	Cola_Dibujar( jugadores[ 0 ].colaFiguras );
	FS_DibujarFigura( jugadores[ 0 ].piezaReservada - 1, ( objTablero.leerEspacioX() - 1.7f ), 0.7f );

	FS_DibujarTiempo( tiempoPartida.obtenerTicks(), objTiempo, fuenteTexto, objTablero.leerEspacioX() + objTablero.leerEspacioAncho() + 0.6, 5.43f );
	objTiempo.renderTexture( objTiempo.leerDimensionesTextura(), objTiempo.leerDimensionesEspacio() );

	// Dibuja el tablero
	/*Tablero_Dibujar( tablero );
	
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
	objTiempo.renderTexture( objTiempo.leerDimensionesTextura(), objTiempo.leerDimensionesEspacio() );*/
}

/* FUNCTIONS */
// Updates game's viewport if window is resized
void FourSquares::actualizarViewport(){
	// Background
	objFondo.escribirTexturaW( ( ( (float)espacioAncho / (float)espacioAlto ) / Objeto::RELACION_ASPECTO ) * (float)objFondo.getWidth() );
	objFondo.escribirTexturaX( ( objFondo.getWidth() - objFondo.leerTexturaW() ) / 2 );
	objFondo.escribirDimensionesEspacio( espacioX, espacioY, espacioAncho, espacioAlto );

	// Board surface
	objTablero.escribirEspacioX( ( espacioAncho - objTablero.leerEspacioAncho() ) / 2  );
	objTablero.escribirEspacioY( ( ( espacioAlto - objTablero.leerEspacioAlto() ) / 2 ) - objBloque.leerEspacioAncho() );
	
	// Margen
	objMargen.escribirEspacioX( ( espacioAncho - objMargen.leerEspacioAncho() ) / 2 );
	objMargen.escribirEspacioY( ( espacioAlto - objMargen.leerEspacioAlto() ) / 2 );
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
bool Tablero_PermiteMover( int tablero[ BOARD_HEIGHT ][ BOARD_WIDTH ], Pieza &pieza, int posicionX, int posicionY ){
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

void Tablero_Acomodar( int tablero[ BOARD_HEIGHT ][ BOARD_WIDTH ], vector< int > &lineas ){
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
		Juego_EstablecerEstado( nullptr, ESTADO_FINALIZAR );
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

void FS_LeerEventosTecladoJugador( Jugador &jugador, Teclado &control, SDL_Keycode codigo ){
	if( codigo == control.soltarPieza ){
		jugador.soltarPieza = true;
	}
	else if( codigo == control.pausarJuego ){
		FS_PausarPartida();
		Juego_EstablecerEstado( new Pausa(), ESTADO_APILAR );
	}
	else if( codigo == control.rotarDerecha && jugador.pieza.tipo != FIGURA_CUADRADO ){
		Pieza_Alternar( jugador.pieza, jugador.tablero, 1 );
		if( jugador.tiempoCambio.obtenerTicks() >= downSpeed[ contadorNivel - 1 ] ){
			jugador.pasosRealizados++;
			jugador.tiempoAgregado.reiniciar();
		}
	}
	else if( codigo == control.rotarIzquierda && jugador.pieza.tipo != FIGURA_CUADRADO ){
		Pieza_Alternar( jugador.pieza, jugador.tablero, -1 );
		if( jugador.tiempoCambio.obtenerTicks() >= downSpeed[ contadorNivel - 1 ] ){
			jugador.pasosRealizados++;
			jugador.tiempoAgregado.reiniciar();
		}
	}
	else if( jugador.permiteReserva && ( codigo == control.reservarPieza ) ){
		// Obtiene la pieza guardada
		int pieza = jugador.piezaReservada ? jugador.piezaReservada - 1 : Cola_ObtenerSiguenteFigura( jugador.colaFiguras );
		
		// Guarda la pieza en juego
		jugador.piezaReservada = jugador.pieza.tipo + 1;

		// Establece los nuevos datos de la pieza
		Pieza_NuevaPieza( jugador.pieza, pieza, jugador.tablero );

		// Establece las banderas y reinicia el tiempo
		jugador.permiteReserva = false;
		jugador.pasosRealizados = 0;
		jugador.tiempoAgregado.reiniciar();
		jugador.tiempoAgregado.pausar();
	}
}

void FS_LeerEntradaTecladoJugador( Jugador &jugador, Teclado &control, const Uint8 *teclado ){
	// Abajo
	if( teclado[ control.bajarPieza ] && jugador.tiempoBajada.obtenerTicks() >= 50 ){
		if( Tablero_PermiteMover( jugador.tablero, jugador.pieza, jugador.pieza.figura.x, jugador.pieza.figura.y + 1 ) ){
			jugador.pieza.figura.y++;
		}
		
		jugador.tiempoBajada.reiniciar();
	}

	// Izquierda
	if( teclado[ control.moverIzquierda ] ){
		if( jugador.tiempoLaterales.obtenerTicks() >= moveResponseTime[ jugador.nivelRespuesta ] ){
			if( Tablero_PermiteMover( jugador.tablero, jugador.pieza, jugador.pieza.figura.x - 1, jugador.pieza.figura.y ) ){
				jugador.pieza.figura.x--;
			}

			jugador.tiempoLaterales.reiniciar();
			if( jugador.tiempoCambio.obtenerTicks() >= downSpeed[ contadorNivel - 1 ] ){
				jugador.pasosRealizados++;
				jugador.tiempoAgregado.reiniciar();
			}

			if( jugador.nivelRespuesta < 2 ){
				jugador.nivelRespuesta++;
			}
		}

		Pieza_ActualizarSombra( jugador.pieza, jugador.tablero );
	}
	// Derecha
	else if( teclado[ control.moverDerecha ] ){
		if( jugador.tiempoLaterales.obtenerTicks() >= moveResponseTime[ jugador.nivelRespuesta ] ){
			if( Tablero_PermiteMover( jugador.tablero, jugador.pieza, jugador.pieza.figura.x + 1, jugador.pieza.figura.y ) ){
				jugador.pieza.figura.x++;
			}

			jugador.tiempoLaterales.reiniciar();
			if( indicadorTiempo.obtenerTicks() >= downSpeed[ contadorNivel - 1 ] ){
				jugador.pasosRealizados++;
				jugador.tiempoAgregado.reiniciar();
			}

			if( jugador.nivelRespuesta < 2 ){
				jugador.nivelRespuesta++;
			}
			Pieza_ActualizarSombra( jugador.pieza, jugador.tablero );
		}
	}
	else{
		jugador.nivelRespuesta = 0;
	}
}

void FS_LeerEventosControlJugador( Jugador &jugador, Control &control, Uint8 codigo ){
	if( !jugador.soltarPieza ){
		if( codigo == control.soltarPieza ){
			jugador.soltarPieza = true;
		}
		else if( codigo == control.pausarJuego ){
			indice = control.id;
			FS_PausarPartida();
			Juego_EstablecerEstado( new Pausa(), ESTADO_APILAR );
		}
		else if( codigo == control.rotarDerecha && jugador.pieza.tipo != FIGURA_CUADRADO ){
			Pieza_Alternar( jugador.pieza, jugador.tablero, 1 );
			if( jugador.tiempoCambio.obtenerTicks() >= downSpeed[ contadorNivel - 1 ] ){
				jugador.pasosRealizados++;
				jugador.tiempoAgregado.reiniciar();
			}
		}
		else if( codigo == control.rotarIzquierda && jugador.pieza.tipo != FIGURA_CUADRADO ){
			Pieza_Alternar( jugador.pieza, jugador.tablero, -1 );
			if( jugador.tiempoCambio.obtenerTicks() >= downSpeed[ contadorNivel - 1 ] ){
				jugador.pasosRealizados++;
				jugador.tiempoAgregado.reiniciar();
			}
		}
		else if( jugador.permiteReserva && ( codigo == control.reservarPieza ) ){
			// Obtiene la pieza guardada
			int pieza = jugador.piezaReservada ? jugador.piezaReservada - 1 : Cola_ObtenerSiguenteFigura( jugador.colaFiguras );
			
			// Guarda la pieza en juego
			jugador.piezaReservada = jugador.pieza.tipo + 1;

			// Establece los nuevos datos de la pieza
			Pieza_NuevaPieza( jugador.pieza, pieza, jugador.tablero );

			// Establece las banderas y reinicia el tiempo
			jugador.permiteReserva = false;
			jugador.pasosRealizados = 0;
			jugador.tiempoAgregado.reiniciar();
			jugador.tiempoAgregado.pausar();
		}
	}
}

void FS_LeerEntradaControlJugador( Jugador &jugador, Control &control ){
	// Abajo
	if( SDL_GameControllerGetButton( control.control, control.bajarPieza ) && jugador.tiempoBajada.obtenerTicks() >= 50 ){
		if( Tablero_PermiteMover( jugador.tablero, jugador.pieza, jugador.pieza.figura.x, jugador.pieza.figura.y + 1 ) ){
			jugador.pieza.figura.y++;
		}
		
		jugador.tiempoBajada.reiniciar();
	}

	// Izquierda
	if( SDL_GameControllerGetButton( control.control, control.moverIzquierda ) ){
		if( jugador.tiempoLaterales.obtenerTicks() >= moveResponseTime[ jugador.nivelRespuesta ] ){
			if( Tablero_PermiteMover( jugador.tablero, jugador.pieza, jugador.pieza.figura.x - 1, jugador.pieza.figura.y ) ){
				jugador.pieza.figura.x--;
			}

			jugador.tiempoLaterales.reiniciar();
			if( jugador.tiempoCambio.obtenerTicks() >= downSpeed[ contadorNivel - 1 ] ){
				jugador.pasosRealizados++;
				jugador.tiempoAgregado.reiniciar();
			}

			if( jugador.nivelRespuesta < 2 ){
				jugador.nivelRespuesta++;
			}
		}

		Pieza_ActualizarSombra( jugador.pieza, jugador.tablero );
	}
	// Derecha
	else if( SDL_GameControllerGetButton( control.control, control.moverDerecha ) ){
		if( jugador.tiempoLaterales.obtenerTicks() >= moveResponseTime[ jugador.nivelRespuesta ] ){
			if( Tablero_PermiteMover( jugador.tablero, jugador.pieza, jugador.pieza.figura.x + 1, jugador.pieza.figura.y ) ){
				jugador.pieza.figura.x++;
			}

			jugador.tiempoLaterales.reiniciar();
			if( indicadorTiempo.obtenerTicks() >= downSpeed[ contadorNivel - 1 ] ){
				jugador.pasosRealizados++;
				jugador.tiempoAgregado.reiniciar();
			}

			if( jugador.nivelRespuesta < 2 ){
				jugador.nivelRespuesta++;
			}
			Pieza_ActualizarSombra( jugador.pieza, jugador.tablero );
		}
	}
	else{
		jugador.nivelRespuesta = 0;
	}
}

unsigned int factorial( unsigned int numero ){
	unsigned int contador = numero;

	while( contador > 1 ){
		contador = contador - 1;
		numero = numero * contador;
	}

	return contador;
}

void FourSquares::escribirNombre( std::string nombre ){
    this -> nombre = nombre;
}

void FourSquares::mostrarNombre( void ){
    cout << nombre << endl;
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
