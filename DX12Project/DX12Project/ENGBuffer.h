#pragma once
#include "ENGBase.h"
#include <d3d12.h>
#include "d3dx12.h"
#include "DXStructs.h"
class ENGBuffer : public IHandle<ENGBuffer, ID3D12Resource*>
{
public:
	ENGBuffer(size_t size, void* data, D3D12_RESOURCE_STATES bufferState);
	ENGBuffer(size_t size);
	ENGBuffer(D3D12_CLEAR_VALUE clearValue, D3D12_RESOURCE_DESC resourceDesc, D3D12_RESOURCE_STATES resourceState);
	ENGBuffer(D3D12_RESOURCE_DESC resourceDesc, D3D12_RESOURCE_STATES resourceState);
	ENGBuffer(D3D12_CLEAR_VALUE clearValue);
	ENGBuffer(DX::Texture texture);
	~ENGBuffer();

	void Map();
	void UnMap();

	void CopyData(void* inData, size_t inSize = 0, size_t offset = 0);
	void CopyBuffer(ENGBuffer* inBuffer, size_t inSize = 0, size_t offset = 0);

	void Flush(size_t offset = 0, size_t flushSize = 0);
	

	size_t GetSize() { return m_size; }

	void* GetData() { return m_data; }
private:
	void* m_data;
	UINT8* m_mappingAddress;
	size_t m_size;
	char* m_description;
	
	D3D12_CONSTANT_BUFFER_VIEW_DESC	descBuffer;
};