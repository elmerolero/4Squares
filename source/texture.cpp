#include "texture.h"
#include "game.h"
#include <iostream>
using namespace std;

Texture::Texture()
{
	ptrTexture = NULL;
	showTexture = true;
}

Texture::~Texture()
{
	destroyTexture();
}

bool Texture::loadFileTexture( const char* path )
{	
	// Surface for creating surface
	SDL_Surface* ptrAuxSurface = NULL;
	ptrAuxSurface = IMG_Load( path );
	if( ptrAuxSurface == NULL ){
		cout << "Error loading image. Details: " << IMG_GetError() << endl;
		return false;
	}

	if( !crearTexturaDesdeSuperficie( gPtrRenderer, ptrAuxSurface ) ){
		return false;
	}
	
	return true;
}

bool Texture::crearTexturaDesdeSuperficie( SDL_Renderer *render, SDL_Surface *superficie ){
	// Frees texture if exists another
	if( ptrTexture != NULL ){
		destroyTexture();
	}
	
	// Crea y asigna la textura desde la superficie dada
	ptrTexture = SDL_CreateTextureFromSurface( render, superficie );
	if( ptrTexture == nullptr ){
		cout << "No pudimos crear la textura. Detalles: " << SDL_GetError() << endl;
		SDL_FreeSurface( superficie );
		return false;
	}
	
	textureWidth = superficie -> w;
	textureHeight = superficie -> h;
			
	SDL_FreeSurface( superficie );
	
	return true;

}

bool Texture::crearTexturaDesdeTextoSolido( const char *texto, SDL_Color color, TTF_Font *fuente ){
	// Rasteriza el texto
	SDL_Surface *superficie = TTF_RenderText_Solid( fuente, texto, color );
	if( superficie == nullptr ){
		cout << "Ocurrió un error al crear la textura. Error: " << TTF_GetError() << endl;
		return false;
	}

	// Convierte la superficie en una textura
	if( !crearTexturaDesdeSuperficie( gPtrRenderer, superficie ) ){
		return false;
	}

	// Returna true
	return true;
}

bool Texture::crearTexturaDesdeTextoBlended( const char *texto, SDL_Color color, TTF_Font *fuente, Uint32 largoMaximo ){
	// Rasteriza el texto
	SDL_Surface *superficie = TTF_RenderUTF8_Blended_Wrapped( fuente, texto, color, largoMaximo );
	if( superficie == nullptr ){
		cout << "Ocurrió un error al crear la textura. Error: " << TTF_GetError() << endl;
		return false;
	}

	// Convierte la superficie en una textura
	if( !crearTexturaDesdeSuperficie( gPtrRenderer, superficie ) ){
		return false;
	}

	// Returna true
	return true;
}

void Texture::setBlendMode( SDL_BlendMode blend )
{
	if( SDL_SetTextureBlendMode( ptrTexture, blend ) < 0 ){
		cout << SDL_GetError() << endl;
	}
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

void Texture::crearTexturaRenderizable( SDL_Renderer *render, int ancho, int alto )
{
	// Crea la textura
	SDL_Texture *textura = SDL_CreateTexture( render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, ancho, alto );
	if( textura == nullptr ){
		cout << "Ocurrió un error al crear la textura." << endl;
		return;
	}

	// Establece las dimensiones de la textura
	textureWidth = ancho;
	textureHeight = alto;
}


void Texture::establecerTexturaRenderizado( SDL_Renderer *render )
{
	// Establece la textura
	SDL_SetRenderTarget( render, ptrTexture );
}

void Texture::show( bool show ){
	showTexture = show;
}

bool Texture::show( void )const{
	return showTexture;
}

void Texture::renderTexture( const SDL_Rect* srcRect, const SDL_Rect* destRect )
{
	if( show() ){
		SDL_RenderCopy( gPtrRenderer, ptrTexture, srcRect, destRect );
	}
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
