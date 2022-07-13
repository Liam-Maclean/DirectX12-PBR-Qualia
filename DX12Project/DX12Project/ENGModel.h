#pragma once
#include "ENGBuffer.h"
#include "ENGGraphicsStructs.h"
#include <string>
#include <vector>
#include <d3dcompiler.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <unordered_map>
#include "DXFrameHeapManager.h"
//GLM includes
#include "../Libs/glm/glm/glm.hpp"
#include "../Libs/glm/glm/common.hpp"
#include "../Libs/glm/glm/gtc/matrix_transform.hpp"
#include "../Libs/glm/glm/gtc/type_ptr.hpp"

#include "../Libs/tinygltf/tiny_gltf.h"

#define MAX_NUM_JOINTS 128u
struct Node;


struct LoaderInfo {
	uint32_t* indexData;
	Vertex* vertexData;
	size_t indexPos = 0;
	size_t vertexPos = 0;
};

struct ShaderMaterialLayout {
	glm::vec4 baseColorFactor;
	glm::vec4 emissiveFactor;
	glm::vec4 diffuseFactor;
	glm::vec4 specularFactor;
	int albedo_idx;
	int normal_idx;
	int metallic_idx;
	int occlusion_idx;
	int emissive_idx;
	float metallicFactor;
	float roughnessFactor;
	float alphaMask;
	float alphaMaskCutoff;
};

struct TextureSampler {
	//VkFilter magFilter;
	//VkFilter minFilter;
	//VkSamplerAddressMode addressModeU;
	//VkSamplerAddressMode addressModeV;
	//VkSamplerAddressMode addressModeW;
};

struct Texture {
	DX::Texture textureData;
	ENGBuffer* textureBuffer;
	uint32_t textureIndex;
	uint32_t width, height;
	uint32_t mipLevels;
	uint32_t layerCount;
	// Load a texture from a glTF image (stored as vector of chars loaded via stb_image) and generate a full mip chaing for it
	void FromglTfImage(tinygltf::Image& gltfimage);
};



struct Material {
	enum AlphaMode { ALPHAMODE_OPAQUE, ALPHAMODE_MASK, ALPHAMODE_BLEND };
	AlphaMode alphaMode = ALPHAMODE_OPAQUE;
	float alphaCutoff = 1.0f;
	float metallicFactor = 1.0f;
	float roughnessFactor = 1.0f;
	glm::vec4 baseColorFactor = glm::vec4(1.0f);
	glm::vec4 emissiveFactor = glm::vec4(1.0f);
	Texture* baseColorTexture;
	Texture* metallicRoughnessTexture;
	Texture* normalTexture;
	Texture* occlusionTexture;
	Texture* emissiveTexture;
	int baseColorTextureIndex = -1;
	int metallicRoughnessTextureIndex = -1;
	int normalTextureIndex = -1;
	int occlusionTextureIndex = -1;
	int emissiveTextureIndex = -1;

	struct TexCoordSets {
		uint8_t baseColor = 0;
		uint8_t metallicRoughness = 0;
		uint8_t specularGlossiness = 0;
		uint8_t normal = 0;
		uint8_t occlusion = 0;
		uint8_t emissive = 0;
	} texCoordSets;
	struct Extension {
		Texture* specularGlossinessTexture;
		Texture* diffuseTexture;
		glm::vec4 diffuseFactor = glm::vec4(1.0f);
		glm::vec3 specularFactor = glm::vec3(0.0f);
	} extension;
	struct PbrWorkflows {
		bool metallicRoughness = true;
		bool specularGlossiness = false;
	} pbrWorkflows;
	int materialIndex;
	//VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
};

struct SubMesh {
	uint32_t firstIndex;
	uint32_t indexCount;
	uint32_t vertexCount;
	Material& material;
	ENGBuffer* materialParamBuffer;
	//material
	bool hasIndices;
	SubMesh(uint32_t firstIndex, uint32_t indexCount, uint32_t vertexCount, Material& material);
};

struct Mesh
{
	std::vector<SubMesh*> submeshes;
	struct UniformBufferStruct
	{
		ENGBuffer* nodeMatrixBuffer;
		
	}uniformBuffer;

	struct UnformMVPBlock {
		glm::mat4 matrix;
		glm::mat4 jointMatrix[MAX_NUM_JOINTS]{};
		float jointCount{ 0 };
	}uniformMVPBlock;

	DXFrameHeapManager* m_frameHeap;
	DXDescriptorHeap m_cbvHeap;
	Mesh(glm::mat4 matrix);
};

struct Skin {
	std::string name;
	Node* skeletonRoot = nullptr;
	std::vector<glm::mat4> inverseBindMatrices;
	std::vector<Node*> joints;
};

struct Node
{

	Node* parent;
	uint32_t index;
	std::vector<Node*> children;
	std::string name;
	Mesh* mesh;
	Skin* skin;
	int32_t skinIndex;
	glm::mat4 matrix;

	glm::vec3 translation{};
	glm::vec3 scale{ 1.0f };
	glm::quat rotation{};

	glm::mat4 LocalMatrix();
	glm::mat4 GetMatrix();
	void Update();
	~Node();
};


struct ENGModel
{
	void LoadFromFile(std::string filename,  float scale);
	void GetNodeProps(const tinygltf::Node& node, const tinygltf::Model& model, size_t& vertexCount, size_t& indexCount);
	void LoadNode(Node* parent, const tinygltf::Node& node, uint32_t nodeIndex, const tinygltf::Model& model, LoaderInfo& loaderInfo, float globalscale);
	void BindDescriptors();
	void LoadSkins(tinygltf::Model& model);
	void LoadSamplers(tinygltf::Model& model);
	void LoadTextures(tinygltf::Model& model);
	void LoadMaterials(tinygltf::Model& model);
	void Draw(ID3D12GraphicsCommandList* commandList);
	void DrawNode(Node* node, ID3D12GraphicsCommandList* commandList);
	Node* NodeFromIndex(uint32_t index);
	Node* FindNode(Node* parent, uint32_t index);
	std::vector<Node*> nodes;
	std::vector<Node*> linearNodes;
	std::vector<Skin*> skins;
	std::vector<Texture> textures;
	std::vector<TextureSampler> textureSamplers;
	std::vector<Material> materials;
	std::vector<ShaderMaterialLayout> materialParams;
	std::vector<std::string> extensions;
	//std::vector<ENGBuffer*> materialParamBuffers;

	ENGBuffer* m_vertexBuffer;
	ENGBuffer* m_indexBuffer;
	int m_indexCount = 0;

	DXDescriptorHeap m_textureHeap;
	int m_srvHeapIndex;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
};

