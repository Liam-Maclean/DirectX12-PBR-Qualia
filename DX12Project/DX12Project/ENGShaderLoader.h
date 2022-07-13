#pragma once
#include <d3d12.h>
#include <string>
#include <d3dcompiler.h>

class ENGShaderLoader
{
public:
	static D3D12_SHADER_BYTECODE LoadShader(std::wstring shaderName);
};

