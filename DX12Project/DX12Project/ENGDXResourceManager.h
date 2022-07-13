#pragma once
#include <unordered_map>
#include <d3d12.h>
#include <wrl/client.h>

//SINGLETON
class ENGDXResourceManager
{
private:
	static ENGDXResourceManager* m_instance;

protected:
	ID3D12Device* m_device;
	std::unordered_map<unsigned int, ID3D12PipelineState*> pipelineStateMap;
	ENGDXResourceManager(ID3D12Device* device);

public:
	static ENGDXResourceManager* CreateInstance(ID3D12Device* device);
	static ENGDXResourceManager* GetInstance();

	ID3D12PipelineState* CreatePipelineState(unsigned int pipelineID, D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc);
	ID3D12PipelineState* CreatePipelineState(unsigned int pipelineID, D3D12_COMPUTE_PIPELINE_STATE_DESC pipelineDesc);
	ID3D12PipelineState* GetPipelineState(unsigned int pipelineID);
};

