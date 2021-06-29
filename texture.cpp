#include "texture.h"
#include "game.h"
#include <iostream>
using namespace std;

Texture::Texture()
{
	ptrTexture = NULL;
}

Texture::~Texture()
{
	destroyTexture();
}

bool Texture::loadFileTexture( const char* path )
{
	// Frees texture if exists another
	if( ptrTexture != NULL ){
		destroyTexture();
	}
	
	// Surface for creating surface
	SDL_Surface* ptrAuxSurface = NULL;
	ptrAuxSurface = IMG_Load( path );
	if( ptrAuxSurface == NULL ){
		cout << "Error loading image. Details: " << IMG_GetError() << endl;
		return false;
	}

	ptrTexture = SDL_CreateTextureFromSurface( gPtrRenderer, ptrAuxSurface );
	if( ptrTexture == NULL ){
		cout << "We could not create the texture. Details: " << SDL_GetError() << endl;
		SDL_FreeSurface( ptrAuxSurface );
		return false;
	}
	
	textureWidth = ptrAuxSurface -> w;
	textureHeight = ptrAuxSurface -> h;
			
	SDL_FreeSurface( ptrAuxSurface );
	
	return true;
}

bool Texture::crearTexturaDesdeTexto( const char *texto, SDL_Color color, TTF_Font *fuente )
{
	// Libera la textura que exista anteriormente
	if( ptrTexture != NULL ){
		destroyTexture();
	}

	// Rasteriza el texto
	SDL_Surface *superficie = TTF_RenderText_Solid( fuente, texto, color );
	if( superficie == nullptr ){
		cout << "Ocurrió un error al crear la textura. Error: " << TTF_GetError() << endl;
		return false;
	}

	// Convierte la superficie en una textura
	ptrTexture = SDL_CreateTextureFromSurface( gPtrRenderer, superficie );
	if( ptrTexture == nullptr ){
		cout << "No pudo crearse la textura de texto. Error: " << TTF_GetError() << endl;
		SDL_FreeSurface( superficie );
		return false;
	}

	// Convierte la superficie en una textura
	textureWidth = superficie -> w;
	textureHeight = superficie -> h;

	// Elimina el residuo
	SDL_FreeSurface( superficie );

	// Returna true
	return true;
}

void Texture::setBlendMode( SDL_BlendMode blend )
{
	if( SDL_SetTextureBlendMode( ptrTexture, blend ) < 0 )
		cout << SDL_GetError() << endl;
}

void Texture::setAlphaMod( Uint8 alpha )
{
	if( SDL_SetTextureAlphaMod( ptrTexture, alpha ) < 0 )
		cout << SDL_GetError() << endl;
}

void Texture::setColorMod( SDL_Color color )
{
	if( SDL_SetTextureColorMod( ptrTexture, color.r, color.g, color.b ) < 0 )
		cout << SDL_GetError() << endl;
}

void Texture::renderTexture( SDL_DRect destRect, SDL_Rect srcRect )
{
	SDL_Rect targetRect = { (int)( destRect.x * gameUnitSize ), (int)( destRect.y * gameUnitSize ), (int)( destRect.w * gameUnitSize ), (int)( destRect.h * gameUnitSize ) };
	SDL_RenderCopy( gPtrRenderer, ptrTexture, &srcRect, &targetRect );
}

void Texture::renderTexture( SDL_Rect* srcRect, SDL_Rect* destRect )
{
	SDL_RenderCopy( gPtrRenderer, ptrTexture, srcRect, destRect );
}

void Texture::destroyTexture()
{
	SDL_DestroyTexture( ptrTexture );
	ptrTexture = NULL;
	textureWidth = 0;
	textureHeight = 0;
}

int Texture::getWidth()
{
	return textureWidth;
}

int Texture::getHeight()
{
	return textureHeight;
}
