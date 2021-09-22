#include "object.h"
#include <iostream>
using namespace std;

int main()
{
	Object object;
	const SDL_DRect * rect;
	
	if( object.loadRelativeCoordinatesFromFile( "Recursos/Coords/margin.dat" ) ){
		rect = object.getRelativeRect();
		cout << "Coordinates loaded: " << endl
		     << "X: " << rect -> x << endl
		     << "Y: " << rect -> y << endl
		     << "W: " << rect -> w << endl
		     << "H: " << rect -> h << endl;
	}
	
	return 0;
}
