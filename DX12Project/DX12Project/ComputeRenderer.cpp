#include "ComputeRenderer.h"
/*
ComputeRenderer::ComputeRenderer(ID3D12Device* device)
{
	m_device = device;
	m_camera = 0;
	m_resourceManager = 0;
	m_rootSignature = 0;
}

void ComputeRenderer::Initialize(ID3D12GraphicsCommandList* commandList)
{
	m_resourceManager = ENGDXResourceManager::CreateInstance(m_device);
	CreateRootSignature();
	CreatePSO();
	CreateBuffers(commandList);
}

void ComputeRenderer::CreateBuffers(ID3D12GraphicsCommandList* commandList)
{
}

void ComputeRenderer::CreateConstantBuffers()
{
}

void ComputeRenderer::CreateRootSignature()
{
	HRESULT result;

	D3D12_ROOT_DESCRIPTOR rootCBVDescriptor;
	rootCBVDescriptor.RegisterSpace = 0;
	rootCBVDescriptor.ShaderRegister = 0;

	D3D12_DESCRIPTOR_RANGE descriptorTableRanges[1]; // just use one descriptor table for now
	descriptorTableRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorTableRanges[0].NumDescriptors = 1;
	descriptorTableRanges[0].BaseShaderRegister = 0;
	descriptorTableRanges[0].RegisterSpace = 0;
	descriptorTableRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable;
	descriptorTable.NumDescriptorRanges = _countof(descriptorTableRanges);
	descriptorTable.pDescriptorRanges = &descriptorTableRanges[0];

	D3D12_ROOT_PARAMETER rootParameters[2];
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //This is a constant buffer view root descriptor
	rootParameters[0].Descriptor = rootCBVDescriptor; // this is our descriptor table for this root parameter
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // our vertex shader will be the only shader accessing this parameter for now 

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[1].DescriptorTable = descriptorTable;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC sampler = {};
	sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = 0;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	sampler.MinLOD = 0.0f;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	sampler.ShaderRegister = 0;
	sampler.RegisterSpace = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(_countof(rootParameters), rootParameters, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS);

	ID3DBlob* signature;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);
	if (FAILED(result))
	{

	}

	result = m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
	if (FAILED(result))
	{

	}

}

void ComputeRenderer::CreatePSO()
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC  psoDesc = {};
	psoDesc.pRootSignature = m_rootSignature;
	psoDesc.CS = ENGShaderLoader::LoadShader(L"ComputeShader.cso");
	m_resourceManager->CreatePipelineState(0, psoDesc);
}

void ComputeRenderer::StartFrame(ID3D12GraphicsCommandList* commandList)
{
}

void ComputeRenderer::PrepareFrame(std::vector<ENGGraphicsStructs::Model> models, Camera* camera)
{
}

void ComputeRenderer::EndFrame(ID3D12GraphicsCommandList* commandList)
{
}
*/