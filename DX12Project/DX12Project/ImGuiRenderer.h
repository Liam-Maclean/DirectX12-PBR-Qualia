#pragma once
#include <imgui.h>
#include <d3d12.h>
#include "DXFrameHeapManager.h"
#include "ENGGraphicsStructs.h"
class ImGuiRenderer
{
public:
	ImGuiRenderer(int width, int height);
	void Initialize(HWND winHandle);
	void GrabMainLight(ENGGraphicsStructs::DirectionalLight* inMainLight);
	void StartImGuiFrame();
	void RenderImGuiData();
private:
	void SetImGuiStyle(ImGuiIO* io);
	ID3D12DescriptorHeap* m_cbvSrvHeap;
	ENGGraphicsStructs::DirectionalLight* mainLight;
};

