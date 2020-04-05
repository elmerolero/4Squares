#include <cstdlib>
#include <iostream>
using namespace std;
#include "game.h"
#include "tetroblock.h"

int main( int argc, char * args[] )
{
	if( !init() ){
		exit( EXIT_FAILURE );
	}
	else{
		while( gActualState != GAME_STATE_EXIT ){
			gGameState -> gameStateEvents();
			gGameState -> gameStateLogic();
			gGameState -> gameStateRender();
		}
	}
	
	return 0;
}
