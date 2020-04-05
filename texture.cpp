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
	// Status flag
	bool success = true;
	
	// Frees texture if exists another
	if( ptrTexture != NULL ){
		destroyTexture();
	}
	
	// Surface for creating surface
	SDL_Surface* ptrAuxSurface = NULL;
	ptrAuxSurface = IMG_Load( path );
	if( ptrAuxSurface == NULL ){
		cout << "Error loading image. Details: " << IMG_GetError() << endl;
		success = false;
	}
	else{
		ptrTexture = SDL_CreateTextureFromSurface( gPtrRenderer, ptrAuxSurface );
		if( ptrTexture == NULL ){
			cout << "We could not create the texture. Details: " << SDL_GetError() << endl;
			success = false;
		}
		else{
			textureWidth = ptrAuxSurface -> w;
			textureHeight = ptrAuxSurface -> h;
			
			SDL_FreeSurface( ptrAuxSurface );
		}
	}
	
	return success;
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
