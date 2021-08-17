#ifndef GLOBALES_H
#define GLOBALES_H
#include "estadojuego.h"
#include "temporizador.h"
#include "objeto.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <unordered_map>
#include <vector>
#include <deque>

// Estructuras
struct Fuente{
	TTF_Font *fuente;
	std::string archivo;
	double magnitud;
};

// Constantes
#define NUMERO_JUGADORES 4		// Número máximo de jugadores
#define BOARD_WIDTH 	10      // Numero de columnas
#define BOARD_HEIGHT	22      // Número de renglones

// Componentes SDL
extern SDL_Window *gVentana;           // Ventana
extern SDL_Renderer *gRender;          // Contexto de renderizado
extern SDL_DisplayMode gVistaInfo;     // Modo de la pantalla
extern SDL_Event gEvento;              // Evento del juego

extern bool salir;                     // Indica si salir del juego
extern bool pantallaCompleta;          // Pantalla completa
extern std::string vSinc;              // Sincronizacion vertical               
extern bool mostrarFPS;                // Mostrar fotogramas por segundo   
extern bool calidadImagen;             // Calidad de imagen del juego

// Pantalla
extern double unidadMedida;            // Unidad de medida
extern int pantallaX;                  // Posicion X de la pantalla
extern int pantallaY;                  // Posicion Y de la pantalla
extern int pantallaAncho;              // Ancho de la vista
extern int pantallaAlto;               // Alto de la vista
extern double espacioX;                // Posición espacial X de la pantalla
extern double espacioY;                // Posición espacial Y de la pantalla
extern double espacioAncho;            // Ancho espacial de la pantalla
extern double espacioAlto;             // Alto espacial de la pantalla

// 
extern bool joystickConectado;

// Estados del juego
extern EstadoJuego *estadoActual;
extern std::deque< Pendiente * > estadosPendientes;
extern std::vector< EstadoJuego * > estadosJuego;

// Mensajes del juego
extern std::unordered_map< std::string, std::string > valoresMensaje;

// FPS
extern Temporizador temporizadorFPS;
extern int contadorFPS;

// Objetos gráficos
extern Fuente fuenteTexto;
extern Objeto fpsTexto;

// Objetos visibles del juego
extern Objeto objFondo;		// Fondo de pantalla
extern Objeto objMargen;	// Margen		
extern Objeto objTablero;	// Superficie del tablero
extern Objeto objBloque;	// Bloque
extern Objeto objFiguras;	// Figuras
extern Objeto objYa;		// Textura con el texto ya
extern Objeto objPuntaje; 	// Puntaje
extern Objeto objNivel;		// Nivel
extern Objeto objLineas;	// Lineas
extern Objeto objTiempo;	// Tiempos

#endif