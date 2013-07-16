//  ---------------------------------------------------------------------------
//  Department of Computer Science, University of Hull
//  DirectX 11 framework, Aug 2012
//
//  ---------------------------------------------------------------------------

#pragma once
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

#include "Global.h"
#include <d3d11.h>
#include <d3dx11Effect.h>
#include <dxerr.h>
#include <xnamath.h>
#include <cassert>
#include <vector>

class DxApp {
private:
	static DxApp* _instance;
	HINSTANCE _hApp;
	HWND _hWnd;
	
protected:
	int width,height;
	ID3D11Device *_dxDev;
	ID3D11DeviceContext *_dxImmedDC;
	IDXGISwapChain *_swapChain;
	DXGI_SWAP_CHAIN_DESC _swapChainDesc;
	


	ID3D11RenderTargetView *_renderTargetView;
	ID3D11DepthStencilView *_depthStencilView;
	D3D11_TEXTURE2D_DESC _depthStencilDesc;
	
private:
	
	LARGE_INTEGER counter;
	double Counts_ps;
	__int64 frameTimeOld;
	double frameTime;
	_int64 start;
	_int64 counter_start;
	_int64 stop;

	static LRESULT CALLBACK MessageProc(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam);
	
	HRESULT initialiseDxDevice();
	HRESULT createDepthStencilView();
	HRESULT createRenderTargetView();
	void setupViewport();
	void startTimer();
	double getFPS();
	double getdt();
	bool vsync();
	void destroy();
protected:
	bool loadEffects(const char* filename, std::vector<char> &compiledShader) const;

public:
	DxApp();
	virtual ~DxApp();
	virtual void OnKey(WPARAM keyinput);
	virtual bool initialise(HINSTANCE instance, int cmdShow,int w, int h);
	int run();

	virtual void renderScene() = 0;
	virtual void updateScene(double dt) = 0;


	virtual HRESULT onResize(int _width, int _height);

	static HINSTANCE hApp() { assert(_instance); return _instance->_hApp; }
	static HWND hWnd() { assert(_instance); return _instance->_hWnd; }
};


//CANNOT FIX THESE PARASOFT ERRORS

// Debug trace macro for DirectX
#ifdef _DEBUG
	#ifndef HR
		#define DXCHECK(x)										\
		{														\
			HRESULT hr = (x);									\
			if (FAILED(hr)) {									\
				DXTrace(__FILE__,__LINE__, hr, L#x, true);		\
			}													\
		}
	#endif
#else
	#ifndef HR
		#define DXCHECK(x) (x)
	#endif
#endif

// Debug trace macro for DirectX
#ifdef _DEBUG
	#ifndef HRRETURN
		#define DXRETURN(x)										\
		{														\
			HRESULT hr = (x);									\
			if (FAILED(hr)) {									\
				DXTrace(__FILE__,__LINE__, hr, L#x, true);		\
				return hr;										\
			}													\
		}
	#endif
#else
	#ifndef HR
		#define DXRETURN(x)										\
		{														\
			HRESULT hr = (x);									\
			if (FAILED(hr)) {									\
				return hr;										\
			}													\
		}
	#endif
#endif