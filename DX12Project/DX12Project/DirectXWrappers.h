#pragma once
#include <Windows.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <wrl/client.h>
#include <d3dcompiler.h>

class DXDescriptorHeap
{
public:
	DXDescriptorHeap() { memset(this, 0, sizeof(*this)); }

	HRESULT Create(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, bool bShaderVisible = false)
	{
		HRESULT hr;

		//Set up heap descriptor
		HeapDesc.Type = type;
		HeapDesc.NumDescriptors = numDescriptors;
		HeapDesc.Flags = bShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE :  D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		//Create descriptor heap
		hr = device->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&pDescriptorHeap));

		hCPUHeapStart = pDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		if (bShaderVisible)
		{
			hGPUHeapStart = pDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		}
		else
		{
			hGPUHeapStart.ptr = 0;
		}

		HandleIncrementSize = device->GetDescriptorHandleIncrementSize(HeapDesc.Type);
		return hr;
	}
	operator ID3D12DescriptorHeap* () { return pDescriptorHeap.Get(); }

	UINT64 MakeOffsetted(UINT64 ptr, UINT index)
	{
		UINT64 offsetted;
		offsetted = ptr + static_cast<UINT64>(index * HandleIncrementSize);
		return offsetted;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU(UINT index)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle;
		handle.ptr = MakeOffsetted(hCPUHeapStart.ptr, index);
		return handle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU(UINT index)
	{
		D3D12_GPU_DESCRIPTOR_HANDLE handle;
		handle.ptr = MakeOffsetted(hGPUHeapStart.ptr, index);
		return handle;
	}

	D3D12_DESCRIPTOR_HEAP_DESC HeapDesc;

	D3D12_CPU_DESCRIPTOR_HANDLE hCPUHeapStart;
	D3D12_GPU_DESCRIPTOR_HANDLE hGPUHeapStart;
	UINT HandleIncrementSize;

public: 
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pDescriptorHeap;
};

