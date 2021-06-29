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
		
		// Crea una textura dado un texto
		bool crearTexturaDesdeTexto( const char *texto, SDL_Color color, TTF_Font *fuente );

		// Sets blend mode 
		void setBlendMode( SDL_BlendMode blend );
		
		// Sets alpha modulation
		void setAlphaMod( Uint8 alpha );
		
		// Sets color modulation
		void setColorMod( SDL_Color color );
		
		// Renders the texture
		void renderTexture( SDL_DRect destRect, SDL_Rect srcRect );
		void renderTexture( SDL_Rect* srcRect, SDL_Rect* destRect );
		
		// Destroys the texture
		void destroyTexture();
		
		// Returns width
		int getWidth();
		
		// Returns height
		int getHeight();
		
	private:
		SDL_Texture* ptrTexture;
		int	textureWidth;
		int textureHeight;
		 
};

#endif
