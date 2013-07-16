//  ---------------------------------------------------------------------------
//  Department of Computer Science, University of Hull
//  DirectX 11 framework, Aug 2012
//
//  ---------------------------------------------------------------------------

#include "MyApp.h"

//struct Vertex {
//	XMFLOAT3 position;
//	XMFLOAT4 color;
//};

struct ShaderConstants {
	XMMATRIX worldViewProjection;
};


MyApp::MyApp() {
	XMMATRIX I = XMMatrixIdentity();
	keypressed_L = false;
	keypressed_M = false;
	_projMx = I;
}

MyApp::~MyApp() {

}

bool MyApp::initialise(HINSTANCE hInstance, int cmdShow,int w, int h) {
	width = w;
	height = h;
	if (!DxApp::initialise(hInstance, cmdShow,w,h))
		return false;

	Scene.initialize(_dxDev,_dxImmedDC, _renderTargetView,_depthStencilView);
	Scene.setup_render_to_texture(w,h);
	// Build FXs
	if (FAILED(buildFX())) {
		MessageBox(hWnd(), L"Cannot create effects", L"Error", MB_OK|MB_ICONERROR);
		return false;
	}

	// Build Geometry
	if (FAILED(buildGeometry())) {
		MessageBox(hWnd(), L"Cannot create geometry", L"Error", MB_OK|MB_ICONERROR);
		return false;
	}



	//sandglobe.initialise(XMFLOAT3(0,0,0),_dxImmedDC,_dxDev,d_states,b_states);
	//sandglobe.create_globe();
	////sandglobe.bind_effect(_technique);
	//island.initialise(XMFLOAT3(0,0,0),_dxImmedDC,_dxDev,d_states,b_states);
	//island.make_globe();
	////island.bind_effect(_technique);
	//sea.initialise(XMFLOAT3(0,0,0),_dxImmedDC,_dxDev,d_states,b_states);
	//sea.create_sea();

	//base.initialise(XMFLOAT3(0,-0.8f,0),_dxImmedDC,_dxDev,d_states,b_states);
	//base.create_Base();

	//Tree.initialise(XMFLOAT3(0,0,0),_dxImmedDC,_dxDev,d_states,b_states);
	//Tree.create();

	//sun.initialise(XMFLOAT3(3,3,0),_dxImmedDC,_dxDev,d_states,b_states);
	//sun.create();
	return true;
}

HRESULT MyApp::buildFX() {
	

	Scene.initialize_effect();

	return S_OK;
}

float movespeed =0.05f;
float rotspeed = 2.0f;
void MyApp::OnKey(WPARAM keyinput)
{
	bool Shift = (GetAsyncKeyState(VK_LSHIFT)<0);

	switch (keyinput)
	{
	case VK_LEFT:
		if (Shift)
		{
			Scene.rotatecam(XMFLOAT2(-rotspeed * XM_PI/180,0));
			//gameCam.rotate(XMFLOAT2(-rotspeed * XM_PI/180,0));
		}
		else
		{

			Scene.movecam(XMFLOAT3(-rotspeed * XM_PI/180,0,0));
			//gameCam.move(XMFLOAT3(-rotspeed * XM_PI/180,0,0));
			//X_movement -= movespeed;
			//Camera_dir.x -= movespeed;
		}
		break;
	case VK_RIGHT:
		if(Shift)
		{
			Scene.rotatecam(XMFLOAT2(rotspeed * XM_PI/180,0));
			//gameCam.rotate(XMFLOAT2(rotspeed * XM_PI/180,0));
		}
		else
		{
			Scene.movecam(XMFLOAT3(rotspeed * XM_PI/180,0,0));
			//gameCam.move(XMFLOAT3(rotspeed * XM_PI/180,0,0));
		}
		break;
	case VK_UP:
		if(Shift)
		{
			Scene.rotatecam(XMFLOAT2(0,-rotspeed*XM_PI/180));
			//gameCam.rotate(XMFLOAT2(0,-rotspeed*XM_PI/180));
		}
		else
		{
			Scene.movecam_z(movespeed);
			//gameCam.move_z(movespeed);
		}
		break;
	case VK_DOWN:
		if(Shift)
		{
			Scene.rotatecam(XMFLOAT2(0,rotspeed*XM_PI/180));
			//gameCam.rotate(XMFLOAT2(0,rotspeed*XM_PI/180));
		}
		else
		{
			Scene.movecam_z(movespeed);
			//gameCam.move_z(-movespeed);
		}
		break;
	case 0x46:
		Scene.Stop_fog();
		break;
	case 0x4C:
		if(!keypressed_L)
		{
			Scene.toggle_multi();
			keypressed_L = true;
		}
		break;
	case VK_F1:
		Scene.globe_view();
		break;
	case VK_F2:
		Scene.tree_view();
		break;
	case VK_F3:
		if(GetAsyncKeyState(VK_F3)<0)
		{
			Scene.leaf_view();
		}
		break;
	case VK_F4:
		Scene.bottle_view();
		break;
	case VK_F5:
		Scene.sea_view();
		break;
	case VK_ADD:
		Scene.modify_speedmod(0.2f);
		break;
	case VK_SUBTRACT:
		if(GetAsyncKeyState(VK_SUBTRACT)<0)
		{
			Scene.modify_speedmod(-0.2f);
		}
		break;
	case 0x52:
		Scene.reset_speedmod();
		break;
	case 0x4D:
		if(!keypressed_M)
		{
			Scene.change_tree_state();
			keypressed_M = true;
		}
		break;
	}
	keypressed_L = (GetAsyncKeyState(0x4C)<0);
	keypressed_M = (GetAsyncKeyState(0x4D)<0);
	DxApp::OnKey(keyinput);
}


HRESULT MyApp::buildGeometry() {

	Scene.create_gemoitry();
	return S_OK;
}

void MyApp::updateScene(double dt) {

	Scene.update(dt);
}

void MyApp::renderScene() {
	// Clear the back buffer 


	_projMx = XMMatrixPerspectiveFovLH((float)3.1415 / 4.0f, (float)width/height, 0.1f, 1000.0f);
	
	Scene.render(_projMx);

	
	_swapChain->Present(0, 0);
} ;

