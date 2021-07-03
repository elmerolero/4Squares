#include "object.h"
#include "game.h"
#include <iostream>
#include <fstream>
using namespace std;

Object::Object()
{
	textureRect  = { 0, 0, 0, 0 };
	absoluteRect = { 0, 0, 0, 0 };
	relativeRect = { 0.0, 0.0, 0.0, 0.0 };
}

Object::~Object()
{
	; // Empty
}

bool Object::loadCoordinatesFromFile( const char * path )
{
	// File descriptor
	ifstream file;
	
	file.open( path );
	if( !file.is_open() ){
		cout << "Error opening coordinates file!" << endl;
		return false;
	}
	
	//
	SDL_DRect rRect;
	SDL_Rect  tRect; 

	while( !file.eof() ){
		// Loads relative coordinates
		file >> rRect.x;
		file >> rRect.y;
		file >> rRect.w;
		file >> rRect.h;
	
		// Loads absolute coordinates
		file >> tRect.x;
		file >> tRect.y;
		file >> tRect.w;
		file >> tRect.h;
	}
	file.close();
	
	// Establece las coordenadas
	setRelativeCoords( rRect );
	setTextureCoords( tRect );

	return true;
}

void Object::setRelativeCoords( SDL_DRect rect )
{
	// Establece las coordenadas relativas
	relativeRect = rect;

	// Actualiza las coordenas absolutas de acuerdo a los nuevos parametros dados
	absoluteRect.x = relativeRect.x * gameUnitSize;
	absoluteRect.y = relativeRect.y * gameUnitSize;
	absoluteRect.w = relativeRect.w * gameUnitSize;
	absoluteRect.h = relativeRect.h * gameUnitSize;
}

void Object::setTextureCoords( SDL_Rect rect )
{
	textureRect = rect;
}

void Object::setRelativeX( double x )
{
	relativeRect.x = x;
	absoluteRect.x = relativeRect.x * gameUnitSize;
}

void Object::setRelativeY( double y )
{
	relativeRect.y = y;
	absoluteRect.y = relativeRect.y * gameUnitSize;
}

double Object::getRelativeX()
{
	return relativeRect.x;
}

double Object::getRelativeY()
{
	return relativeRect.y;
}

double Object::getRelativeW()
{
	return relativeRect.w;
}

double Object::getRelativeH()
{
	return relativeRect.h;
}

int Object::getAbsoluteX()
{
	return absoluteRect.x;
}

int Object::getAbsoluteY()
{
	return absoluteRect.y;
}

int Object::getAbsoluteW()
{
	return absoluteRect.w;
}

int Object::getAbsoluteH()
{
	return absoluteRect.h;
}

void Object::setAbsoluteX( int x )
{
	absoluteRect.x = x;
}

void Object::setAbsoluteY( int y )
{
	absoluteRect.y = y;
}

SDL_Rect* Object::getSrcRect( void )
{
	return &textureRect;
}

SDL_Rect* Object::getDestRect( void )
{
	return &absoluteRect;
}

void Object::actualizarCoordanadasAbsolutas( void )
{
	absoluteRect.x = relativeRect.x * gameUnitSize;
	absoluteRect.y = relativeRect.y * gameUnitSize;
	absoluteRect.w = relativeRect.w * gameUnitSize;
	absoluteRect.h = relativeRect.h * gameUnitSize;
}