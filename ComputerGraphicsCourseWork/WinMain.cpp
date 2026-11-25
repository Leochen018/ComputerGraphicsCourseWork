#include "Window.h"
#include "Core.h"
extern "C" {
	_declspec (dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	Window window;
	window.create(1024, 1024, "My Window");

	Core core;
	core.init(window.hwnd, 1024, 1024);

	while (true)
	{

		window.processMessages();

		core.beginFrame();

		if (window.keys[VK_ESCAPE] == 1)
		{
			break;
		}
		core.finishFrame();

		core.flushGraphicsQueue();
	}

}



