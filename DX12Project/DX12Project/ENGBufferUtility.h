#pragma once
#include <d3dcompiler.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <wincodec.h>

#include "d3dx12.h"
#include "ENGTextureLoader.h"

class ENGBufferUtility
{
public:
	static void CreateBufferDefaultToUploadHeap(ID3D12GraphicsCommandList* commandList, ID3D12Resource* &buffer, int bufferSize, D3D12_SUBRESOURCE_DATA bufferData);
	static void CreateVertexBuffer(ID3D12GraphicsCommandList* commandList, ID3D12Resource*& buffer, int bufferSize, D3D12_SUBRESOURCE_DATA bufferData);
	static void CreateIndexBuffer(ID3D12GraphicsCommandList* commandList, ID3D12Resource*& buffer, int bufferSize, D3D12_SUBRESOURCE_DATA bufferData);
	static void CreateBufferTexture( ID3D12GraphicsCommandList* commandList, LPCWSTR filename, ID3D12Resource*& buffer, D3D12_RESOURCE_DESC& textureDesc);
};

