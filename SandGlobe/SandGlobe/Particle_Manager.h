#pragma once
#include <Windows.h>
#include <xnamath.h>
#include "Global.h"
#include <d3d11.h>
#include <D3DX11.h>
#include <d3dx11Effect.h>
#include "shaders.h"
#include "Particle.h"
#include "Water_Particles.h"
#include "Fog_Particles.h"
class Scene_manager;

class Particle_Manager
{
public:
	Particle_Manager(void);
	~Particle_Manager(void);
	void Initialsie(Scene_manager* _manager,ID3D11DeviceContext *_dxDC, ID3D11Device *_dxD);
	void Render_Water(const XMMATRIX& viewMX,const XMMATRIX& projMX);
	void Render_Fire(const XMMATRIX& viewMX,const XMMATRIX& projMX);
	void Render_Fog(const XMMATRIX& viewMX,const XMMATRIX& projMX);
	void Calculate_Particles(double dt,float sea_level,bool is_rising);
	void Add_Fire(const XMFLOAT3& _pos);
	void Setup_Water(const XMFLOAT3& pos,float radius);
	void Setup_Fog(const XMFLOAT3& center,float start_r, float end_r,float step_r,float step_angle);
	void destroy_Fire();
	void start_Fog();
	void stop_Fog();
private:
	//void add_fire(XMFLOAT3& position);
	void cleanup(); //cleanup all particle data
	Scene_manager *manager;

	std::vector<Particle> fires; //fire is a basic particle(no extra data require, only basics)
	std::vector<Water_Particles> waters;
	std::vector<Fog_Particles> fog;

	shaders fire_shader;
	shaders water_shader;
	shaders fog_shader;

	bool destroying_fire;
	bool destroying_fog;
	bool render_fog;

	ID3D11Device *_dxDev;
	ID3D11DeviceContext *_dxImmedDC;
};

