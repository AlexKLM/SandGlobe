#include "Season.h"


Season::Season(void)
{
	current = 0;
}


Season::~Season(void)
{
}


void Season::activate()
{
	current = duration;
}

void Season::initialize(XMFLOAT3 &_sun_pos, float _duration,bool _seafreeze,bool burn_leaves, bool grow_leaves, float sun_intensity,Season* next)
{
	sun_pos = _sun_pos;
	duration = _duration;
}

void Season::update(double time)
{
	if(current>0)
	{
		current -= time;
	}
	else
	{

	}
}

void Season::reinitialize()
{
	current = 0;
}