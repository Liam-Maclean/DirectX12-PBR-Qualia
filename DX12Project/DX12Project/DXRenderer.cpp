#pragma once
#include "DXRenderer.h"
#include "ENGModel.h"
#include "ENGRendererContext.h"
#include <stdexcept>
#include "DeferredRenderer.h"
#include "cvars.h"


extern AutoCVar_Int CVAR_ImGuiEnabled;
extern AutoCVar_Int CVAR_TesselationFactor;
extern AutoCVar_Int CVAR_InnerTesselationFactor;

///<summary>Constructor (default)</summary>
DXRenderer::DXRenderer()
{

}

///<summary>Constructor (Copy)</summary>
DXRenderer::DXRenderer(const DXRenderer &)
{

}

///<summary>Destructor</summary>
DXRenderer::~DXRenderer()
{
}

///<summary>Initialise the creation of the D3D window</summary>
///<param name="screenWidth">Width of the output screen</param>
///<param name="screenHeight">Height of the output screen</param>
///<param name="hwnd">Window handle of the output screen</param>
///<param name="vsync">vsync toggle</param>
///<param name="fullscreen">Set game fullscreen or windowed</param>
bool DXRenderer::Initialize(int screenWidth, int screenHeight, HWND hwnd, bool vsync, bool fullscreen)
{
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	m_HWND = hwnd;
	//Create the graphics context
	ENGContext.Init(m_HWND);

	//Load the frameheap
	m_frame = new DXFrameHeapManager(ENGContext.GetDevice());

	CreateFences();
	CreateViewportObject();

	CreateSceneData();
	CreateRenderers();
	CreateViewportObject();
	EndInitialization();
	


	return true;
}

///<summary>Create and initialize any scene data that is needed for any renderer (Camera, models and lights)</summary>
void DXRenderer::CreateSceneData()
{
	//Create Lights
	m_directionalLight.direction = glm::vec4(-0.2f, -1.0f, 0.0f, 1.0f);
	m_directionalLight.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);


	//Load Geometry
	model.LoadFromFile("C:/Users/liamm/Downloads/DirectX12-Engine-DeferredRendering/DirectX12-Engine-DeferredRendering/DX12Project/DX12Project/Models/MasterChief/scene.gltf", 1.0f);
	//model.LoadFromFile("C:/Users/liamm/Downloads/DirectX12-Engine-DeferredRendering/DirectX12-Engine-DeferredRendering/DX12Project/DX12Project/Models/spartan_armour_mkv_-_halo_reach/scene.gltf", 1.0f);
	//model.LoadFromFile("C:/Users/liamm/Downloads/DirectX12-Engine-DeferredRendering/DirectX12-Engine-DeferredRendering/DX12Project/DX12Project/Models/stylised_sky_player_home_dioroma/scene.gltf", 1.0f);
	//model.LoadFromFile("C:/Users/liamm/Downloads/DirectX12-Engine-DeferredRendering/DirectX12-Engine-DeferredRendering/DX12Project/DX12Project/Models/starry_night_diorama_tilt_brush/scene.gltf", 5.0f);
	//model.LoadFromFile("C:/Users/liamm/Downloads/DirectX12-Engine-DeferredRendering/DirectX12-Engine-DeferredRendering/DX12Project/DX12Project/Models/halo_mk_v_model/scene.gltf", 1.0f);
	//model.LoadFromFile("C:/Users/liamm/Downloads/DirectX12-Engine-DeferredRendering/DirectX12-Engine-DeferredRendering/DX12Project/DX12Project/Models/cube/glTF/cube.gltf", 1.0f);
	//model.LoadFromFile("C:/Users/liamm/Downloads/DirectX12-Engine-DeferredRendering/DirectX12-Engine-DeferredRendering/DX12Project/DX12Project/Models/sponza/glTF/sponza.gltf", 1.0f);
	//model.LoadFromFile("C:/Users/liamm/Downloads/DirectX12-Engine-DeferredRendering/DirectX12-Engine-DeferredRendering/DX12Project/DX12Project/Models/valleyoftrials/scene.gltf", 1.0f);
	//model.LoadFromFile("C:/Users/liamm/Downloads/DirectX12-Engine-DeferredRendering/DirectX12-Engine-DeferredRendering/DX12Project/DX12Project/Models/darius/scene.gltf", 1.0f);
	//model.LoadFromFile("C:/Users/liamm/Downloads/DirectX12-Engine-DeferredRendering/DirectX12-Engine-DeferredRendering/DX12Project/DX12Project/Models/new_cortana_halo4_og/scene.gltf", 1.0f);
	//model.LoadFromFile("C:/Users/liamm/Downloads/DirectX12-Engine-DeferredRendering/DirectX12-Engine-DeferredRendering/DX12Project/DX12Project/Models/concerto/scene.gltf", 1.0f);
	//model.LoadFromFile("C:/Users/liamm/Downloads/DirectX12-Engine-DeferredRendering/DirectX12-Engine-DeferredRendering/DX12Project/DX12Project/Models/jobba_the_brawler_frog/scene.gltf", 1.0f);
	//model.LoadFromFile("C:/Users/liamm/Downloads/DirectX12-Engine-DeferredRendering/DirectX12-Engine-DeferredRendering/DX12Project/DX12Project/Models/among_drip/scene.gltf", 1.0f);
	

	m_camera = new Camera(glm::vec3(-3.0f, 2.0f, -70.0f), glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), m_screenWidth, m_screenHeight);

	//This could belong to the model
	m_cube1WorldMat = glm::mat4(1.0f);
	m_cube1WorldMat = glm::scale(m_cube1WorldMat, glm::vec3(0.2f));
	m_cube1WorldMat = glm::rotate(m_cube1WorldMat, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));


}

