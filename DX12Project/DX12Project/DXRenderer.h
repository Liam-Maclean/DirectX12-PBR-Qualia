#pragma once

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

//DX12 compulsary headers
#include "InputClass.h"
#include <d3dcompiler.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <wincodec.h>
#include "ENGTextureLoader.h"
#include "ENGBufferUtility.h"
#include "ENGGraphicsStructs.h"
#include "ENGShaderLoader.h"
#include "ENGInputLayouts.h"
#include "ENGFence.h"
#include "ImGuiRenderer.h"
#include "MeshSubdivideRenderer.h"
#include "DXFrameHeapManager.h"
#include "ENGBuffer.h"
#include "Camera.h"
#include "ENGModel.h"
#include "../Libs/glm/glm/glm.hpp"
#define DX12_FEATURE_LEVEL D3D_FEATURE_LEVEL_12_1
#define FRAME_BUFFER_COUNT 3
class DeferredRenderer;
class DXRenderer
{

public:
	struct ConstantBuffer {
		glm::vec4 colorMultiplier;
	};

	struct ConstantBufferPerObject {
		glm::mat4 mvpMat;
	};

	struct TesselationParams {
		int tesselationFactor;
		int innerTesselationFactor;
		int padding1;
		int padding2;
	};


	//Functions
	DXRenderer();
	DXRenderer(const DXRenderer&);
	~DXRenderer();

	bool Initialize(int, int, HWND, bool, bool);
	void Update(InputClass* input);
	void Shutdown();
	bool Render();

private:
	bool CreateDescriptorHeap();
	void CreateSceneData();
	void CreateRenderers();
	void CreateViewportObject();
	
	void EndInitialization();
	void CreateFences();
	void UpdatePipeline();
	void WaitForPreviousFrame();
	void LoadGeometry();

private:

	HWND m_HWND;
	DXFrameHeapManager* m_frame;
	unsigned int m_bufferIndex;
	D3D12_VIEWPORT m_viewport;							//What we see in the output of the rasterizer
	D3D12_RECT m_scissorRect;							//Area to draw into
	unsigned int m_screenWidth, m_screenHeight;
	bool m_vsync_enabled = true;

	
	//renderers
	DeferredRenderer* m_deferredRenderer;
	ImGuiRenderer* m_imGuiRenderer;
	MeshSubdivideRenderer* m_subdivideRenderer;

	ENGFence* m_fences[FRAME_BUFFER_COUNT];

	//Model and shader data
	Camera* m_camera;
	ENGModel model;
	ENGGraphicsStructs::DirectionalLight m_directionalLight;
	glm::mat4 m_cube1WorldMat; //First cube world matrix
};

