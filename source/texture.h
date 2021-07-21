#ifndef TEXTURE_H
#define TEXTURE_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "game.h"

class Texture
{
	public:
		// Constructor
		Texture();
		
		// Destructor
		~Texture();
		
		// Loads texture from a file
		bool loadFileTexture( const char* path );
		
		bool crearTexturaDesdeSuperficie( SDL_Renderer *render, SDL_Surface *superficie );

		// Crea una textura dado un texto en texto solido
		bool crearTexturaDesdeTextoSolido( const char *texto, SDL_Color color, TTF_Font *fuente );

		// Crea una textura desde texto en modo blended
		bool crearTexturaDesdeTextoBlended( const char *texto, SDL_Color color, TTF_Font *fuente, Uint32 largoMaximo );

		// Crea una textura renderizable
		void crearTexturaRenderizable( SDL_Renderer *render, int ancho, int alto );

		void establecerTexturaRenderizado( SDL_Renderer *render );

		// Sets blend mode 
		void setBlendMode( SDL_BlendMode blend );
		
		// Sets alpha modulation
		void setAlphaMod( Uint8 alpha );
		
		// Sets color modulation
		void setColorMod( SDL_Color color );
		
		// Show or hide render
		void show( bool show );
		bool show( void ) const;

		// Renders the texture
		void renderTexture( const SDL_Rect* srcRect, const SDL_Rect* destRect );
		
		// Destroys the texture
		void destroyTexture();
		
		// Returns width
		int getWidth();
		
		// Returns height
		int getHeight();
		
	private:
		SDL_Texture* ptrTexture;
		bool showTexture;
		int	textureWidth;
		int textureHeight;
		 
};

#endif
