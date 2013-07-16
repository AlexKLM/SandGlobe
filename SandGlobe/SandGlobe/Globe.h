#pragma once
#include "scene_object.h"
#include "Shape_Maker.h"

class Globe :
	public Scene_object
{
public:
	void Render(const XMMATRIX& viewMX,const XMMATRIX& projMX,bool multilights);
	void Render_lights(const XMMATRIX& viewMX,const XMMATRIX& projMX, shaders& _sh);
	void create_globe();
	Globe(void);
	~Globe(void);

private:
							
	//ID3D11BlendState *transperency_blendState;
	Shape_Maker sm;
};

