#pragma once

#include <Windows.h>
#include "DXRenderer.h"

//constants we can change pre-initialization
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsClass
{
public:

	//Default constructor
	GraphicsClass();

	//Copy constructor 
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(InputClass* input);

private:
	bool Render(InputClass* input);

private:

	DXRenderer* m_Direct3D;
};

