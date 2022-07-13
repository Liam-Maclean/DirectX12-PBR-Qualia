#pragma once
#include <d3dcompiler.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include "d3dx12.h"

#include "ENGGraphicsStructs.h"
#include "ENGDXResourceManager.h"
#include "ENGInputLayouts.h"
#include "ENGShaderLoader.h"
#include "ENGBufferUtility.h"
#include "ENGPipeline.h"
#include "ENGShaderLayout.h"
#include "ENGBuffer.h"
#include <vector>
#include <memory>

#include "DXFrameHeapManager.h"
#include "Camera.h"

class MeshSubdivideRenderer
{
public:
	MeshSubdivideRenderer(int width, int height);
	void Initialize();
	void CreateCPUHeaps();
	void CreateBuffers();
	void CreateRenderTargets();
	void CreatePSO();
	void SetPSO(ID3D12GraphicsCommandList* commandList);
	void Draw(ENGGraphicsStructs::Mesh* mesh, ID3D12GraphicsCommandList* commandList);
	void StartFrame(ID3D12GraphicsCommandList* commandList, int bufferIndex);
	void PrepareFrame(std::vector<ENGGraphicsStructs::Model> models, Camera* camera);
	void EndFrame(ID3D12GraphicsCommandList* commandList, int bufferIndex);



private:

	//Pipelines
	ENGShaderLayout* m_shaderLayout;
	ENGPipeline* m_pipeline;


	//Scene data
	Camera* m_camera;
	FrameHeapParameters m_frameHeapParameters;

	//Texture data
	DX::Texture braynzarTexture;

	//Buffers
	ENGBuffer* m_textureBuffer;
	ENGBuffer* m_depthStencilBuffer;


	//Views
	ID3D12Resource* m_backBufferRenderTarget[3];

	//Heaps
	std::unique_ptr<DXFrameHeapManager> m_frame;
	DXDescriptorHeap m_rtvHeap;
	DXDescriptorHeap m_srvHeap;
	DXDescriptorHeap m_dsvHeap;
	int srvHeapIndex;


	


};

