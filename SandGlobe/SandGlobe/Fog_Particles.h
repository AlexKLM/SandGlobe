#pragma once
#include "Global.h"
#include "shaders.h"
#include <time.h>
#include <xnamath.h>
#include <d3d11.h>
#include <D3DX11.h>
#include <d3dx11Effect.h>
#include "Particle.h"
class Scene_manager;

struct PVdata{
	float sea_level;
	XMFLOAT3 padding;
};

class Fog_Particles : public Particle
{
public:
	Fog_Particles(void);
	~Fog_Particles(void);
	void initialize(Scene_manager* _manager,ID3D11DeviceContext *_dxDC, ID3D11Device *_dxD,const XMFLOAT3& _position,float final_fog_life);
	void Calculate_Particles(double dt, shaders& _sh,bool _shouldemit, float _sea_level);
	void Render(const XMMATRIX& viewMX,const XMMATRIX& projMX, shaders& _sh);
	void create_buffers();
	void start_Fog();
	void stop_Fog();

private:

	float current_time;
	float sea_level;


	PVdata p_pv_data;
	ID3D11Buffer* p_pvBuffer;
};

