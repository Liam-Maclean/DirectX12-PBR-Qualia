#pragma once
#include <d3dcompiler.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <vector>
#include <string>
#include "../Libs/glm/glm/glm.hpp"

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv0;
	glm::vec2 uv1;
	glm::vec4 joint0;
	glm::vec4 weight0;
};

struct ShaderDebugValues {
	int HDREnabled =1;
	int GammaCorrectionEnabled=1;
	int viewMode = 0;
};
namespace ENGGraphicsStructs
{



	struct Vertex {
		//Default constructor
		Vertex() : pos(0,0,0), texCoord(0,0) {};

		//Override constructor
		Vertex(float x, float y, float z, float u, float v) : pos(x, y, z), texCoord(u, v) {}

		glm::vec3 pos;
		glm::vec2 texCoord;
	};

	struct NewVertex
	{
		//Default constructor
		NewVertex() : pos(0, 0, 0), normal(0,0,0), texCoord(0, 0) {};

		//Override constructor
		NewVertex(float posX, float posY, float posZ, float normX, float normY, float normZ, float u, float v) 
			: pos(posX, posY, posZ), normal(normX, normY, normZ), texCoord(u, v) {}

		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 texCoord;
	};

	struct DirectionalLight
	{
		glm::vec4 direction;
		glm::vec4 color;
	};

	struct Mesh
	{
		ID3D12Resource* vertexBuffer;
		ID3D12Resource* indexBuffer;

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
		D3D12_INDEX_BUFFER_VIEW indexBufferView;

		std::vector<NewVertex> vertices;
		std::vector<DWORD> indices;

		inline int GetNumIndices() { return indices.size(); }
		inline int GetVertexBufferSize() { return vertices.size() * sizeof(NewVertex); }
		inline int GetIndexBufferSize() { return indices.size() * sizeof(DWORD); }

		void SetBufferView()
		{
			vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
			vertexBufferView.StrideInBytes = sizeof(ENGGraphicsStructs::NewVertex);
			vertexBufferView.SizeInBytes = GetVertexBufferSize();

			indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
			indexBufferView.Format = DXGI_FORMAT_R32_UINT;
			indexBufferView.SizeInBytes = GetIndexBufferSize();
		}
	};

	struct Model
	{
		Mesh modelMesh;
	};

	struct Material
	{
		std::string name;
		//XMFLOAT3 ambientColor; //Ka
		//XMFLOAT3 diffuseColor; //Kd
		//XMFLOAT3 specularColor; //Kd
		//XMFLOAT3 emmisive; //Ke
		float opticalDensity; //Ni
		float specularExponent; //Ns
		float transparency; //d or Tr (0.0 to 1.0)
		float roughness; //Pr
		float metallic; //Pm
		float sheen; //Ps
	};
};

