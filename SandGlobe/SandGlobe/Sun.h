#pragma once
#include "scene_object.h"
#include "Shape_Maker.h"

struct sun_buff {
	XMMATRIX worldViewProj;
	XMMATRIX worldMx;
	XMFLOAT4 clip_level;
};


class Sun :
	public Scene_object
{
public:
	Sun(void);
	~Sun(void);
	void create();
	void update(XMFLOAT2 &sun_pos);
	void Render(const XMMATRIX& viewMX,const XMMATRIX& projMX,const XMFLOAT4& cliplevel);
	void initialise(Scene_manager* _manager,XMFLOAT3& _position,ID3D11DeviceContext *_dxDC, ID3D11Device *_dxD);
private:
	sun_buff buff;
};

