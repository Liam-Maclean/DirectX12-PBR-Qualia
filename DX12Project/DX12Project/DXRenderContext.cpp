#include "ENGRendererContext.h"
#include "ENGBase.h"
#include "ENGSwapchain.h"
#include "cvars.h"

extern AutoCVar_Int CVAR_ResolutionWidth;
extern AutoCVar_Int CVAR_ResolutionHeight;

#define DX12_FEATURE_LEVEL D3D_FEATURE_LEVEL_12_1

ENGRendererContext::ENGRendererContext()
{

}

void ENGRendererContext::Init(HWND hwnd)
{
	CreateDevice();
	CreateQueues();
	CreateSwapchain(hwnd);
	CreateCommandList();
	CreateFences();
}

void ENGRendererContext::CreateDevice()
{
	//set the feature level to DirectX 12.1 to enable all DX12 features.
	//Not all cards support full DX12,  this feature level may need  to be reduced on some cards to 12.0

	IDXGIFactory4* factory;
	IDXGIAdapter* adapter;

	HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface));
	if (FAILED(hr))
	{

	}
	debugInterface->QueryInterface(IID_PPV_ARGS(&debugInterface1));
	debugInterface->EnableDebugLayer();
	//debugInterface->EnableDebugLayer();
	debugInterface1->SetEnableGPUBasedValidation(true);
	
	//Create D3D12 device
	hr = D3D12CreateDevice(NULL, DX12_FEATURE_LEVEL, IID_PPV_ARGS(&m_device));
	if (FAILED(hr))
	{
		//static_assert(true);
	}


	//Create a DirectX graphics interface factory
	hr = CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&factory);
	if (FAILED(hr))
	{
		//return false;
	}

	//Use factory to create an adaptor for the graphics card
	hr = factory->EnumAdapters(0, &adapter);
	if (FAILED(hr))
	{
		//return false;
	}


	DXGI_ADAPTER_DESC adapterDesc;
	unsigned long long stringLength;

	//Get adapter (video card) description)
	hr = adapter->GetDesc(&adapterDesc);
	if (FAILED(hr))
	{
		//return false;
	}

	//store the amount of dedicated video card memory in megabytes
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//Convert the name of the video card to a character array and store it
	int  error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{

	}
}


void ENGRendererContext::CreateQueues()
{
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
	//Initialize the description of the command queue
	ZeroMemory(&commandQueueDesc, sizeof(commandQueueDesc));

	//Set up the description of the command queue.
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.NodeMask = 0;

	//Create the command queue
	HRESULT result = m_device->CreateCommandQueue(&commandQueueDesc, __uuidof(ID3D12CommandQueue), (void**)&m_commandQueue);
	if (FAILED(result))
	{
		//return false;
	}


	D3D12_COMMAND_QUEUE_DESC computeQueueDesc;
	//Initialize the description of the command queue
	ZeroMemory(&computeQueueDesc, sizeof(computeQueueDesc));

	//Set up the description of the command queue.
	computeQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
	computeQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	computeQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	computeQueueDesc.NodeMask = 0;

	//Create the command queue
	result = m_device->CreateCommandQueue(&computeQueueDesc, __uuidof(ID3D12CommandQueue), (void**)&m_computeQueue);
	if (FAILED(result))
	{
		//return false;
	}
}

void ENGRendererContext::CreateSwapchain(HWND hwnd)
{
	m_swapChain = ENGSwapchain::Create( hwnd, CVAR_ResolutionHeight.Get(), CVAR_ResolutionWidth.Get());
}

void ENGRendererContext::CreateCommandList()
{
	HRESULT result;
	//create a command allocator
	for (int i = 0; i < 3; i++)
	{
		result = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator[i]));
		if (FAILED(result))
		{
			//return false;
		}


		//result = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&m_computeCommandAllocator[i]));
		//if (FAILED(result))
		//{
		//	//return false;
		//}
	}

	//Create a basic command list
	result = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator[0], NULL, IID_PPV_ARGS(&m_commandList));
	if (FAILED(result))
	{
		//return false;
	}

	//result = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, m_computeCommandAllocator[0], NULL, IID_PPV_ARGS(&m_computeList));
	//if (FAILED(result))
	//{
	//	return false;
	//}
	//return true;
}

void ENGRendererContext::CreateFences()
{
	////Create a fence for GPU synchronization
	//for (int i = 0; i < 3; i++)
	//{
	//	HRESULT result = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence[i]));
	//	if (FAILED(result))
	//	{
	//	}
	//	//Initialize the starting fence value
	//	m_fenceValue[i] = 0;
	//}
	////Create an event object for the fence.
	//m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	//if (m_fenceEvent == nullptr)
	//{
	//
	//}
}