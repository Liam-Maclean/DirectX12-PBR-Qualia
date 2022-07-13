#pragma once
#include <string>
#include <vector>
#include <d3dcompiler.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <unordered_map>
#include "ENGGraphicsStructs.h"

class ENGModelLoader
{
public:
	static bool LoadObjModel(std::wstring filename, std::vector<ENGGraphicsStructs::NewVertex>& outVertices, std::vector<DWORD>& outIndices, bool isRhCoordSys, bool computeNormals);
	static bool LoadObjModel(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, std::wstring filename, ENGGraphicsStructs::Mesh& outMesh, bool isRhCoordSys, bool computeNormals);
	static bool TinyObjloadModel(std::string modelPath, std::string materialPath, ENGGraphicsStructs::Model& outMesh);
	static bool LoadObj(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, std::string filename, ENGGraphicsStructs::Mesh& outMesh, bool isRhCoordSys, bool computeNormals);
};

