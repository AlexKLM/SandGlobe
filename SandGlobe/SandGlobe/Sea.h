#pragma once
#include "scene_object.h"
#include "Shape_Maker.h"


struct plightbuff{
	pointlight plight;
};

struct Sea_buff{
	XMMATRIX worldViewProj;
	XMMATRIX worldMx;
	float sea_level;
	float WaveTime;
};

class Sea :


public Scene_object
{



public:
	Sea(void);
	~Sea(void);
	void create_sea();
	float return_sealevel() const;
	void update(float sealevel, double dt, bool frozen);
	void Render(const XMMATRIX& viewMX,const XMMATRIX& projMX);
private:
	void cleanup();
	float sea_level;
	float waveTime;
	ID3D11Buffer *sea_Buffer;
};

