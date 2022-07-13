#pragma once
#include "ENGBase.h"
#include <d3d12.h>
class ENGShaderLayout : public IHandle<ENGShaderLayout, ID3D12RootSignature*>
{
public:
	ENGShaderLayout();
	~ENGShaderLayout();

	void GetDefferredShaderLayout();
	void GetTessellationShaderLayout();
	void GetBlinnPhongShaderLayout();
	void GetFXAAShaderLayout();
private:
};

