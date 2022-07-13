#include "ENGShaderLoader.h"

D3D12_SHADER_BYTECODE ENGShaderLoader::LoadShader(std::wstring shaderName)
{
	ID3DBlob* shaderBlob;
	D3DReadFileToBlob(shaderName.c_str(), &shaderBlob);
	D3D12_SHADER_BYTECODE byteCode;
	byteCode.BytecodeLength = shaderBlob->GetBufferSize();
	byteCode.pShaderBytecode = shaderBlob->GetBufferPointer();
	return byteCode;
}
