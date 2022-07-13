#include "GraphicsClass.h"



GraphicsClass::GraphicsClass()
{
	m_Direct3D = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass &)
{

}

GraphicsClass::~GraphicsClass()
{

}

bool GraphicsClass::Initialize(int screenHeight, int screenWidth, HWND hwnd)
{
	bool result;

	//Create the Direct3D object
	m_Direct3D = new DXRenderer;
	if (!m_Direct3D)
	{
		return false;
	}

	//initialize the Direct3D object
	result = m_Direct3D->Initialize(screenWidth, screenHeight, hwnd, VSYNC_ENABLED, FULL_SCREEN);
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize Direct3D", "Error", MB_OK);
		return false;
	}
	return true;
}

void GraphicsClass::Shutdown()
{
	//release Direct3D object
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	return;
}

bool GraphicsClass::Frame(InputClass* input)
{
	bool result;

	//render the graphics scene
	result = Render(input);
	if (!result)
	{
		return false;
	}

	return true;
}

bool GraphicsClass::Render(InputClass* input)
{
	bool result;

	//Use the Direct3D object to render the scene
	m_Direct3D->Update(input);
	result = m_Direct3D->Render();
	if (!result)
	{
		return false;
	}
	return true;
}
