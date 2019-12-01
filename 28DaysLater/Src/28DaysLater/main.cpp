#include "App.h"

int main()
{
	static CApp App;

	if(!App.Init())
		return 0;

	App.GameLoop();
}