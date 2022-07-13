#pragma once
#include <vector>
#include <memory>
#include <d3dcompiler.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include "d3dx12.h"
#include "ENGBase.h"
#include "ENGScreenQuad.h"
#include "ENGGraphicsStructs.h"
#include "ENGPipeline.h"
#include "ENGBuffer.h"
#include "ENGTexture.h"
#include "ENGDXResourceManager.h"
#include "ENGInputLayouts.h"
#include "ENGShaderLoader.h"
#include "ENGBufferUtility.h"
#include "DXFrameHeapManager.h"
#include "ENGModel.h"
#include "Camera.h"


class ENGModel;
class DeferredRenderer
{
	struct MVPMatrix {
		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 model;
		glm::mat4 invertedViewMat;
		glm::mat4 invertedProjMat;
		glm::mat4 invertedTransposeModelMat;
		glm::vec4 cameraPos;
	};



public:
	DeferredRenderer(int width, int height);
	void Initialize(DXFrameHeapManager* frameHeap);
	void CreateConstantBuffers();
	void CreateCPUHeaps();
	void CreateRenderTargets();
	void CreatePSO();
	void CreateLightCBV(ENGGraphicsStructs::DirectionalLight* directionalLight);
	void SetUpModels(ENGModel* mesh);
	void Draw(ENGGraphicsStructs::Mesh* mesh, ID3D12GraphicsCommandList* commandList);
	void Draw(ENGModel* mesh, ID3D12GraphicsCommandList* commandList);
	void DrawScreenQuad(ID3D12GraphicsCommandList* commandList);
	void PrepareFrame(glm::mat4 modelMat, Camera* camera, ENGGraphicsStructs::DirectionalLight* directionalLight);

	void StartFrameForward(ID3D12GraphicsCommandList* commandList, int bufferIndex);
	void SetPSOForward(ID3D12GraphicsCommandList* commandList, int bufferIndex);
	void EndFrameForward(ID3D12GraphicsCommandList* commandList);

	void StartFrameComposition(ID3D12GraphicsCommandList* commandList, int bufferIndex);
	void SetPSOComposition(ID3D12GraphicsCommandList* commandList);
	void EndFrameComposition(ID3D12GraphicsCommandList* commandList, int bufferIndex);

	void StartFramePostProcess(ID3D12GraphicsCommandList* commandList, int bufferIndex);
	void SetPSOPostProcess(ID3D12GraphicsCommandList* commandList, int bufferIndex);
	void EndFramePostProcess(ID3D12GraphicsCommandList* commandList, int bufferIndex);
	
	

private:

	float m_clearColor[4] = { 0.0,0.0f,0.0f,1.0f };
	DXGI_FORMAT m_rtvFormat[3] = { DXGI_FORMAT_R16G16B16A16_FLOAT,DXGI_FORMAT_R16G16B16A16_FLOAT,DXGI_FORMAT_R16G16B16A16_UNORM };
	D3D12_CLEAR_VALUE depthOptimizedClearValue;

	ShaderDebugValues* m_shaderDebugValues;

	//Pipelines
	ENGShaderLayout* m_deferredShaderLayout;
	ENGShaderLayout* m_fxaaShaderLayout;
	ENGPipeline* m_forwardPipeline;
	ENGPipeline* m_compositionPipeline;
	ENGPipeline* m_fxaaPipeline;

	//Scene data
	Camera* m_camera;
	FrameHeapParameters m_frameHeapParameters;
	MVPMatrix m_mvpMat;

	ENGBuffer* m_mvpConstantBuffer;
	//ID3D12Resource* m_constantBufferUploadHeaps[FRAME_BUFFER_COUNT];	//This is the memory on the GPU where constant buffers for each frame will be placed
	UINT8* m_mvpConstantBufferAddress;	//This is a pointer to each of the constant buffer Resource Heaps

	ENGBuffer* m_directionalLightConstantBuffer;
	//Texture data
	DX::Texture m_braynzarTexture;
	

	ENGBuffer* m_cubeMapBuffer;
	DX::Texture m_cubemap;

	//Buffers
	ENGTexture* m_albedoRoughnessRTVTexture;
	ENGTexture* m_normalRTVTexture;
	ENGTexture* m_emissiveRTVTexture;
	ENGTexture* m_postProcessRTVTexture;

	ENGBuffer* m_textureBuffer;
	ENGBuffer* m_depthStencilBuffer;
	ENGBuffer* m_positionCommitedResourceBuffer;
	ENGBuffer* m_normalCommitedResourceBuffer;
	ENGBuffer* m_albedoCommitedResourceBuffer;
	ENGBuffer* m_postProcessCommitedResourceBuffer;
	ENGBuffer* m_debugShaderValuesBuffer;
	ENGScreenQuad m_screenQuad;

	//Views
	ID3D12Resource* m_backBufferRenderTarget[3];
	ID3D12Resource* m_rtvTexture[3];
	ID3D12Resource* m_MainRenderViewTexture;
	std::vector<Texture> m_textures;

	

	//Heaps
	DXFrameHeapManager* m_frame;
	DXDescriptorHeap m_cubemapSRVHeap;
	DXDescriptorHeap m_rtvHeap;
	DXDescriptorHeap m_cbvHeap;
	DXDescriptorHeap m_deferredSRVHeap;
	DXDescriptorHeap m_deferredRTVHeap;
	DXDescriptorHeap m_srvHeap;
	DXDescriptorHeap m_dsvHeap;
	DXDescriptorHeap m_postProcessHeap;
	int m_postProcessHeapIndex;
	int m_srvHeapIndex;
	int m_deferredSRVHeapIndex;
	int m_rtvHeapIndex;
	int m_deferredRTVHeapIndex;
};
