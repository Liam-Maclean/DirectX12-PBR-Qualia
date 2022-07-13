#include "ENGDXResourceManager.h"

ENGDXResourceManager* ENGDXResourceManager::m_instance = nullptr;

ENGDXResourceManager::ENGDXResourceManager(ID3D12Device* device)
{
	m_device = device;
	m_instance = this;
}

ENGDXResourceManager* ENGDXResourceManager::CreateInstance(ID3D12Device* device)
{
	//if we already have an instance of the singleton alive, return that instead
	if (m_instance != nullptr)
		return m_instance;

	//else create a new singleton instance
	ENGDXResourceManager* resourceManager = new ENGDXResourceManager(device);
	return resourceManager;
}

ENGDXResourceManager* ENGDXResourceManager::GetInstance()
{
	return m_instance;
}

ID3D12PipelineState* ENGDXResourceManager::CreatePipelineState(unsigned int pipelineID, D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc)
{
	ID3D12PipelineState* newPipelineState;
	m_device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&newPipelineState));
	pipelineStateMap.insert(std::pair<unsigned int, ID3D12PipelineState*>(pipelineID, newPipelineState));
	return newPipelineState;
}

ID3D12PipelineState* ENGDXResourceManager::CreatePipelineState(unsigned int pipelineID, D3D12_COMPUTE_PIPELINE_STATE_DESC computeDesc)
{
	ID3D12PipelineState* newPipelineState;
	m_device->CreateComputePipelineState(&computeDesc, IID_PPV_ARGS(&newPipelineState));
	pipelineStateMap.insert(std::pair<unsigned int, ID3D12PipelineState*>(pipelineID, newPipelineState));
	return newPipelineState;
}

ID3D12PipelineState* ENGDXResourceManager::GetPipelineState(unsigned int pipelineID)
{
	return pipelineStateMap[pipelineID];
}
