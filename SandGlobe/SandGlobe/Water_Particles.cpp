#include "Water_Particles.h"
#include "Scene_manager.h"

Water_Particles::Water_Particles(void)
{

}


Water_Particles::~Water_Particles(void)
{
}

void Water_Particles::Calculate_Particles(double dt, shaders& _sh, float sea_level, bool isrising)
{
	cal_cBuffer temp_cdata = Particle::get_c_data();
	if(isrising)
	{
		temp_cdata.shouldemit = 1;
	}
	else
	{
		temp_cdata.shouldemit = -1;
	}
	XMFLOAT3 pos = Particle::get_pos();
	pos.y = sea_level;
	temp_cdata.emit_pos = pos;
	Particle::set_pos(pos);
	Particle::set_c_data(temp_cdata);
	Particle::Calculate_Particles(dt,_sh,temp_cdata);
}

