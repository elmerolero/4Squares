#ifndef OBJECT_H
#define OBJECT_H
#include <string>
#include <SDL2/SDL.h>
#include "texture.h"

// For floating point operations
struct SDL_DRect
{
	double x;
	double y;
	double w;
	double h;
};

class Objeto : public Texture
{
	public:
		Objeto();
		~Objeto();
		
		//
		static const double UNIDAD_PANTALLA;
		static const double RELACION_ASPECTO;

		// Renderiza
		void renderizar( void );

		// Carga textura desde archivo
		void leerObjetoDesdeArchivo( std::string ubicacion );

		// Sets the relative position of the object
		void escribirEspacioX( double x );
		void escribirEspacioY( double y );
		void escribirEspacioAncho( double w );
		void escribirEspacioAlto( double h );
		
		// Gets the relative position of the object
		double leerEspacioX( void ) const;
		double leerEspacioY( void ) const;
		double leerEspacioAncho( void ) const;
		double leerEspacioAlto( void ) const;

		// Permite leer las dimensiones absolutas del objeto
		int leerAbsolutoX( void ) const;
		int leerAbsolutoY( void ) const;
		int leerAbsolutoAncho( void ) const;
		int leerAbsolutoAlto( void ) const;

		/* Dimensiones de la textura */
		void escribirTexturaX( int x );	// Establece la posición X de la textura
		void escribirTexturaY( int y );
		void escribirTexturaW( int w );
		void escribirTexturaH( int h );

		// Gets texture dimensions
		int leerTexturaX( void ) const;
		int leerTexturaY( void ) const;
		int leerTexturaW( void ) const;
		int leerTexturaH( void ) const;
		
		// Establece las dimensiones de la textura
		void escribirDimensionesTextura( SDL_Rect rect );
		void escribirDimensionesTextura( int x, int y, int ancho, int alto );

		// Obtiene las dimensiones de la textura
		SDL_Rect *leerDimensionesTextura( void );

		// Establece las dimensiones Espacioes dado un rect de doubles
		void escribirDimensionesEspacio( SDL_DRect rect );
		void escribirDimensionesEspacio( double x, double y, double ancho, double alto );

		// Actualiza las dimensiones 
		void actualizarDimensionesAbsolutas( void );

		// Obtiene las dimensiones absolutas
		SDL_Rect *leerDimensionesEspacio( void );
		
		static double leerMagnitudUnidad( void );
		static void actualizarMagnitudUnidad( int pantallaAltura );

	private:
		// Dimensiones de la textura
		SDL_Rect rectTextura;

		// Dimensiones absolutas en la pantalla
		SDL_Rect rectAbsoluto;

		// Dimensiones Espacio
		double espacioX;
		double espacioY;
		double espacioAncho;
		double espacioAlto;

		// Tamaño de la unidad de la pantalla
		static double magnitudUnidad;
};

#endif
