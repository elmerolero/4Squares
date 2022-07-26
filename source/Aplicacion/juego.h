#ifndef JUEGO_H
#define JUEGO_H

#include "estadojuego.h"
#include "temporizador.h"
#include "globales.h"
#include "objeto.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <deque>

extern Uint32 previous_frame_time;

// Inicializa el juego con el nombre dado
void Juego_Iniciar( std::string nombre, std::string mensaje );

// Gestiona la ejecución del juego
void Juego_Entrada( void );                   
void Juego_Logica( void );
void Juego_Renderizar( void );
void Juego_Cerrar( void );

// Dibuja un fondo negro transparente
void Juego_FondoNegro( void );

// Actualiza las dimensiones de la ventana
void Juego_ActualizarVentana( void );

// Establece el estado del juego
void Juego_EstablecerEstado( EstadoJuego *estado, EstadoAccion accion );
void Juego_ActualizarEstado( void );
bool Juego_Salir( void );

void Juego_LimpiarEstados( void );
void Juego_LimpiarPendientes( void );

// Obtiene un valor del mapa de variables
void Juego_CargarValoresMensaje( std::string archivo );
void establecerMensaje( std::string valor, std::string mensaje );

// Carga el contenido del juego
void Juego_CargarMedia( std::string resolucion );

void Fuente_ActualizarTamanio( Fuente &fuente );
void Fuente_ActualizarTexto( std::string texto, Fuente &fuente, Objeto &objeto );
void Fuente_ActualizarTexto( std::string texto, Fuente &fuente, Objeto &objeto, double x, double y );

#endif