///<summary>Create and initialize any renderers we're using (ImGui and Deferred for now)</summary>
void DXRenderer::CreateRenderers()
{
	m_deferredRenderer = new DeferredRenderer(m_screenWidth, m_screenHeight);
	m_deferredRenderer->Initialize(m_frame);
	m_deferredRenderer->SetUpModels(&model);
	m_deferredRenderer->CreateLightCBV(&m_directionalLight);
	if (CVAR_ImGuiEnabled.Get() == 1)
	{
		m_imGuiRenderer = new ImGuiRenderer(m_screenWidth, m_screenHeight);
		m_imGuiRenderer->Initialize(m_HWND);
		m_imGuiRenderer->GrabMainLight(&m_directionalLight);
	}
}

///<summary>Create and initialize the viewport and scissor object</summary>
void DXRenderer::CreateViewportObject()
{
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.Width = m_screenWidth;
	m_viewport.Height = m_screenHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;

	m_scissorRect.left = 0;
	m_scissorRect.top = 0;
	m_scissorRect.right = m_screenWidth;
	m_scissorRect.bottom = m_screenHeight;
}

///<summary>Create fences needed for synchronisation</summary>
void DXRenderer::CreateFences()
{
	for (int i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_fences[i] = ENGFence::Create();
	}
}

//Update (Handle input)
///<summary>Update (Handle input)</summary>
///<param name="input">Input handler for the game</param>
void DXRenderer::Update(InputClass* input)
{	
	m_camera->HandleInput(input);
}

///<summary>Release all open pointers and memory <summary/>
void DXRenderer::Shutdown()
{
	
}

///<summary>End the initialization and execute any command list executions needed</summary>
void DXRenderer::EndInitialization()
{
	HRESULT hr;
	ID3D12GraphicsCommandList* m_commandList = ENGContext.GetCommandList();
	m_commandList->Close();
	ID3D12CommandList* ppCommandLists[] = { m_commandList };
	ENGContext.GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	m_fences[m_bufferIndex]->IncrementFenceValue();
	m_fences[m_bufferIndex]->Signal();
}

