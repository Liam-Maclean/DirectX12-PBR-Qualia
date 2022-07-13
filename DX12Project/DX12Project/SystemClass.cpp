#include "SystemClass.h"
#include "imgui.h"
#include "cvars.h"
extern AutoCVar_Int CVAR_ResolutionWidth;
extern AutoCVar_Int CVAR_ResolutionHeight;
extern AutoCVar_Int CVAR_ClientWidth;
extern AutoCVar_Int CVAR_ClientHeight;

SystemClass::SystemClass()
{
	m_Input = 0;
	m_Graphics = 0;
}

SystemClass::SystemClass(const SystemClass&)
{

}


SystemClass::~SystemClass()
{

}

bool SystemClass::Initialize()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	int screenHeight, screenWidth;
	bool result;

	screenHeight = 0;
	screenWidth = 0;

	InitializeWindows(screenHeight, screenWidth);

	m_Input = new InputClass();
	if (!m_Input)
	{
		return false;
	}

	m_Input->Initialize();

	m_Graphics = new GraphicsClass();
	if (!m_Graphics)
	{
		return false;
	}

	result = m_Graphics->Initialize(screenHeight, screenWidth, m_hwnd);
	if (!result)
	{
		return false;
	}

	return true;
}

void SystemClass::Shutdown()
{
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	ShutdownWindows();

	return;

}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	ZeroMemory(&msg, sizeof(MSG));

	done = false;
	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}
	}

	return;
}

LRESULT SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		case WM_KEYDOWN:
		{
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		}

		case WM_KEYUP:
		{
			m_Input->KeyUp((unsigned int)wparam);
			return 0;
		}

		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

bool SystemClass::Frame()
{
	bool result;

	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	result = m_Graphics->Frame(m_Input);
	if (!result)
	{
		return false;
	}

	return true;
}

void SystemClass::InitializeWindows(int& screenHeight, int& screenWidth)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	ApplicationHandle = this;

	m_hinstance = GetModuleHandle(NULL);

	m_applicationName = L"DX12 Engine";

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "DX12 Engine";
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);

	screenHeight = GetSystemMetrics(SM_CYSCREEN);
	screenWidth = GetSystemMetrics(SM_CXSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		screenWidth = CVAR_ResolutionWidth.Get();
		screenHeight = CVAR_ResolutionHeight.Get();

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, "DX12 Engine", "DX12 Engine", WS_OVERLAPPEDWINDOW , posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Hide the mouse cursor.
	ShowCursor(true);
	RECT rect;
	if (GetClientRect(m_hwnd, &rect))
	{
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		CVAR_ClientWidth.Set(width);
		CVAR_ClientHeight.Set(height);
	}

	return;



}

void SystemClass::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass("DX12 Engine", m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

	return;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	ImGuiIO& io = ImGui::GetIO();

	switch (umessage)
	{
		case WM_LBUTTONDOWN:
		{
			io.MouseDown[0] = true;
			return 0;
		}
		case WM_LBUTTONUP:
		{
			io.MouseDown[0] = false;
			return 0;
		}
		case WM_RBUTTONDOWN:
		{
			io.MouseDown[1] = true;
			return 0;
		}
		case WM_RBUTTONUP:
		{
			io.MouseDown[1] = false;
			return 0;
		}
		case WM_MBUTTONDOWN:
		{
			io.MouseDown[2] = true;
			return 0;
		}
		case WM_MBUTTONUP:
		{
			io.MouseDown[2] = false;
			return 0;
		}
		case WM_MOUSEMOVE:
		{
			io.MousePos.x = (signed short)(lparam);
			io.MousePos.y = (signed short)(lparam >> 16);
			return 0;
		}
		case WM_KEYDOWN:
		{
			if (wparam < 256)
				io.KeysDown[wparam] = 1;
			ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
			return 0;
		}
		case WM_KEYUP:
		{
			if (wparam < 256)
				io.KeysDown[wparam] = 0;
			ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
			return 0;
		}
		case WM_CHAR:
		{
			if (wparam > 0 && wparam < 0x10000)
				io.AddInputCharacter((unsigned short)wparam);
			return 0;
		}
		// Check if the window is being destroyed.
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		// All other messages pass to the message handler in the system class.
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}