//  ---------------------------------------------------------------------------
//  Department of Computer Science, University of Hull
//  DirectX 11 framework, Aug 2012
//
//  ---------------------------------------------------------------------------

#include "MyApp.h"

// Add the following to the project properties: Configuations Proporties->VC++ Directories for ALL configs
// Executable directories: C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Utilities\bin\x86
// Include directories:    C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include
//                         C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Samples\C++\Effects11\Inc
// Library directories:    C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x86

// Libraries (Alternative is to add to Solution)
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxerr.lib")
#pragma comment(lib, "Effects11.lib") // Warning, this library needs to be built.


int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int cmdShow) {
	// Memory leak detection
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	MyApp app;
	if (!app.initialise(instance, cmdShow,800,600))//width and height needs to be changeed to read from config
		return 0;
		
	return app.run();
}