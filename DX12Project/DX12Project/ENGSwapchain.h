#pragma once
#include "ENGBase.h"
#include "DirectXWrappers.h"

class ENGSwapchain : public IHandle<ENGSwapchain, IDXGISwapChain3*>
{
public:
	ENGSwapchain(HWND hwnd, int screenHeight, int screenWidth);
	~ENGSwapchain();

private:

	int width, height;

	unsigned int m_videoCardMemory;
	bool m_vsync_enabled = false, m_fullscreen = false;
	char m_videoCardDescription[128];

};

