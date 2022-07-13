#include "ENGPipeline.h"
#include "ENGRendererContext.h"
#include "ENGInputLayouts.h"
#include "ENGShaderLoader.h"

ENGPipeline::ENGPipeline(const PipelineCreateInfo& info)
{
	HRESULT hr;
	CD3DX12_RASTERIZER_DESC rasterizer(D3D12_DEFAULT);
	if (info.wireframeMode)
	{
		rasterizer.FillMode = D3D12_FILL_MODE_WIREFRAME;
	}
	else
	{
		rasterizer.FillMode = D3D12_FILL_MODE_SOLID;
	}
	rasterizer.CullMode = D3D12_CULL_MODE_NONE;

	//if we have a compute shader in the pipeline create info we need an entire different kind of pipeline
	if (info.computeShader.pShaderBytecode != nullptr)
	{

	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = info.vertexInputLayout;
	psoDesc.pRootSignature = info.shaderLayout->Handle();
	psoDesc.VS = info.vertShader;
	psoDesc.PS = info.pixelShader;
	
	if (info.hullShader.pShaderBytecode)
	{
		psoDesc.HS = info.hullShader;
	}
	if (info.domainShader.pShaderBytecode)
	{
		psoDesc.DS = info.domainShader;
	}
	psoDesc.PrimitiveTopologyType = info.topologyType;

	for (int i = 0; i < info.rtvFormats.size(); ++i)
	{
		psoDesc.RTVFormats[i] = info.rtvFormats[i];
	}

	if (info.depthTestEnabled)
	{
		psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	}
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.RasterizerState = rasterizer;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	psoDesc.NumRenderTargets = static_cast<UINT>(info.rtvFormats.size());
	
	ENGContext.GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_handle));
}

ENGPipeline::ENGPipeline()
{

}

ENGPipeline::~ENGPipeline()
{

}
