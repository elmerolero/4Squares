#ifndef GAME_STATE_H
#define GAME_STATE_H

class GameState
{
	public:
		virtual void gameStateEvents() = 0;
		virtual void gameStateLogic() = 0;
		virtual void gameStateRender() = 0;
};

#endif
