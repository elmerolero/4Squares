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
void Juego_FinalizarEstado( void );
void Juego_ApilarEstado( EstadoJuego * );
void Juego_EstablecerEstado( EstadoJuego *estado );
void Juego_ActualizarEstado( void );
bool Juego_Salir( void );

// Obtiene un valor del mapa de variables
void Juego_CargarValoresMensaje( std::string archivo );
void establecerMensaje( std::string valor, std::string mensaje );

// Carga el contenido del juego
void Juego_CargarMedia( void );

void Fuente_ActualizarTamanio( Fuente &fuente );
void Fuente_ActualizarTexto( std::string texto, Fuente &fuente, Objeto &objeto );

#endif