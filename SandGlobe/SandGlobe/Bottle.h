#pragma once
#include "scene_object.h"
#include "Shape_Maker.h"
struct bottle_buff {
	XMMATRIX worldViewProj;
	XMMATRIX worldInverseTranspose;
	XMMATRIX worldMx;
	XMFLOAT4 clip_level;
};
class Bottle : public Scene_object
{
public:
	Bottle(void);
	~Bottle(void);
	void Render(const XMMATRIX& viewMX,const XMMATRIX& projMX, const XMFLOAT4& cliplevel,bool multi_light);
	/*void Render_lights(XMMATRIX& viewMX, XMMATRIX& projMX, shaders& _sh, XMFLOAT4& cliplevel);*/
	void create();
private:
	void cleanup();
	bottle_buff buff;
	ID3D11Buffer *cbBuffer; 
};

