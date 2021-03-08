#include "NewGarphics.h"

int main()
{
	NewGarphics game;
	
	game.ConstructConsole(240, 140, 4, 4);
	game.Loop();

	return 0;
}