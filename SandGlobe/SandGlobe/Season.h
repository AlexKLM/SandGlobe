#pragma once
#include "Global.h"
class Season
{
public:
	Season(void);
	~Season(void);
	virtual void initialize(XMFLOAT3 &_sun_pos, float _duration,bool _seafreeze,bool burn_leaves, bool grow_leaves, float sun_intensity,Season* next);
	virtual void update(double time);
	virtual void activate();
	virtual void reinitialize();
private:
	Season* next_season;
	XMFLOAT3 sun_pos;
	bool grow_leafs;
	bool burn_leafs;
	bool freeze_sea;
	float sun_intensity;
	bool season_over;
	float duration;
	float current;

};

