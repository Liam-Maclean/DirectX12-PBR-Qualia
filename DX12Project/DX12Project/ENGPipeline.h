#pragma once
#include "d3dx12.h"
#include "ENGBase.h"
#include "ENGShaderLayout.h"
#include <vector>

struct PipelineCreateInfo
{
	D3D12_SHADER_BYTECODE vertShader;
	D3D12_SHADER_BYTECODE pixelShader;
	D3D12_SHADER_BYTECODE computeShader;
	D3D12_SHADER_BYTECODE hullShader;
	D3D12_SHADER_BYTECODE domainShader;
	D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType;
	D3D12_INPUT_LAYOUT_DESC vertexInputLayout;
	ENGShaderLayout* shaderLayout;
	bool wireframeMode;
	bool depthTestEnabled;
	std::vector<DXGI_FORMAT> rtvFormats;
};


class ENGPipeline : public IHandle<ENGPipeline, ID3D12PipelineState*>
{
public:

	
	ENGPipeline(const PipelineCreateInfo& desc);
	ENGPipeline();
	~ENGPipeline();

	inline ENGShaderLayout* GetRootSignature() { return m_rootSignature; }
private:

	ENGShaderLayout* m_rootSignature;

};

