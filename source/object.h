#ifndef OBJECT_H
#define OBJECT_H
#include <string>
#include <SDL2/SDL.h>
#include "game.h"

class Object
{
	public:
		Object();
		~Object();
		
		// Load the relative coords from file
		bool loadCoordinatesFromFile( const char * path );
		
		// Sets relative coords from given rect
		void setRelativeCoords( SDL_DRect rect );
		
		// Sets texture coords from given rect
		void setTextureCoords( SDL_Rect rect );
		
		// Sets the relative position of the object
		void setRelativeX( double x );
		void setRelativeY( double y );
		void setRelativeW( double w );
		void setRelativeH( double h );
		
		// Gets the relative position of the object
		double getRelativeX();
		double getRelativeY();
		double getRelativeW();
		double getRelativeH();
		
		// Gets
		int getAbsoluteX();
		int getAbsoluteY();
		int getAbsoluteW();
		int getAbsoluteH();

		/* Dimensiones de la textura */
		void establecerTexturaX( int x );	// Establece la posición X de la textura
		void establecerTexturaY( int y );
		void establecerTexturaW( int w );
		void establecerTexturaH( int h );

		// Gets texture dimensions
		int obtenerTexturaX( void ) const;
		int obtenerTexturaY( void ) const;
		int obtenerTexturaW( void ) const;
		int obtenerTexturaH( void ) const;
		
		void setAbsoluteX( int x );
		void setAbsoluteY( int y );

		void actualizarCoordanadasAbsolutas( void );
		
		// Returns the src rect
		SDL_Rect* getSrcRect( void );
		
		// Returns the dest rect 
		SDL_Rect* getDestRect( void );
		
	private:
		SDL_Rect  textureRect;
		SDL_DRect relativeRect;
		SDL_Rect  absoluteRect;
		int fontSize;
};

#endif
