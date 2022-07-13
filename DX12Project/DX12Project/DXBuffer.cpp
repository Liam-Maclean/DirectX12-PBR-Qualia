#pragma once
#include "ENGBuffer.h"
#include "ENGRendererContext.h"
#include "d3dx12.h"
#include "ENGTextureLoader.h"
#include "cvars.h"
extern AutoCVar_Int CVAR_ResolutionWidth;
extern AutoCVar_Int CVAR_ResolutionHeight;

ENGBuffer::ENGBuffer(size_t size, void* data, D3D12_RESOURCE_STATES bufferState)
{
	m_data = data;
	m_size = size;

	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData = static_cast<BYTE*>(data);
	subResourceData.RowPitch = m_size;
	subResourceData.SlicePitch = m_size;
	HRESULT hr;

	hr = ENGContext.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(m_size), D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_handle));
	if (FAILED(hr))
	{

	}

	//m_handle->SetName(L"Depth/Stencil Resource Heap");
	ID3D12Resource* bufferUploadHeap;
	hr = ENGContext.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(m_size), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&bufferUploadHeap));
	if (FAILED(hr))
	{

	}

	UpdateSubresources(ENGContext.GetCommandList(),	m_handle, bufferUploadHeap, 0, 0, 1, &subResourceData);



	ENGContext.GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_handle, D3D12_RESOURCE_STATE_COPY_DEST, bufferState));

	bufferUploadHeap = 0;
	delete bufferUploadHeap;
}

ENGBuffer::ENGBuffer(size_t size)
{
	HRESULT hr = ENGContext.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(size), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_handle));
	if (FAILED(hr))
	{

	}
}

ENGBuffer::ENGBuffer(D3D12_CLEAR_VALUE clearValue)
{

	HRESULT hr = ENGContext.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D24_UNORM_S8_UINT, CVAR_ResolutionWidth.Get(), CVAR_ResolutionHeight.Get(),1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL), D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_PPV_ARGS(&m_handle));
	if (FAILED(hr))
	{

	}

	m_handle->SetName(L"Depth/Stencil Resource Heap");
}

ENGBuffer::ENGBuffer(D3D12_CLEAR_VALUE clearValue, D3D12_RESOURCE_DESC resourceDesc, D3D12_RESOURCE_STATES resourceState)
{
	HRESULT hr = ENGContext.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &resourceDesc, resourceState, &clearValue, IID_PPV_ARGS(&m_handle));
	if (FAILED(hr))
	{

	}

	m_handle->SetName(L"Depth/Stencil Resource Heap");
}

ENGBuffer::ENGBuffer(D3D12_RESOURCE_DESC resourceDesc, D3D12_RESOURCE_STATES resourceState)
{
	HRESULT hr = ENGContext.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &resourceDesc, resourceState, nullptr, IID_PPV_ARGS(&m_handle));
	if (FAILED(hr))
	{

	}

	m_handle->SetName(L"Depth/Stencil Resource Heap");
}

ENGBuffer::ENGBuffer(DX::Texture texture)
{
	HRESULT hr = ENGContext.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &texture.textureDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_handle));
	if (FAILED(hr))
	{

	}
	//ENGContext.GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_handle, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));



	ID3D12Resource* bufferUploadHeap;
	UINT64 textureUploadBufferSize;
	ENGContext.GetDevice()->GetCopyableFootprints(&texture.textureDesc, 0, 1, 0, nullptr, nullptr, nullptr, &textureUploadBufferSize);
	hr = ENGContext.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(textureUploadBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&bufferUploadHeap));
	if (FAILED(hr))
	{

	}

	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData = &texture.imageData[0];
	textureData.RowPitch = texture.imageBytesPerRow;
	textureData.SlicePitch = texture.imageBytesPerRow * texture.textureDesc.Height;

	UpdateSubresources(ENGContext.GetCommandList(), m_handle, bufferUploadHeap, 0, 0, 1, &textureData);


	ENGContext.GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_handle, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));


	bufferUploadHeap = 0;
	delete bufferUploadHeap;
}

ENGBuffer::~ENGBuffer()
{

}

void ENGBuffer::Map()
{
	CD3DX12_RANGE readRange(0, 0);
	HRESULT hr = Handle()->Map(0, &readRange, reinterpret_cast<void**>(&m_mappingAddress));
	if (FAILED(hr))
	{

	}
}

void ENGBuffer::UnMap()
{
	CD3DX12_RANGE readRange(0, 0);
	Handle()->Unmap(0, &readRange);
}

void ENGBuffer::CopyData(void* inData, size_t inSize, size_t offset)
{
	m_data = inData;
	//if (!m_data)
		//PE_ERROR("Buffer::CopyDataRaw: Buffer is not mapped");
	//if (offset + inSize > m_size)
		//PE_ERROR("Buffer::CopyDataRaw: Source data size is too large");

	memcpy(m_mappingAddress + offset, m_data, inSize);
}

void ENGBuffer::CopyBuffer(ENGBuffer* inBuffer, size_t inSize, size_t offset)
{

}

void ENGBuffer::Flush(size_t offset, size_t flushSize)
{

}