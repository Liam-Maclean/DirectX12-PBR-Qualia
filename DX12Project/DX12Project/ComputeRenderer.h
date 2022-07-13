#pragma once
/*
#include <d3d12.h>
#include "Camera.h"
#include <vector>
#include "ENGBufferUtility.h"

#include "ENGShaderLoader.h"
#include "ENGGraphicsStructs.h"
#include "DXFrameHeapManager.h"
class ComputeRenderer
{
public:
	ComputeRenderer(ID3D12Device* device);

	
	void Initialize(ID3D12GraphicsCommandList* commandList);
private:
	void CreateBuffers(ID3D12GraphicsCommandList* commandList);
	void CreateConstantBuffers();
	void CreateRootSignature();
	void CreatePSO();
	void Draw(ENGGraphicsStructs::Mesh* mesh, ID3D12GraphicsCommandList* commandList);
	void StartFrame(ID3D12GraphicsCommandList* commandList);
	void PrepareFrame(std::vector<ENGGraphicsStructs::Model> models, Camera* camera);
	void EndFrame(ID3D12GraphicsCommandList* commandList);

	ID3D12RootSignature* m_rootSignature;
	Camera* m_camera;

	//std::unique_ptr<FrameManager> m_frame;

};

*/