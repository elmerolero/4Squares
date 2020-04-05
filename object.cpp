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
	
	while( !file.eof() ){
		// Loads relative coordinates
		file >> relativeRect.x;
		file >> relativeRect.y;
		file >> relativeRect.w;
		file >> relativeRect.h;
	
		// Loads absolute coordinates
		file >> textureRect.x;
		file >> textureRect.y;
		file >> textureRect.w;
		file >> textureRect.h;
	}
	
	file.close();
	
	return true;
}

void Object::setRelativeCoords( SDL_DRect rect )
{
	relativeRect = rect;
}

void Object::setTextureCoords( SDL_Rect rect )
{
	textureRect = rect;
}

void Object::setRelativeX( double x )
{
	relativeRect.x = x;
}

void Object::setRelativeY( double y )
{
	relativeRect.y = y;
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

void Object::setAbsoluteX( int x )
{
	absoluteRect.x = x;
}

void Object::setAbsoluteY( int y )
{
	absoluteRect.y = y;
}

void Object::updateAbsCoords( void )
{
	absoluteRect.x = relativeRect.x * gameUnitSize;
	absoluteRect.y = relativeRect.y * gameUnitSize;
	absoluteRect.w = relativeRect.w * gameUnitSize;
	absoluteRect.h = relativeRect.h * gameUnitSize;
}

SDL_Rect* Object::getSrcRect( void )
{
	return &textureRect;
}

SDL_Rect* Object::getDestRect( void )
{
	return &absoluteRect;
}
