//  ---------------------------------------------------------------------------
//  Department of Computer Science, University of Hull
//  DirectX 11 framework, Aug 2012
//
//  ---------------------------------------------------------------------------

#pragma once
#include "dxapp.h"
#include <xnamath.h>
#include "Shape_Maker.h"
#include "Scene_manager.h"
#include "Camera.h"
#include "shaders.h"

class MyApp : public DxApp {
private:
	bool keypressed_L;
	bool keypressed_M;
	XMMATRIX _projMx;
	//Scene_manager
	Scene_manager Scene;

public:
	MyApp();
	~MyApp();
	
	void OnKey(WPARAM keyinput);
	bool initialise(HINSTANCE hInstance, int cmdShow,int w, int h);
	


	HRESULT buildFX();
	HRESULT buildGeometry();

	void renderScene();
	void updateScene(double dt);
};

