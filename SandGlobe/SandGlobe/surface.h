#pragma once
#include "scene_object.h"
#include "shaders.h"
#include "Shape_Maker.h"

struct Surface_buff {
	XMMATRIX worldViewProj;
	XMMATRIX viewMx;
	XMMATRIX projMx;
	XMMATRIX reflecMx;
	XMMATRIX worldMx;
	float Time;
	float WaveTime;
};


class surface :
	public Scene_object
{
public:
	surface(void);
	~surface(void);
	void initialise_shader(const char* filename);
	void create();
	void update(float sea_level,double dt, bool frozen);
	void Render(const XMMATRIX& viewMX,const XMMATRIX& projMX,const XMMATRIX& reflectMX,bool frozen, bool multilight);
	void Render_lights(const XMMATRIX& viewMX,const XMMATRIX& projMX,const XMMATRIX& reflectMX, bool frozen);
	void create_render_to_texture(int w, int h);
	ID3D11Texture2D* return_ref_texture() const;
	ID3D11ShaderResourceView* return_ref_SRview() const;
	ID3D11ShaderResourceView* return_rtv_SRview() const;
	ID3D11RenderTargetView* return_rtv() const;
private:
	void cleanup();
	ID3D11RenderTargetView * rtv_render_to_texture;
	ID3D11Texture2D * reflection_texture;
	ID3D11ShaderResourceView* reflect_SRview;
	ID3D11ShaderResourceView * srv_render_to_texture;

	shaders surface_sh;
	ID3DX11EffectTechnique* ice_technique;

	ID3D11ShaderResourceView* sky_texture;
	ID3D11Buffer *surface_Buffer; 
	ID3D11ShaderResourceView* ice_texture;
	XMMATRIX surfaceMx;
	ID3D11SamplerState *seareflect_Sampler;


	float time;
	float Wavetime;
	//ID3D11SamplerState *sky_Sampler;
};

