	#pragma once
#include "scene_object.h"
#include "Shape_Maker.h"

class Globe_Base :
public Scene_object
{
public:
	Globe_Base(void);
	~Globe_Base(void);
	void Render(const XMMATRIX& viewMX,const XMMATRIX& projMX);//base needs to be rendered specially(with texture)
	void create_Base();

private:
	ID3D11ShaderResourceView* Base_tex;
	ID3D11SamplerState *Base_Sampler;
};

