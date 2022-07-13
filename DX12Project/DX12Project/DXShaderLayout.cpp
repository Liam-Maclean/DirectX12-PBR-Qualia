#include "ENGShaderLayout.h"
#include "d3dx12.h"
#include "ENGRendererContext.h"

ENGShaderLayout::ENGShaderLayout()
{

}

ENGShaderLayout::~ENGShaderLayout()
{

}

void ENGShaderLayout::GetBlinnPhongShaderLayout()
{

}

void ENGShaderLayout::GetFXAAShaderLayout()
{

	HRESULT result;

	CD3DX12_DESCRIPTOR_RANGE range[1] = {};
	//Screen SRV buffer
	range[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	CD3DX12_ROOT_PARAMETER rootParameters[1] = {};
	rootParameters[0].InitAsDescriptorTable(1, &range[0], D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_STATIC_SAMPLER_DESC staticSampler[1] = {};
	staticSampler[0].Init(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(_countof(rootParameters), rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS);
	rootSignatureDesc.NumStaticSamplers = 1;
	rootSignatureDesc.pStaticSamplers = staticSampler;

	ID3DBlob* signature;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);
	if (FAILED(result))
	{

	}

	result = ENGContext.GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_handle));
	if (FAILED(result))
	{

	}
}

void ENGShaderLayout::GetDefferredShaderLayout()
{
	HRESULT result;

	CD3DX12_DESCRIPTOR_RANGE range[3] = {};
	//Camera matrices
	range[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	//G buffer
	range[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 5, 0); //T0 -> T4
	//Texture bindings (diffuse, metallic, normal, occlusion and emissive)
	range[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	-1, 5, 1); //T5 - > unbound


	CD3DX12_ROOT_PARAMETER rootParameters[7] = {};
	rootParameters[0].InitAsDescriptorTable(1, &range[0], D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[1].InitAsDescriptorTable(1, &range[1], D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[2].InitAsDescriptorTable(1, &range[2], D3D12_SHADER_VISIBILITY_ALL); //(T3, space1) - unbounded)
	rootParameters[3].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[4].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[5].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[6].InitAsConstantBufferView(4, 0, D3D12_SHADER_VISIBILITY_ALL);
	//rootParameters[2].InitAsDescriptorTable(1, &range[2], D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_STATIC_SAMPLER_DESC staticSampler[1];
	staticSampler[0].Init(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	//staticSampler[0].
	//D3D12_STATIC_SAMPLER_DESC sampler = {};
	//sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	//sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	//sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	//sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	//sampler.MipLODBias = 0;
	//sampler.MaxAnisotropy = 0;
	//sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	//sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	//sampler.MinLOD = 0.0f;
	//sampler.MaxLOD = D3D12_FLOAT32_MAX;
	//sampler.ShaderRegister = 0;
	//sampler.RegisterSpace = 0;
	//sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(_countof(rootParameters), rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS);
	rootSignatureDesc.NumStaticSamplers = 1;
	rootSignatureDesc.pStaticSamplers = staticSampler;

	ID3DBlob* signature;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);
	if (FAILED(result))
	{

	}

	result = ENGContext.GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_handle));
	if (FAILED(result))
	{

	}
}

void ENGShaderLayout::GetTessellationShaderLayout()
{

	HRESULT hr;
	D3D12_ROOT_DESCRIPTOR rootCBVDescriptor;
	rootCBVDescriptor.RegisterSpace = 0;
	rootCBVDescriptor.ShaderRegister = 0;

	D3D12_ROOT_DESCRIPTOR rootCBVDescriptorHull;
	rootCBVDescriptorHull.RegisterSpace = 0;
	rootCBVDescriptorHull.ShaderRegister = 1;

	D3D12_DESCRIPTOR_RANGE descriptorTableRanges[1]; // just use one descriptor table for now
	descriptorTableRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorTableRanges[0].NumDescriptors = 1;
	descriptorTableRanges[0].BaseShaderRegister = 0;
	descriptorTableRanges[0].RegisterSpace = 0;
	descriptorTableRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable;
	descriptorTable.NumDescriptorRanges = _countof(descriptorTableRanges);
	descriptorTable.pDescriptorRanges = &descriptorTableRanges[0];

	D3D12_ROOT_PARAMETER rootParameters[3];
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //This is a constant buffer view root descriptor
	rootParameters[0].Descriptor = rootCBVDescriptor; // this is our descriptor table for this root parameter
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; // our vertex shader will be the only shader accessing this parameter for now 

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[1].DescriptorTable = descriptorTable;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //This is a constant buffer view root descriptor
	rootParameters[2].Descriptor = rootCBVDescriptorHull; // this is our descriptor table for this root parameter
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; // our vertex shader will be the only shader accessing this parameter for now 


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
	rootSignatureDesc.Init(_countof(rootParameters), rootParameters, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ID3DBlob* signature;
	hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);
	if (FAILED(hr))
	{

	}

	hr = ENGContext.GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_handle));
	if (FAILED(hr))
	{

	}
}