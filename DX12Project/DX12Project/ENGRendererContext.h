#pragma once
#define ENGContext (*ENGRendererContext::Get())
#include <Windows.h>
#include "ENGSwapchain.h"
class ENGRendererContext
{
	class Device;
public:
	static ENGRendererContext *Get()
	{
		static auto renderContext = new ENGRendererContext();
		return renderContext;
	}

	void Init(HWND hwnd);
	void CreateDevice();
	void CreateQueues();
	void CreateSwapchain(HWND hwnd);
	void CreateCommandList();
	void CreateFences();


	ID3D12Device* GetDevice() { return m_device; }
	ID3D12GraphicsCommandList* GetCommandList() { return m_commandList; }
	IDXGISwapChain3* GetSwapchain() { return m_swapChain->Handle(); }
	ID3D12CommandAllocator* GetAllocator(int bufferIndex) { return m_commandAllocator[bufferIndex]; }
	ID3D12CommandQueue* GetCommandQueue() { return m_commandQueue; }
private:

	ENGRendererContext();
	ID3D12Debug* debugInterface;
	ID3D12Debug1* debugInterface1;

	ENGSwapchain* m_swapChain;
	ID3D12Device* m_device;
	unsigned int m_videoCardMemory;
	char m_videoCardDescription[128];


	ID3D12CommandQueue* m_commandQueue;
	ID3D12CommandQueue* m_computeQueue;

	ID3D12CommandAllocator* m_commandAllocator[3];
	ID3D12GraphicsCommandList* m_commandList;

	ID3D12Fence* m_fence[3];
	HANDLE m_fenceEvent;
	unsigned long long m_fenceValue[3];


	int m_frameIndex;
};