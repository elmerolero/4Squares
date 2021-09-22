#include "globales.h"
using namespace std;

// Componentes SDL
SDL_Window *gVentana;           // Ventana
SDL_Renderer *gRender;          // Contexto de renderizado
SDL_DisplayMode gVistaInfo;     // Modo de la pantalla
SDL_Event gEvento;              // Evento del juego

bool salir;                     // Indica si salir del juego
bool pantallaCompleta;          // Pantalla completa
string vSinc;                   // Sincronizacion vertical               
bool mostrarFPS;                // Mostrar fotogramas por segundo   
bool calidadImagen;             // Calidad de imagen del juego

// Pantalla
double unidadMedida;            // Unidad de medida
int pantallaX;                  // Posicion X de la pantalla
int pantallaY;                  // Posicion Y de la pantalla
int pantallaAncho;              // Ancho de la vista
int pantallaAlto;               // Alto de la vista
double espacioX;                // Posición espacial X de la pantalla
double espacioY;                // Posición espacial Y de la pantalla
double espacioAncho;            // Ancho espacial de la pantalla
double espacioAlto;             // Alto espacial de la pantalla

// Controles del juego (permite un número máximo de cuatro jugadores)
bool joystickConectado;
int numeroJugadores;
Jugador jugadores[ NUMERO_JUGADORES ];
Teclado teclado;
Control controles[ NUMERO_JUGADORES ];

// Estados del juego
EstadoJuego *estadoActual;
deque< Pendiente * > estadosPendientes;
vector< EstadoJuego * > estadosJuego;

// Mensajes del juego
unordered_map< std::string, std::string > valoresMensaje;

// Para el conteo de FPS's
Temporizador temporizadorFPS;
int contadorFPS;

// Objetos gráficos
Fuente fuenteTexto;
Objeto fpsTexto;

// Objetos visibles del juego
Objeto objFondoInicio; // Fondo de inicio
Objeto objFondo;	// Fondo de pantalla
Objeto objMargen;   // Margen		
Objeto objTablero;  // Superficie del tablero
Objeto objBloque;   // Bloque
Objeto objFiguras;      // Figuras
Objeto objYa;          // Textura con el texto ya
Objeto objPuntaje;     // Puntaje
Objeto objNivel;       // Nivel
Objeto objLineas;      // Lineas
Objeto objTiempo;      // Tiempos
Objeto objCuentaRegresiva; // Cuenta regresiva