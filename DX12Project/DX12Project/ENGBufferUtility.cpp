#include "ENGBufferUtility.h"
#include "ENGRendererContext.h"
void ENGBufferUtility::CreateBufferDefaultToUploadHeap(ID3D12GraphicsCommandList* commandList, ID3D12Resource*& buffer, int bufferSize, D3D12_SUBRESOURCE_DATA bufferData)
{
	HRESULT hr;

	hr = ENGContext.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(bufferSize), D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&buffer));
	if (FAILED(hr))
	{
		
	}

	ID3D12Resource* bufferUploadHeap;
	hr = ENGContext.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(bufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&bufferUploadHeap));
	if (FAILED(hr))
	{

	}

	UpdateSubresources(commandList, buffer, bufferUploadHeap, 0, 0, 1, &bufferData);

	bufferUploadHeap = 0;
	delete bufferUploadHeap;
}

void ENGBufferUtility::CreateVertexBuffer(ID3D12GraphicsCommandList* commandList, ID3D12Resource*& buffer, int bufferSize, D3D12_SUBRESOURCE_DATA bufferData)
{

	HRESULT hr;

	hr = ENGContext.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(bufferSize), D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&buffer));
	if (FAILED(hr))
	{

	}

	ID3D12Resource* bufferUploadHeap;
	hr = ENGContext.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(bufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&bufferUploadHeap));
	if (FAILED(hr))
	{

	}

	UpdateSubresources(commandList, buffer, bufferUploadHeap, 0, 0, 1, &bufferData);



	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(buffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

	bufferUploadHeap = 0;
	delete bufferUploadHeap;
}

void ENGBufferUtility::CreateIndexBuffer(ID3D12GraphicsCommandList* commandList, ID3D12Resource*& buffer, int bufferSize, D3D12_SUBRESOURCE_DATA bufferData)
{
	HRESULT hr;

	hr = ENGContext.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(bufferSize), D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&buffer));
	if (FAILED(hr))
	{

	}

	ID3D12Resource* bufferUploadHeap;
	hr = ENGContext.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(bufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&bufferUploadHeap));
	if (FAILED(hr))
	{

	}

	UpdateSubresources(commandList, buffer, bufferUploadHeap, 0, 0, 1, &bufferData);

	bufferUploadHeap = 0;
	delete bufferUploadHeap;

}

void ENGBufferUtility::CreateBufferTexture(ID3D12GraphicsCommandList* commandList, LPCWSTR filename, ID3D12Resource*& buffer, D3D12_RESOURCE_DESC& textureDesc)
{
	HRESULT hr;
	int imageBytesPerRow;
	BYTE* imageData;

	int imageSize = ENGTextureLoader::LoadImageDataFromFile(&imageData, textureDesc, filename, imageBytesPerRow);
	if (imageSize <= 0)
	{

	}

	hr = ENGContext.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&buffer));
	if (FAILED(hr))
	{

	}

	ID3D12Resource* bufferUploadHeap;
	UINT64 textureUploadBufferSize;
	ENGContext.GetDevice()->GetCopyableFootprints(&textureDesc, 0, 1, 0, nullptr, nullptr, nullptr, &textureUploadBufferSize);
	hr = ENGContext.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(textureUploadBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&bufferUploadHeap));
	if (FAILED(hr))
	{

	}

	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData = &imageData[0];
	textureData.RowPitch = imageBytesPerRow;
	textureData.SlicePitch = imageBytesPerRow * textureDesc.Height;

	UpdateSubresources(commandList, buffer, bufferUploadHeap, 0, 0, 1, &textureData);

	imageData = 0;
	delete imageData;

	bufferUploadHeap = 0;
	delete bufferUploadHeap;
}
