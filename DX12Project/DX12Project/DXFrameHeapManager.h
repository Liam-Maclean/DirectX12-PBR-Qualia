#pragma once
#include "DirectXWrappers.h"

struct FrameHeapParameters
{
	int main;
	int GBuffer;
	int Entities;
	int Textures;
	int PostProcessSRVs;
	int cubemapSRV;
};

class DXFrameHeapManager
{
private:
	const D3D12_DESCRIPTOR_HEAP_TYPE gpuHeapType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	
	//We will just create 1024 descriptors (not sure if this is good)
	const int descriptorHeapSize = 1024;

	int preIndex;
	int currentHeapIndex;
	int frameStartIndex;
	ID3D12Device* device;
	DXDescriptorHeap gpuHeap;

public:
	
	DXFrameHeapManager(ID3D12Device* device);
	~DXFrameHeapManager();
	void StartFrame();
	void EndFrame();
	int CopyAllocate(int numDescriptors, DXDescriptorHeap descriptorHeap, int offset = 0);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentCPUHandle();
	D3D12_GPU_DESCRIPTOR_HANDLE GetCurrentGPUHandle();
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(int baseIndex, int offset = 0);
	ID3D12DescriptorHeap* GetDescriptorHeap();


};

