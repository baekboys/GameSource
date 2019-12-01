#include "IrrlichtApp.h"

int main()
{
	IrrlichtApp irrlichtApp;
	irrlichtApp.setWorkingDirectory("../..");
	irrlichtApp.initIrrlicht();
	irrlichtApp.gameLoop();
}