#include "NewGarphics.h"

int main()
{
	NewGarphics game;
	
	if (!game.ConstructConsole(240, 160, 4, 4))
		game.Loop();

	return 0;
}