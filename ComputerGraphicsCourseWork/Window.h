#pragma once
#include <Windows.h>
#include <string>
class Window
{
public:
	HWND hwnd;
	HINSTANCE hinstance;

	int width, height;
	std::string name;



	bool keys[256];
	int mousex ;
	int mousey;
	bool mouseButtons [3];

	void processMessages();

	void create(int window_width, int window_height, const std::string window_name);
	void updateMouse(int x, int y)
	{
		mousex = x;
		mousey = y;
	}
};