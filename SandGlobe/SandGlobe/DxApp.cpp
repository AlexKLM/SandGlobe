//  ---------------------------------------------------------------------------
//  Department of Computer Science, University of Hull
//  DirectX 11 framework, Aug 2012
//
//  ---------------------------------------------------------------------------

#include "DxApp.h"
#include <fstream>

DxApp* DxApp::_instance = NULL;

DxApp::DxApp() {

	counter.LowPart = 0;
	counter.HighPart = 0;
	frameTimeOld = 0;
	frameTime = 0;
	start = 0;
	counter_start =  0;
	stop = 0;
	Counts_ps = 0;
	width = 800;
	height = 600;
	_hApp = 0;
	_hWnd = 0;
	_instance = this;

	_dxDev = NULL;
	_dxImmedDC = NULL;
	_swapChain = NULL;
	_renderTargetView = NULL;
	_depthStencilView = NULL;
}

DxApp::~DxApp() {
	try
	{
		destroy();
	}
	catch(...)
	{

	}
}

void DxApp::destroy()
{
	if (_dxImmedDC) 
	{
		(_dxImmedDC->ClearState());
		
	}
	if (_renderTargetView) _renderTargetView->Release();
	if (_depthStencilView) _depthStencilView->Release();
	
	if (_swapChainDesc.Windowed) _swapChain->Release();
	if (_dxImmedDC) _dxImmedDC->Release();
	if (_dxDev) _dxDev->Release();

	_instance = NULL;
}


void DxApp::startTimer() 
{
	//LARGE_INTEGER counter;
	QueryPerformanceFrequency( &counter );
	Counts_ps = double(counter.QuadPart);
	QueryPerformanceCounter(&counter);
	start = counter.QuadPart;
	counter_start = counter.QuadPart;
}

double DxApp::getdt()
{
	LARGE_INTEGER current_time;
	QueryPerformanceCounter(&current_time);
	double dt = (current_time.QuadPart-counter_start)/Counts_ps;
	counter_start = current_time.QuadPart;
	return dt;
}

double DxApp::getFPS()
{
	LARGE_INTEGER current_Time;
	__int64 tickCount;
	QueryPerformanceCounter(&current_Time);

	tickCount = current_Time.QuadPart-frameTimeOld;
	frameTimeOld = current_Time.QuadPart;

	if(tickCount < 0.0f)
		tickCount = 0.0f;

	return float(tickCount)/Counts_ps;
}
bool DxApp::vsync()
{
	QueryPerformanceCounter(&counter);
	stop = counter.QuadPart;
	double ellapsedtime = double((stop-start)*1000)/Counts_ps;
	if(ellapsedtime > 1000/60)
	{
		QueryPerformanceCounter(&counter);
		start = counter.QuadPart;
		return true;
	}
	else
	{
		return false;
	}
}

bool DxApp::initialise(HINSTANCE hInstance, int cmdShow,int w,int h) {
	_hApp = hInstance;
	
	// Register our window class
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_HREDRAW|CS_VREDRAW, MessageProc,
		0L, 0L, _hApp, NULL, NULL, NULL, NULL, L"SandGlobe", NULL };
	RegisterClassEx(&wcex);

	// Create a window
	RECT rc = { 0, 0, w, h };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	_hWnd = CreateWindow(L"SandGlobe", L"SandGlobe?", 
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
		rc.right-rc.left, rc.bottom-rc.top, NULL, NULL, _hApp, NULL);
	if (!_hWnd) {
		MessageBox(NULL, L"Cannot create window", L"Error", MB_OK|MB_ICONERROR);
		return false;
	}

	// Initialize D3D11
	if (FAILED(initialiseDxDevice())) {
		MessageBox(_hWnd, L"Cannot create D3D11 device", L"Error", MB_OK|MB_ICONERROR);
		return false;
	}

	ShowWindow(_hWnd, cmdShow);
	UpdateWindow(_hWnd);
	startTimer();
	return true;
}


// Main message loop
int DxApp::run() {

	MSG msg = {0};
	while (WM_QUIT != msg.message) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			bool time_to_update = vsync();
			if(time_to_update)
			{
			updateScene(getdt());
			renderScene();
			}
		}
	}
	return (int)msg.wParam;
}



// Called every time the application receives a message
LRESULT CALLBACK DxApp::MessageProc(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam) {
	assert (_instance);

	switch (message) {

	case WM_SIZE: 
		_instance->onResize(LOWORD(lParam), HIWORD(lParam));
		return 0;

	case WM_CHAR:
		_instance->OnKey(wParam);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		_instance->OnKey(wParam);
		return DefWindowProc(wnd, message, wParam, lParam);
	}
}

