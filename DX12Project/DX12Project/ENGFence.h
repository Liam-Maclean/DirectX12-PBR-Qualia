#pragma once
#include "ENGBase.h"
#include <d3d12.h>
class ENGFence : public IHandle<ENGFence, ID3D12Fence*>
{
public:
	ENGFence();
	~ENGFence();

	void IncrementFenceValue();
	void Signal();
	void WaitForFenceEvent();
private:
	unsigned long long m_fenceValue;
	HANDLE m_fenceEvent;


};

