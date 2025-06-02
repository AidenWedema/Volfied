#include <iostream>
#include "Game.h"

#include "Hitbox.h"

int main()
{
	Game* game = Game::GetInstance();
	game->Start();
}