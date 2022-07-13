#pragma once
#include <d3d12.h>
#include "ENGBuffer.h"
class ENGScreenQuad
{
public:
	void Init();
	void Draw(ID3D12GraphicsCommandList* commandList);

private:
	ID3D12Resource* vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW mVbView;
};

