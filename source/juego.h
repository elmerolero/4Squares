#ifndef JUEGO_H
#define JUEGO_H

#include "estadojuego.h"
#include "temporizador.h"
#include "objeto.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <unordered_map>
#include <string>
#include <vector>
#include <deque>

struct Pendiente{
    EstadoJuego *estado;
    int accion;
};

class Juego
{
    public:
        Juego();
        Juego( std::string nombre, std::string traducciones );

        // Inicializa el juego con el nombre dado
        void iniciar( std::string nombre, std::string traducciones );
        void cerrar( void );

        // Ejecución del juego
        void entrada( void );
        void logica( void );
        void renderizar( void );

        // Dibuja un fondo negro transparente
        void fondoNegro( void );

        // Obtiene el contexto de renderizado
        SDL_Renderer *leerConextoRender( void );

        // Actualiza las dimensiones de la ventana
        void actualizarVentana( void );

        // Establece el estado del juego
        void finalizarEstado( void );
        void apilarEstado( EstadoJuego * );
        void establecerEstado( EstadoJuego *estado );
        void actualizarEstado( void );
        void salir( bool salir );
        bool salir( void ) const;

        // Permite obtener las dimensiones de la pantalla
        int leerVistaX( void ) const;
        int leerVistaY( void ) const;
        int leerVistaAncho( void ) const;
        int leerVistaAlto( void ) const;

        // Permite establecer las dimensiones espaciales de la pantalla
        void escribirEspacioX( double x );
        void escribirEspacioY( double y );
        void escribirEspacioAncho( double ancho );
        void escribirEspacioAlto( double alto );

        // Permite obtener las dimensiones espaciales de la pantalla
        double leerEspacioX( void ) const;
        double leerEspacioY( void ) const;
        double leerEspacioAncho( void ) const;
        double leerEspacioAlto( void ) const;

        // Obtiene el magniud de la unidad en pixeles
        double leerMagnitudUnidad( void ) const;

        // Obtiene un valor del mapa de variables
        void cargarValoresMensaje( std::string archivo );
        void establecerMensaje( std::string valor, std::string mensaje );
        std::string leerValor( std::string nombreValor );

    private:
        // Componentes SDL
        SDL_Window *jVentana;
        SDL_Renderer *jRender;
        SDL_DisplayMode jVistaInfo;
        SDL_Event jEvento;

        // Preferencias del juego
        bool jSalir;
        bool jPantallaCompleta;
        std::string jSincVertical;
        bool jMostrarTasaCuadros;
        bool jCalidadImagen;

        // Estados del juego
        EstadoJuego *estadoActual;
        std::deque< Pendiente * > estadosPendientes;
        std::vector< EstadoJuego * > jEstados;

        // Tamaño de la unidad espacial
        double magnitudUnidad;

        // Para la medición de los Fotogramas por segundo (FPS)
        Temporizador temporizadorFPS;
        int contadorFPS;

        // Dimensiones de la pantalla
        int jPantallaX;
        int jPantallaY;
        int jPantallaAncho;
        int jPantallaAlto;

        // Dimensiones relativas de la pantalla
        double jEspacioX;
        double jEspacioY;
        double jEspacioAncho;
        double jEspacioAlto;

        // Para poder traducir los programas
        std::unordered_map< std::string, std::string > valoresMensajes;
};

struct Fuente{
	TTF_Font *fuente;
	std::string archivo;
	double magnitud;
};

extern Fuente fuenteTexto;
extern Objeto fpsTexto;

void Fuente_ActualizarTamanio( Fuente &fuente );
void Fuente_ActualizarTexto( std::string texto, Fuente &fuente, Objeto &objeto );

static const int ESTADO_CONTINUAR = 1;
static const int ESTADO_APILAR = 2;
static const int ESTADO_ESTABLECER = 3;
static const int ESTADO_FINALIZAR = 0;

#endif