///<summary>Updates the pipeline and records commands</summary>
void DXRenderer::UpdatePipeline()
{

	if (CVAR_ImGuiEnabled.Get() == 1)
	{
		m_imGuiRenderer->StartImGuiFrame();
	}

	HRESULT hr;
	unsigned int renderTargetViewDescriptorSize; 
	renderTargetViewDescriptorSize = ENGContext.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	WaitForPreviousFrame();

	ID3D12GraphicsCommandList* m_commandList = ENGContext.GetCommandList();

	hr = ENGContext.GetAllocator(m_bufferIndex)->Reset();
	if (FAILED(hr))
	{

	}

	hr = m_commandList->Reset(ENGContext.GetAllocator(m_bufferIndex), NULL);
	if (FAILED(hr))
	{

	}


	//Deferred G-Buffer Pass
	m_deferredRenderer->PrepareFrame(m_cube1WorldMat, m_camera, &m_directionalLight);
	m_deferredRenderer->SetPSOForward(m_commandList, m_bufferIndex);
	m_deferredRenderer->StartFrameForward(m_commandList, m_bufferIndex);
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // IA = Input assembler
	m_commandList->RSSetViewports(1, &m_viewport);				//RS = Rasterizer
	m_commandList->RSSetScissorRects(1, &m_scissorRect);

	model.Draw(m_commandList);
	m_deferredRenderer->EndFrameForward(m_commandList);


	////Composition pass
	m_deferredRenderer->SetPSOComposition(m_commandList);
	m_deferredRenderer->StartFrameComposition(m_commandList, m_bufferIndex);
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // IA = Input assembler
	m_commandList->RSSetViewports(1, &m_viewport);				//RS = Rasterizer
	m_commandList->RSSetScissorRects(1, &m_scissorRect);
	m_deferredRenderer->DrawScreenQuad(m_commandList);
	m_deferredRenderer->EndFrameComposition(m_commandList, m_bufferIndex);


	//FXAA
	m_deferredRenderer->SetPSOPostProcess(m_commandList, m_bufferIndex);
	m_deferredRenderer->StartFramePostProcess(m_commandList, m_bufferIndex);
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // IA = Input assembler
	m_commandList->RSSetViewports(1, &m_viewport);				//RS = Rasterizer
	m_commandList->RSSetScissorRects(1, &m_scissorRect);
	//Render the quad again with the new post process technique applied ontop
	m_deferredRenderer->DrawScreenQuad(m_commandList);
	if (CVAR_ImGuiEnabled.Get() == 1)
	{
		m_imGuiRenderer->RenderImGuiData();
	}
	m_deferredRenderer->EndFramePostProcess(m_commandList, m_bufferIndex);

	hr = m_commandList->Close();
	if (FAILED(hr))
	{

	}
}

///<summary>Render geometry using pipelines</summary>
bool DXRenderer::Render()
{
	HRESULT hr;
	UpdatePipeline();

	ID3D12GraphicsCommandList* m_commandList = ENGContext.GetCommandList();
	ID3D12CommandQueue* m_commandQueue = ENGContext.GetCommandQueue();
	ID3D12CommandList* ppCommandLists[] = { m_commandList };

	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	
	m_fences[m_bufferIndex]->Signal();

	///Present using VSync
	if (m_vsync_enabled)
	{
		//lock to screen refresh rate
		hr = ENGContext.GetSwapchain()->Present(1, 0);
		if (FAILED(hr))
		{
			return false;
		}
	}
	else
	{
		//Present as fast as possible
		hr = ENGContext.GetSwapchain()->Present(0, 0);
		if (FAILED(hr))
		{
			return false;
		}
	}

	return true;
}

///<summary>Waits for the previous frame to render</summary>
void DXRenderer::WaitForPreviousFrame()
{
	//Swap the current RTV buffer index so we draw on the correct buffer
	m_bufferIndex = ENGContext.GetSwapchain()->GetCurrentBackBufferIndex();
	m_fences[m_bufferIndex]->WaitForFenceEvent();
}