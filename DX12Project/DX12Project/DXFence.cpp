#include "ENGFence.h"
#include "ENGRendererContext.h"

ENGFence::ENGFence()
{
	HRESULT hr;

	//Create a fence for GPU synchronization
	hr = ENGContext.GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_handle));

	//Initialize the starting fence value
	m_fenceValue = 0;
	
	//Create an event object for the fence.
	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_fenceEvent == nullptr)
	{
		//return false;
	}
}

ENGFence::~ENGFence()
{

}

void ENGFence::IncrementFenceValue()
{
	m_fenceValue++;
}

void ENGFence::Signal()
{
	HRESULT hr;
	hr = ENGContext.GetCommandQueue()->Signal(m_handle, m_fenceValue);
	if (FAILED(hr))
	{

	}
}

void ENGFence::WaitForFenceEvent()
{
	HRESULT hr;
	//if the current fance value is less than fence value, we know GPU is not finished executing
	if (m_handle->GetCompletedValue() < m_fenceValue)
	{
		//We have the fence create an event which is signaled once the fence's current value is fence value
		hr = m_handle->SetEventOnCompletion(m_fenceValue, m_fenceEvent);
		if (FAILED(hr))
		{

		}

		//we will wait until the fence has triggered the event htat its current value has reached fence value
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}

	//incremement fence value for next frame
	m_fenceValue++;
}
