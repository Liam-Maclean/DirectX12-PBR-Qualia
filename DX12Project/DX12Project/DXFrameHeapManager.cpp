#include "DXFrameHeapManager.h"

DXFrameHeapManager::DXFrameHeapManager(ID3D12Device* device)
{
	this->device = device;
	gpuHeap.Create(device, gpuHeapType, descriptorHeapSize, TRUE);
	currentHeapIndex = 0;
	preIndex = 0;
	frameStartIndex = 0;
}

DXFrameHeapManager::~DXFrameHeapManager()
{
}

void DXFrameHeapManager::StartFrame()
{
	frameStartIndex = currentHeapIndex;
}

void DXFrameHeapManager::EndFrame()
{
}

int DXFrameHeapManager::CopyAllocate(int numDescriptors, DXDescriptorHeap descriptorHeap, int offset)
{
	auto futureIndex = currentHeapIndex + numDescriptors;
	if (futureIndex >= descriptorHeapSize)
	{
		currentHeapIndex = 0;
		frameStartIndex = 0;
	}

	preIndex = currentHeapIndex;
	
	//Add on the new descriptor heap onto the end of the current index
	device->CopyDescriptorsSimple(numDescriptors, gpuHeap.handleCPU(currentHeapIndex), descriptorHeap.handleCPU(offset), gpuHeapType);
	currentHeapIndex += numDescriptors;
	return preIndex;
}

D3D12_CPU_DESCRIPTOR_HANDLE DXFrameHeapManager::GetCurrentCPUHandle()
{
	return gpuHeap.handleCPU(currentHeapIndex);
}

D3D12_GPU_DESCRIPTOR_HANDLE DXFrameHeapManager::GetCurrentGPUHandle()
{
	return gpuHeap.handleGPU(currentHeapIndex);
}

D3D12_GPU_DESCRIPTOR_HANDLE DXFrameHeapManager::GetGPUHandle(int baseIndex, int offset)
{
	return gpuHeap.handleGPU(baseIndex + offset);
}

ID3D12DescriptorHeap* DXFrameHeapManager::GetDescriptorHeap()
{
	return gpuHeap.pDescriptorHeap.Get();
}