void DxApp::OnKey(WPARAM keyinput) //CAN'T MODIFY THIS FOR PARASOFT, WILL BREAK KEYBOARD FUNCTION
{
	switch(keyinput)
	{
	case VK_ESCAPE:
		PostQuitMessage(0);
		return;
	}

}
// Create Direct3D device and swap chain
HRESULT DxApp::initialiseDxDevice() {
	// Get window size
	RECT rc;
	GetClientRect(_hWnd, &rc);
	width = rc.right - rc.left;
	height = rc.bottom - rc.top;

	// Create D3D11 device and swap chain
	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	ZeroMemory(&_swapChainDesc, sizeof(_swapChainDesc));
	_swapChainDesc.BufferDesc.Width = width;
	_swapChainDesc.BufferDesc.Height = height;
	_swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	_swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	_swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	_swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	_swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	// No multisampling
	_swapChainDesc.SampleDesc.Count = 1;
	_swapChainDesc.SampleDesc.Quality = 0;

	_swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	_swapChainDesc.BufferCount = 1;
	_swapChainDesc.OutputWindow = _hWnd;
	_swapChainDesc.Windowed = true;
	_swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	_swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// Try to create a hardware accelerated device
	const D3D_FEATURE_LEVEL featureLevel[] = {D3D_FEATURE_LEVEL_10_0};
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL, // Primary graphics adapter
		D3D_DRIVER_TYPE_HARDWARE, // Use graphics hardware
		NULL, // Use hardware rasteriser
		createDeviceFlags, // Debug flags
		featureLevel, // Feature level
		1, // Elements in the feature level
		D3D11_SDK_VERSION, // SDK version
		&_swapChainDesc, // Description of the swap chain
		&_swapChain, // Returns the created swap chain
		&_dxDev, // Returns the created device
		NULL, // Returns feature level
		&_dxImmedDC // Returns the Device Context 
		);

	if (FAILED(hr)) {
		// If failed, try to create a reference device
		DXRETURN(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_REFERENCE, NULL, createDeviceFlags, 
			NULL, 0, D3D11_SDK_VERSION, &_swapChainDesc, &_swapChain, &_dxDev, NULL, &_dxImmedDC));
		
		MessageBox(_hWnd, L"No DX10 hardware acceleration found.\nSwitching to REFERENCE driver (very slow).", L"Warning", MB_OK|MB_ICONWARNING);
	}

	// Create a depth - stencil view
	_depthStencilDesc.Width = width;
	_depthStencilDesc.Height = height;
	_depthStencilDesc.MipLevels = 1;
	_depthStencilDesc.ArraySize = 1;
	_depthStencilDesc.Format = DXGI_FORMAT_D16_UNORM;
	_depthStencilDesc.SampleDesc = _swapChainDesc.SampleDesc;
	_depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	_depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	_depthStencilDesc.CPUAccessFlags = 0;
	_depthStencilDesc.MiscFlags = 0;



	DXRETURN( createDepthStencilView() );

	// Create a render target view
	DXRETURN( createRenderTargetView() );

	// Bind the views to the output
	_dxImmedDC->OMSetRenderTargets(1, &_renderTargetView, _depthStencilView);

	// Setup viewport
	setupViewport();

	return S_OK;
}

HRESULT DxApp::onResize(int _width, int _height) {
	if (!_dxDev)
		return S_FALSE;

	// Release render target and depth-stencil view
	ID3D11RenderTargetView *nullRTV = NULL;
	_dxImmedDC->OMSetRenderTargets(1, &nullRTV, NULL);
	
	if (_depthStencilView) {
		_depthStencilView->Release();
		_depthStencilView = NULL;
	}

	if (_renderTargetView) {
		_renderTargetView->Release();
		_renderTargetView = NULL;
	}

	// Resize swap chain
	_swapChainDesc.BufferDesc.Width = _width;
	_swapChainDesc.BufferDesc.Height = _height;
	if (_swapChain) 
		_swapChain->ResizeBuffers(_swapChainDesc.BufferCount, _swapChainDesc.BufferDesc.Width, 
			_swapChainDesc.BufferDesc.Height, _swapChainDesc.BufferDesc.Format, _swapChainDesc.Flags);

	// Re-create a depth-stencil view
	_depthStencilDesc.Width = _swapChainDesc.BufferDesc.Width;
	_depthStencilDesc.Height = _swapChainDesc.BufferDesc.Height;

	DXRETURN( createDepthStencilView() );

	// Re-create a render target 
	DXRETURN( createRenderTargetView() );

	// Bind the views to the output
	_dxImmedDC->OMSetRenderTargets(1, &_renderTargetView, _depthStencilView);

	// Setup viewport
	setupViewport();
	width = _width;
	height = _height;
	return S_OK;
}

bool DxApp::loadEffects(const char* filename, std::vector<char> &compiledShader) const
{
	std::ifstream fin(filename, std::ios::binary);
	if (!fin)
		return false;
	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();

	compiledShader.resize(size);
	fin.seekg(0, std::ios_base::beg);
	fin.read(&compiledShader[0], size);
	if (!fin)
		return false;
	return true;
}

void DxApp::setupViewport() {
	D3D11_VIEWPORT vp;
	vp.Width = (float)_swapChainDesc.BufferDesc.Width;
	vp.Height = (float)_swapChainDesc.BufferDesc.Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	_dxImmedDC->RSSetViewports(1, &vp);
}

HRESULT DxApp::createDepthStencilView() {
	ID3D11Texture2D *dsBuffer = NULL;
	DXRETURN( _dxDev->CreateTexture2D(&_depthStencilDesc, NULL, &dsBuffer) );
	HRESULT hr = _dxDev->CreateDepthStencilView(dsBuffer, NULL, &_depthStencilView);
	dsBuffer->Release();
	
	return hr;
}

HRESULT DxApp::createRenderTargetView() {
	ID3D11Texture2D *backBuffer = NULL;
	DXRETURN( _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer) );
	HRESULT hr = _dxDev->CreateRenderTargetView(backBuffer, NULL, &_renderTargetView);
	backBuffer->Release();
	
	return hr;
}
