#pragma once
#include "Particle.h"

class Scene_manager;


class Water_Particles: public Particle
{
public:
	Water_Particles(void);
	~Water_Particles(void);
	void Calculate_Particles(double dt, shaders& _sh, float sea_level, bool isrising);

};

