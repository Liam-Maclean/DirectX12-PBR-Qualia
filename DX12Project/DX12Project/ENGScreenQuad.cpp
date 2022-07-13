#include "ENGScreenQuad.h"
#include "DirectXWrappers.h"
#include "ENGGraphicsStructs.h"
#include "ENGRendererContext.h"

void ENGScreenQuad::Init()
{

	std::vector<ENGGraphicsStructs::NewVertex> vertexVector;
	vertexVector.push_back(ENGGraphicsStructs::NewVertex(-1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
	vertexVector.push_back(ENGGraphicsStructs::NewVertex(1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f));
	vertexVector.push_back(ENGGraphicsStructs::NewVertex(-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));
	vertexVector.push_back(ENGGraphicsStructs::NewVertex(1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f));

	CD3DX12_HEAP_PROPERTIES heapProperty(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC resourceDesc;
	ZeroMemory(&resourceDesc, sizeof(resourceDesc));
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment = 0;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Width = vertexVector.size() * sizeof(ENGGraphicsStructs::NewVertex);
	resourceDesc.Height = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	ENGContext.GetDevice()->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexBuffer));

	UINT8* dataBegin;
	vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&dataBegin));
	memcpy(dataBegin, vertexVector.data(), vertexVector.size() * sizeof(ENGGraphicsStructs::NewVertex));
	vertexBuffer->Unmap(0, nullptr);

	mVbView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	mVbView.StrideInBytes = sizeof(ENGGraphicsStructs::NewVertex);
	mVbView.SizeInBytes = (vertexVector.size() * sizeof(ENGGraphicsStructs::NewVertex));
}

void ENGScreenQuad::Draw(ID3D12GraphicsCommandList* commandList)
{
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	commandList->IASetVertexBuffers(0, 1, &mVbView);
	commandList->DrawInstanced(4, 1, 0, 0);

}
