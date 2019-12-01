#pragma once
#include "irrlicht.h"

enum ETOUCH_EVENT_TYPE 
{
	ETOUCH_BEGAN,
	ETOUCH_MOVE,
	ETOUCH_END,
	ETOUCH_DOUBLE_TOUCH,
	ETOUCH_TOUCHCANCEL,
	ETOUCH_NOTOUCH
};

struct STouchEvent
{
	ETOUCH_EVENT_TYPE type;
	float pointX;
	float pointY;
};
