#include <iostream>
#include "Game.h"

int main()
{
	Game* game = Game::GetInstance();
	game->Start();
}
