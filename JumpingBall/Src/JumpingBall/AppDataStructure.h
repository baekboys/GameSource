#ifndef APP_DATA_STRUCTURE_H
#define APP_DATA_STRUCTURE_H

#include "irrlicht.h"

struct SMapData
{
	irr::core::stringc map_fileName;
	irr::f32 map_gravity_value;
	irr::f32 map_time;
	irr::u32 map_startBallType;
	irr::video::SColor map_background_color;
	irr::core::stringc map_bgm_fileName;
};

struct SProfile
{
	irr::u32 clear_stage;
};

struct SSignalData
{
	irr::u32 type;
	irr::core::array<irr::f32> fValue;
};

#endif