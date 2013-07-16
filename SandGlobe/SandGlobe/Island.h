#pragma once
#include "scene_object.h"
#include "Shape_Maker.h"
struct island_buff {
	XMMATRIX worldViewProj;
	XMMATRIX worldInverseTranspose;
	XMMATRIX worldMx;
	XMMATRIX ViewMx;
	XMMATRIX lightViewProj;
	XMFLOAT4 clip_level;
};

class Island :
	public Scene_object
{
public:
	Island(void);
	~Island(void);
	void make_globe();
	void Render(const XMMATRIX& viewMX,const XMMATRIX& projMX,const XMMATRIX& lvpMx,const XMFLOAT4& cliplevel,bool multilight);
	void generate_shadow(const XMMATRIX& lightVPMx,const XMFLOAT3& lightpos, shaders& shadow_shader, ID3D11Buffer* shadowBuff);
private:
	void cleanup();
	Shadow_cBuffer shadow_data;
	ID3D11Buffer *cbBuffer; 
};

