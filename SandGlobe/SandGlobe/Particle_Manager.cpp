#include "Particle_Manager.h"


Particle_Manager::Particle_Manager(void)
{
	manager = NULL;
	_dxDev = NULL;
	_dxImmedDC = NULL;
	destroying_fire = false;
	destroying_fog = false;
	render_fog = false;
}

Particle_Manager::~Particle_Manager(void)
{
	try
	{
		cleanup();
	}
	catch(...)
	{
	}
}

void Particle_Manager::cleanup()
{
	for(int i = 0; i < waters.size();i++)
	{
		waters[i].destroy();
	}
	for(int i = 0 ; i < fog.size(); i++)
	{
		fog[i].destroy();
	}
	for(int i = 0; i < fires.size();i++)
	{
		fires[i].destroy();
	}
}

void Particle_Manager::Initialsie(Scene_manager* _manager,ID3D11DeviceContext *_dxDC, ID3D11Device *_dxD)
{
	manager = _manager;
	_dxImmedDC = _dxDC;
	_dxDev = _dxD;

	water_shader.loadfx("fx/Water_P_Effect.fxo");
	water_shader.load_particle_effect(_dxDev,_dxImmedDC);

	fire_shader.loadfx("fx/Fire_P_Effect.fxo");
	fire_shader.load_particle_effect(_dxDev,_dxImmedDC);

	fog_shader.loadfx("fx/Fog_P_Effect.fxo");
	fog_shader.load_particle_effect(_dxDev,_dxImmedDC);
}

void Particle_Manager::Setup_Water(const XMFLOAT3& _pos,float radius)
{
	for(int i = 0; i < 360 ; i++)
	{
		Water_Particles particle;
		XMFLOAT3 pos;
		pos.x = _pos.x + radius * cos(i * XM_PI / 180);
		pos.y = _pos.y;
		pos.z = _pos.z + radius * sin(i * XM_PI / 180);
		//particle.initialize(manager,XMFLOAT3(0,0,0),_dxImmedDC,_dxDev,pos);
		particle.initialize(manager,_dxImmedDC,_dxDev,pos,1.5f);
		
		particle.set_gravity(XMFLOAT4(0,-1,0,0));
		waters.push_back(particle);
	}
	for(int i = 0; i < waters.size();i++)
	{
		waters[i].create_buffers();
		waters[i].set_particle_texture(L"media\\particle_tex.png");		
		waters[i].generate_randoms();
	}
}

void Particle_Manager::Setup_Fog(const XMFLOAT3& center,float start_r, float end_r,float step_r,float step_angle)
{
	float r = start_r;
	//for(float r = start_r; r > end_r; r -= step_r)
	while(r > end_r)
	{
		for(int i = 0; i < 360 ; i += step_angle)
		{
			Fog_Particles particle;
			XMFLOAT3 pos;
			pos.x = center.x + r * cos(i * XM_PI / 180);
			pos.y = center.y;
			pos.z = center.z + r * sin(i * XM_PI / 180);
			particle.initialize(manager,_dxImmedDC,_dxDev,pos,35);
			particle.set_gravity(XMFLOAT4(0,-1,0,0));
			fog.push_back(particle);
		}
		step_angle+=5;
		r -=step_r;
	}
	for(int i = 0; i < fog.size();i++)
	{
		fog[i].create_buffers();
		fog[i].set_particle_texture(L"media\\smoke_particle.png");	
		fog[i].generate_randoms();
	}
}

void Particle_Manager::Add_Fire(const XMFLOAT3& _pos)
{
	Particle particle;
	particle.initialize(manager,_dxImmedDC,_dxDev,_pos,1.5f);
	particle.set_gravity(XMFLOAT4(0,-1,0,0));
	fires.push_back(particle);

	int i = fires.size() - 1;
	fires[i].create_buffers();
	fires[i].generate_randoms();
	fires[i].set_particle_texture(L"media\\particle_tex.png");
}

void Particle_Manager::Calculate_Particles(double dt,float sea_level,bool is_rising)
{
	for(int i = 0; i < waters.size(); i++)
	{
		waters[i].Calculate_Particles(dt,water_shader,sea_level,is_rising);
	}
	if(!fires.empty())
	{
		if(destroying_fire)
		{
			bool all_dead = true;
			for(int i = 0; i < fires.size();i++)
			{
				if(!fires[i].is_die_ready())
				{
					fires[i].Calculate_Particles(dt,fire_shader,false);
					all_dead = false;
				}
			}
			if(all_dead)
			{
				fires.clear();
				destroying_fire = false;
			}
		}
		else
		{
			for(int i = 0; i < fires.size();i++)
			{

				fires[i].Calculate_Particles(dt,fire_shader,true);

			}
		}
	}

	if(render_fog)
	{
		if(destroying_fog)
		{
			bool all_dead = true;
			for(int i = 0; i < fog.size();i++)
			{
				if(!fog[i].is_die_ready())
				{
					fog[i].Calculate_Particles(dt,fog_shader,false,sea_level);
					all_dead = false;
				}
			}
			if(all_dead)
			{
				render_fog = false;
			}
		}
		else
		{
			for(int i = 0; i < fog.size();i++)
			{

				fog[i].Calculate_Particles(dt,fog_shader,true,sea_level);

			}
		}
	}
}

void Particle_Manager::Render_Fire(const XMMATRIX& viewMX,const XMMATRIX& projMX)
{
	if(!fires.empty())
	{
		for(int i = 0; i < fires.size();i++)
		{
			fires[i].Render(viewMX,projMX,fire_shader);
		}
	}
}

void Particle_Manager::Render_Water(const XMMATRIX& viewMX,const XMMATRIX& projMX)
{
	for(int i = 0; i < waters.size(); i++)
	{
		waters[i].Render(viewMX,projMX,water_shader);
	}
}

void Particle_Manager::Render_Fog(const XMMATRIX& viewMX,const XMMATRIX& projMX)
{
	if(render_fog)
	{
		for(int i = 0; i < fog.size(); i++)
		{
			fog[i].Render(viewMX,projMX,fog_shader);
		}
	}
}

void Particle_Manager::start_Fog()
{
	for(int i = 0; i < fog.size();i++)
	{
		fog[i].start_Fog();
	}
	destroying_fog = false;
	render_fog = true;
}

void Particle_Manager::stop_Fog()
{
	for(int i = 0; i < fog.size();i++)
	{
		fog[i].stop_Fog();
	}
	destroying_fog = true;
}

void Particle_Manager::destroy_Fire()
{
	//fires.clear();
	destroying_fire = true;
	for(int i = 0; i < fires.size(); i++)
	{
		fires[i].set_delete();
	}
}