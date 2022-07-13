#pragma once
#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define STB_IMAGE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#include "ENGModel.h"
#include "ENGRendererContext.h"




SubMesh::SubMesh(uint32_t firstIndex, uint32_t indexCount, uint32_t vertexCount, Material& material) : firstIndex(firstIndex), indexCount(indexCount), vertexCount(vertexCount), material(material) 
{
	hasIndices = indexCount > 0;

	ShaderMaterialLayout materialLayoutParams = {};
	materialLayoutParams.baseColorFactor = material.baseColorFactor;
	materialLayoutParams.emissiveFactor = material.emissiveFactor;
	materialLayoutParams.roughnessFactor = material.roughnessFactor;
	materialLayoutParams.metallicFactor = material.metallicFactor;
	materialLayoutParams.albedo_idx = material.baseColorTextureIndex;
	materialLayoutParams.emissive_idx = material.emissiveTextureIndex;
	materialLayoutParams.normal_idx = material.normalTextureIndex;
	materialLayoutParams.occlusion_idx = material.occlusionTextureIndex;
	materialLayoutParams.metallic_idx = material.metallicRoughnessTextureIndex;
	

	materialParamBuffer = ENGBuffer::Create(sizeof(ShaderMaterialLayout));
	materialParamBuffer->Map();
	materialParamBuffer->CopyData(&materialLayoutParams, sizeof(ShaderMaterialLayout), 0);

}

Mesh::Mesh(glm::mat4 matrix)
{
	//Create the buffer and start mapping to it 
	uniformMVPBlock.matrix = matrix;
	uniformBuffer.nodeMatrixBuffer = ENGBuffer::Create(sizeof(uniformMVPBlock));
	uniformBuffer.nodeMatrixBuffer->Map();
	uniformBuffer.nodeMatrixBuffer->CopyData(&uniformMVPBlock, sizeof(uniformMVPBlock), 0);


}

// Node
glm::mat4 Node::LocalMatrix() {
	return glm::translate(glm::mat4(1.0f), translation) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), scale) * matrix;
}

glm::mat4 Node::GetMatrix() {
	glm::mat4 m = LocalMatrix();
	Node* p = parent;
	while (p) {
		m = p->LocalMatrix() * m;
		p = p->parent;
	}
	return m;
}

void Node::Update() {
	if (mesh) {
		glm::mat4 m = GetMatrix();
		m = glm::transpose(m);
		if (skin) {
			mesh->uniformMVPBlock.matrix = m;
			//mesh->uniformMVPBlock.matrix = glm::transpose(mesh->uniformMVPBlock.matrix);
			// Update join matrices
			glm::mat4 inverseTransform = glm::inverse(m);
			size_t numJoints = std::min((uint32_t)skin->joints.size(), MAX_NUM_JOINTS);
			for (size_t i = 0; i < numJoints; i++) {
				Node* jointNode = skin->joints[i];
				glm::mat4 jointMat = jointNode->GetMatrix() * skin->inverseBindMatrices[i];
				jointMat = inverseTransform * jointMat;
				mesh->uniformMVPBlock.jointMatrix[i] = jointMat;
				//mesh->uniformMVPBlock.jointMatrix[i] = glm::transpose(mesh->uniformMVPBlock.jointMatrix[i]);
			}
			mesh->uniformMVPBlock.jointCount = (float)numJoints;
			//mesh->uniformBuffer.buffer->CopyData(&mesh->uniformMVPBlock, sizeof(mesh->uniformMVPBlock), 0);
			//memcpy(mesh->uniformBuffer.mappingAddress, &mesh->uniformMVPBlock, sizeof(mesh->uniformMVPBlock));
		}
		else {
			mesh->uniformBuffer.nodeMatrixBuffer->CopyData(&m, sizeof(glm::mat4), 0);

			//memcpy(mesh->uniformBuffer.mappingAddress, &m, sizeof(glm::mat4));
		}
	}

	for (auto& child : children) {
		child->Update();
	}
}

void ENGModel::LoadFromFile(std::string filename, float scale)
{
	tinygltf::Model gltfModel;
	tinygltf::TinyGLTF gltfContext;
	std::string error;
	std::string warning;

	//Figure out if the file is binary or ASCII
	bool binary = false;
	size_t extpos = filename.rfind('.', filename.length());
	if (extpos != std::string::npos)
	{
		binary = (filename.substr(extpos + 1, filename.length() - extpos) == "glb");
	}

	//load the file either from binary or from the ASCII file itself 
	bool fileLoaded = binary ? gltfContext.LoadBinaryFromFile(&gltfModel, &error, &warning, filename.c_str()) : gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, filename.c_str());
	LoaderInfo loaderInfo{};
	size_t vertexCount = 0;
	size_t indexCount = 0;

	if (fileLoaded)
	{
		//Load Samplers
		LoadSamplers(gltfModel);
		//Load Textures
		LoadTextures(gltfModel);
		//Load materials
		LoadMaterials(gltfModel);

		//for (int i = 0; i < materials.size(); ++i)
		//{
		//	ShaderMaterialLayout materialLayoutParams = {};
		//	materialLayoutParams.baseColorFactor = materials[i].baseColorFactor;
		//	materialLayoutParams.albedo_idx = materials[i].baseColorTextureIndex;
		//	materialLayoutParams.emissive_idx = materials[i].emissiveTextureIndex;
		//	materialLayoutParams.normal_idx = materials[i].normalTextureIndex;
		//	materialLayoutParams.occlusion_idx = materials[i].occlusionTextureIndex;
		//	materialLayoutParams.metallic_idx = materials[i].metallicRoughnessTextureIndex;
		//	materialParams.push_back(materialLayoutParams);
		//
		//	ENGBuffer* materialCBVBuffer;
		//	materialCBVBuffer = ENGBuffer::Create(sizeof(ShaderMaterialLayout));
		//	materialCBVBuffer->Map();
		//	materialCBVBuffer->CopyData(&materialLayoutParams, sizeof(ShaderMaterialLayout), 0);
		//	materialParamBuffers.push_back(materialCBVBuffer);
		//
		//}




		const tinygltf::Scene& scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];

		for (size_t i = 0; i < scene.nodes.size(); ++i)
		{
			GetNodeProps(gltfModel.nodes[scene.nodes[i]], gltfModel, vertexCount, indexCount);
		}
		//allocate an array of new vertices and indices with the count from the file
		loaderInfo.vertexData = new Vertex[vertexCount];
		loaderInfo.indexData = new uint32_t[indexCount];


		//set the vertex buffer to the new vertex count
		//Set the index buffer to the new index count
		for (size_t i = 0; i < scene.nodes.size(); ++i)
		{
			const tinygltf::Node node = gltfModel.nodes[scene.nodes[i]];
			LoadNode(nullptr, node, scene.nodes[i], gltfModel, loaderInfo, scale);
			//Load the node (geometry)
		}
		//Load animations here, commented out for now we don't need them
		if (gltfModel.animations.size() > 0)
		{
			//load animations
		}
		LoadSkins(gltfModel);
		for (auto node : linearNodes) {
			// Assign skins
			if (node->skinIndex > -1) {
				node->skin = skins[node->skinIndex];
			}
			// Initial pose
			if (node->mesh) {
				node->Update();
			}
		}
		//Load the skins here?
	}
	else
	{
		//Could not load the gltf file
		return;
	}
	m_textureHeap.Create(ENGContext.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, textures.size(), true);

	////extensions = gltfModel.extensionsUsed;
	for (int i = 0; i < textures.size(); ++i)
	{
		ENGContext.GetDevice()->CreateShaderResourceView(textures[i].textureBuffer->Handle(), &textures[i].textureData.shaderResourceView, m_textureHeap.handleCPU(m_srvHeapIndex));
		textures[i].textureIndex = m_srvHeapIndex;
		m_srvHeapIndex++;
	}

	size_t vertexBufferSize = vertexCount * sizeof(Vertex);
	size_t indexBufferSize = indexCount * sizeof(uint32_t);
	m_indexCount = indexCount;

	m_vertexBuffer = ENGBuffer::Create(vertexBufferSize,  reinterpret_cast<void*>(loaderInfo.vertexData), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	m_indexBuffer = ENGBuffer::Create(indexBufferSize, reinterpret_cast<void*>(loaderInfo.indexData), D3D12_RESOURCE_STATE_INDEX_BUFFER);

	vertexBufferView.BufferLocation = m_vertexBuffer->Handle()->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = sizeof(Vertex);
	vertexBufferView.SizeInBytes = vertexBufferSize;

	indexBufferView.BufferLocation = m_indexBuffer->Handle()->GetGPUVirtualAddress();
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	indexBufferView.SizeInBytes = indexBufferSize;

}

void ENGModel::GetNodeProps(const tinygltf::Node& node, const tinygltf::Model& model, size_t& vertexCount, size_t& indexCount)
{
	//get node props ? whatever this is I think it's to do with grabbing the objects and setting the overall vertex count 
	if (node.children.size() > 0) {
		for (size_t i = 0; i < node.children.size(); i++) {
			GetNodeProps(model.nodes[node.children[i]], model, vertexCount, indexCount);
		}
	}
	if (node.mesh > -1) {
		const tinygltf::Mesh mesh = model.meshes[node.mesh];
		for (size_t i = 0; i < mesh.primitives.size(); i++) {
			auto primitive = mesh.primitives[i];
			vertexCount += model.accessors[primitive.attributes.find("POSITION")->second].count;
			if (primitive.indices > -1) {
				indexCount += model.accessors[primitive.indices].count;
			}
		}
	}
}

void ENGModel::LoadNode(Node* parent, const tinygltf::Node& node, uint32_t nodeIndex, const tinygltf::Model& model, LoaderInfo& loaderInfo, float globalscale)
{
	Node* newNode = new Node{};
	newNode->index = nodeIndex;
	newNode->parent = parent;
	newNode->name = node.name;
	newNode->skinIndex = node.skin;
	newNode->matrix = glm::mat4(1.0f);

	// Generate local node matrix
	glm::vec3 translation = glm::vec3(1.0f);
	if (node.translation.size() == 3) {
		//Create a translation matrix from the node data
		translation = glm::make_vec3(node.translation.data());
		newNode->translation = translation;
	}
	glm::mat4 rotation = glm::mat4(1.0f);
	if (node.rotation.size() == 4) {
		//Create a rotation matrix from the node data
		glm::quat q = glm::make_quat(node.rotation.data());
		newNode->rotation = glm::mat4(q);
	}
	glm::vec3 scale = glm::vec3(1.0f);
	if (node.scale.size() == 3) {
		//Create a scale matrix with the node data
		scale = glm::make_vec3(node.scale.data());
		newNode->scale = scale;
	}
	//Grab the matrix from the model
	if (node.matrix.size() == 16) {
		newNode->matrix = glm::make_mat4x4(node.matrix.data());
	};

	// Node with children
	if (node.children.size() > 0) {
		for (size_t i = 0; i < node.children.size(); i++) {
			LoadNode(newNode, model.nodes[node.children[i]], node.children[i], model, loaderInfo, globalscale);
		}
	}

	// Node contains mesh data
	if (node.mesh > -1) {
		//Load the mesh lol
		const tinygltf::Mesh mesh = model.meshes[node.mesh];
		Mesh* newMesh = new Mesh(newNode->matrix);
		for (size_t j = 0; j < mesh.primitives.size(); ++j)
		{
			const tinygltf::Primitive& primitive = mesh.primitives[j];
			uint32_t vertexStart = loaderInfo.vertexPos;
			uint32_t indexStart = loaderInfo.indexPos;
			uint32_t indexCount = 0;
			uint32_t vertexCount = 0;
			glm::vec3 posMin{};
			glm::vec3 posMax{};
			bool hasSkin = false;
			bool hasIndices = primitive.indices > -1;
			
			{
				const float* bufferPos = nullptr;
				const float* bufferNormals = nullptr;
				const float* bufferTexCoordSet0 = nullptr;
				const float* bufferTexCoordSet1 = nullptr;
				const void* bufferJoints = nullptr;
				const float* bufferWeights = nullptr;

				int posByteStride;
				int normByteStride;
				int uv0ByteStride;
				int uv1ByteStride;
				int jointByteStride;
				int weightByteStride;

				int jointComponentType;

				// Position attribute is required
				assert(primitive.attributes.find("POSITION") != primitive.attributes.end());

				const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
				const tinygltf::BufferView& posView = model.bufferViews[posAccessor.bufferView];
				bufferPos = reinterpret_cast<const float*>(&(model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));
				posMin = glm::vec3(posAccessor.minValues[0], posAccessor.minValues[1], posAccessor.minValues[2]);
				posMax = glm::vec3(posAccessor.maxValues[0], posAccessor.maxValues[1], posAccessor.maxValues[2]);
				vertexCount = static_cast<uint32_t>(posAccessor.count);
				posByteStride = posAccessor.ByteStride(posView) ? (posAccessor.ByteStride(posView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);

				if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
					const tinygltf::Accessor& normAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
					const tinygltf::BufferView& normView = model.bufferViews[normAccessor.bufferView];
					bufferNormals = reinterpret_cast<const float*>(&(model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
					normByteStride = normAccessor.ByteStride(normView) ? (normAccessor.ByteStride(normView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);
				}

				if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
					const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
					const tinygltf::BufferView& uvView = model.bufferViews[uvAccessor.bufferView];
					bufferTexCoordSet0 = reinterpret_cast<const float*>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
					uv0ByteStride = uvAccessor.ByteStride(uvView) ? (uvAccessor.ByteStride(uvView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC2);
				}
				if (primitive.attributes.find("TEXCOORD_1") != primitive.attributes.end()) {
					const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_1")->second];
					const tinygltf::BufferView& uvView = model.bufferViews[uvAccessor.bufferView];
					bufferTexCoordSet1 = reinterpret_cast<const float*>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
					uv1ByteStride = uvAccessor.ByteStride(uvView) ? (uvAccessor.ByteStride(uvView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC2);
				}

				// Skinning
				// Joints
				if (primitive.attributes.find("JOINTS_0") != primitive.attributes.end()) {
					const tinygltf::Accessor& jointAccessor = model.accessors[primitive.attributes.find("JOINTS_0")->second];
					const tinygltf::BufferView& jointView = model.bufferViews[jointAccessor.bufferView];
					bufferJoints = &(model.buffers[jointView.buffer].data[jointAccessor.byteOffset + jointView.byteOffset]);
					jointComponentType = jointAccessor.componentType;
					jointByteStride = jointAccessor.ByteStride(jointView) ? (jointAccessor.ByteStride(jointView) / tinygltf::GetComponentSizeInBytes(jointComponentType)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC4);
				}

				if (primitive.attributes.find("WEIGHTS_0") != primitive.attributes.end()) {
					const tinygltf::Accessor& weightAccessor = model.accessors[primitive.attributes.find("WEIGHTS_0")->second];
					const tinygltf::BufferView& weightView = model.bufferViews[weightAccessor.bufferView];
					bufferWeights = reinterpret_cast<const float*>(&(model.buffers[weightView.buffer].data[weightAccessor.byteOffset + weightView.byteOffset]));
					weightByteStride = weightAccessor.ByteStride(weightView) ? (weightAccessor.ByteStride(weightView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC4);
				}

				hasSkin = (bufferJoints && bufferWeights);

				for (size_t v = 0; v < posAccessor.count; v++) {
					Vertex& vert = loaderInfo.vertexData[loaderInfo.vertexPos];
					vert.pos = glm::vec4(glm::make_vec3(&bufferPos[v * posByteStride]), 1.0f);
					vert.normal = glm::normalize(glm::vec3(bufferNormals ? glm::make_vec3(&bufferNormals[v * normByteStride]) : glm::vec3(0.0f)));
					vert.uv0 = bufferTexCoordSet0 ? glm::make_vec2(&bufferTexCoordSet0[v * uv0ByteStride]) : glm::vec3(0.0f);
					vert.uv1 = bufferTexCoordSet1 ? glm::make_vec2(&bufferTexCoordSet1[v * uv1ByteStride]) : glm::vec3(0.0f);

					if (hasSkin)
					{
						switch (jointComponentType) {
						case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
							const uint16_t* buf = static_cast<const uint16_t*>(bufferJoints);
							vert.joint0 = glm::vec4(glm::make_vec4(&buf[v * jointByteStride]));
							break;
						}
						case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
							const uint8_t* buf = static_cast<const uint8_t*>(bufferJoints);
							vert.joint0 = glm::vec4(glm::make_vec4(&buf[v * jointByteStride]));
							break;
						}
						default:
							//error
							// Not supported by spec
							//std::cerr << "Joint component type " << jointComponentType << " not supported!" << std::endl;
							break;
						}
					}
					else {
						vert.joint0 = glm::vec4(0.0f);
					}
					vert.weight0 = hasSkin ? glm::make_vec4(&bufferWeights[v * weightByteStride]) : glm::vec4(0.0f);
					// Fix for all zero weights
					if (glm::length(vert.weight0) == 0.0f) {
						vert.weight0 = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
					}
					loaderInfo.vertexPos++;
				}
			}
			
			//Go through all the submeshes and load the file
			// Indices
			if (hasIndices)
			{
				const tinygltf::Accessor& accessor = model.accessors[primitive.indices > -1 ? primitive.indices : 0];
				const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
				const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

				indexCount = static_cast<uint32_t>(accessor.count);
				const void* dataPtr = &(buffer.data[accessor.byteOffset + bufferView.byteOffset]);

				switch (accessor.componentType) {
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
					const uint32_t* buf = static_cast<const uint32_t*>(dataPtr);
					for (size_t index = 0; index < accessor.count; index++) {
						loaderInfo.indexData[loaderInfo.indexPos] = buf[index] + vertexStart;
						loaderInfo.indexPos++;
					}
					break;
				}
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
					const uint16_t* buf = static_cast<const uint16_t*>(dataPtr);
					for (size_t index = 0; index < accessor.count; index++) {
						loaderInfo.indexData[loaderInfo.indexPos] = buf[index] + vertexStart;
						loaderInfo.indexPos++;
					}
					break;
				}
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
					const uint8_t* buf = static_cast<const uint8_t*>(dataPtr);
					for (size_t index = 0; index < accessor.count; index++) {
						loaderInfo.indexData[loaderInfo.indexPos] = buf[index] + vertexStart;
						loaderInfo.indexPos++;
					}
					break;
				}
				default:
					//Error
					//std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
					return;
				}
			}
			//SubMesh* newPrimitive = new SubMesh(indexStart, indexCount, vertexCount, primitive.material > -1 ? materials[primitive.material] : materials.back());
			SubMesh* newPrimitive = new SubMesh(indexStart, indexCount, vertexCount, primitive.material > -1 ? materials[primitive.material] : materials.back());
			//newPrimitive->setBoundingBox(posMin, posMax);
			newMesh->submeshes.push_back(newPrimitive);
		}
		newNode->mesh = newMesh;
	}
	if (parent)
	{
		parent->children.push_back(newNode);
	}
	else
	{
		nodes.push_back(newNode);
	}
	linearNodes.push_back(newNode);
}

void ENGModel::BindDescriptors()
{

}

void ENGModel::LoadSkins(tinygltf::Model& model)
{
	for (tinygltf::Skin& source : model.skins) {
		Skin* newSkin = new Skin{};
		newSkin->name = source.name;

		// Find skeleton root node
		if (source.skeleton > -1) {
			newSkin->skeletonRoot = NodeFromIndex(source.skeleton);
		}

		// Find joint nodes
		for (int jointIndex : source.joints) {
			Node* node = NodeFromIndex(jointIndex);
			if (node) {
				newSkin->joints.push_back(NodeFromIndex(jointIndex));
			}
		}

		// Get inverse bind matrices from buffer
		if (source.inverseBindMatrices > -1) {
			const tinygltf::Accessor& accessor = model.accessors[source.inverseBindMatrices];
			const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
			const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
			newSkin->inverseBindMatrices.resize(accessor.count);
			memcpy(newSkin->inverseBindMatrices.data(), &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(glm::mat4));
		}

		skins.push_back(newSkin);
	}
}

void ENGModel::LoadSamplers(tinygltf::Model& model)
{
	//for (tinygltf::Sampler smpl : model.samplers) {
	//	vkglTF::TextureSampler sampler{};
	//	sampler.minFilter = getVkFilterMode(smpl.minFilter);
	//	sampler.magFilter = getVkFilterMode(smpl.magFilter);
	//	sampler.addressModeU = getVkWrapMode(smpl.wrapS);
	//	sampler.addressModeV = getVkWrapMode(smpl.wrapT);
	//	sampler.addressModeW = sampler.addressModeV;
	//	textureSamplers.push_back(sampler);
	//}
}

void ENGModel::LoadTextures(tinygltf::Model& model)
{
	

	for (tinygltf::Texture& tex : model.textures) {
		tinygltf::Image image = model.images[tex.source];
		//TextureSampler textureSampler;
		//if (tex.sampler == -1) {
		//	//// No sampler specified, use a default one
		//	//textureSampler.magFilter = VK_FILTER_LINEAR;
		//	//textureSampler.minFilter = VK_FILTER_LINEAR;
		//	//textureSampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		//	//textureSampler.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		//	//textureSampler.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		//}
		//else {
		//	textureSampler = textureSamplers[tex.sampler];
		//}
		Texture texture;
		texture.FromglTfImage(image);
		textures.push_back(texture);
	}
}

void ENGModel::LoadMaterials(tinygltf::Model& model)
{
	for (tinygltf::Material& mat : model.materials) {
		Material material{};
		if (mat.values.find("baseColorTexture") != mat.values.end()) {
			material.baseColorTextureIndex = mat.values["baseColorTexture"].TextureIndex();
			material.baseColorTexture = &textures[mat.values["baseColorTexture"].TextureIndex()];
			material.texCoordSets.baseColor = mat.values["baseColorTexture"].TextureTexCoord();
			
		}
		if (mat.values.find("metallicRoughnessTexture") != mat.values.end()) {
			material.metallicRoughnessTextureIndex = mat.values["metallicRoughnessTexture"].TextureIndex();
			material.metallicRoughnessTexture = &textures[mat.values["metallicRoughnessTexture"].TextureIndex()];
			material.texCoordSets.metallicRoughness = mat.values["metallicRoughnessTexture"].TextureTexCoord();
		}
		if (mat.values.find("roughnessFactor") != mat.values.end()) {
			material.roughnessFactor = static_cast<float>(mat.values["roughnessFactor"].Factor());
		}
		if (mat.values.find("metallicFactor") != mat.values.end()) {
			material.metallicFactor = static_cast<float>(mat.values["metallicFactor"].Factor());
		}
		if (mat.values.find("baseColorFactor") != mat.values.end()) {
			material.baseColorFactor = glm::make_vec4(mat.values["baseColorFactor"].ColorFactor().data());
		}
		if (mat.additionalValues.find("normalTexture") != mat.additionalValues.end()) {
			material.normalTextureIndex = mat.additionalValues["normalTexture"].TextureIndex();
			material.normalTexture = &textures[mat.additionalValues["normalTexture"].TextureIndex()];
			material.texCoordSets.normal = mat.additionalValues["normalTexture"].TextureTexCoord();
		}
		if (mat.additionalValues.find("emissiveTexture") != mat.additionalValues.end()) {
			material.emissiveTextureIndex = mat.additionalValues["emissiveTexture"].TextureIndex();
			material.emissiveTexture = &textures[mat.additionalValues["emissiveTexture"].TextureIndex()];
			material.texCoordSets.emissive = mat.additionalValues["emissiveTexture"].TextureTexCoord();
		}
		if (mat.additionalValues.find("occlusionTexture") != mat.additionalValues.end()) {
			material.occlusionTextureIndex = mat.additionalValues["occlusionTexture"].TextureIndex();
			material.occlusionTexture = &textures[mat.additionalValues["occlusionTexture"].TextureIndex()];
			material.texCoordSets.occlusion = mat.additionalValues["occlusionTexture"].TextureTexCoord();
		}
		if (mat.additionalValues.find("alphaMode") != mat.additionalValues.end()) {
			tinygltf::Parameter param = mat.additionalValues["alphaMode"];
			if (param.string_value == "BLEND") {
				material.alphaMode = Material::ALPHAMODE_BLEND;
			}
			if (param.string_value == "MASK") {
				material.alphaCutoff = 0.5f;
				material.alphaMode = Material::ALPHAMODE_MASK;
			}
		}
		if (mat.additionalValues.find("alphaCutoff") != mat.additionalValues.end()) {
			material.alphaCutoff = static_cast<float>(mat.additionalValues["alphaCutoff"].Factor());
		}
		if (mat.additionalValues.find("emissiveFactor") != mat.additionalValues.end()) {
			material.emissiveFactor = glm::vec4(glm::make_vec3(mat.additionalValues["emissiveFactor"].ColorFactor().data()), 1.0);
		}
	
		// Extensions
		// @TODO: Find out if there is a nicer way of reading these properties with recent tinygltf headers
		if (mat.extensions.find("KHR_materials_pbrSpecularGlossiness") != mat.extensions.end()) {
			auto ext = mat.extensions.find("KHR_materials_pbrSpecularGlossiness");
			if (ext->second.Has("specularGlossinessTexture")) {
				auto index = ext->second.Get("specularGlossinessTexture").Get("index");
				material.extension.specularGlossinessTexture = &textures[index.Get<int>()];
				auto texCoordSet = ext->second.Get("specularGlossinessTexture").Get("texCoord");
				material.texCoordSets.specularGlossiness = texCoordSet.Get<int>();
				material.pbrWorkflows.specularGlossiness = true;
			}
			if (ext->second.Has("diffuseTexture")) {
				auto index = ext->second.Get("diffuseTexture").Get("index");
				material.extension.diffuseTexture = &textures[index.Get<int>()];
			}
			if (ext->second.Has("diffuseFactor")) {
				auto factor = ext->second.Get("diffuseFactor");
				for (uint32_t i = 0; i < factor.ArrayLen(); i++) {
					auto val = factor.Get(i);
					material.extension.diffuseFactor[i] = val.IsNumber() ? (float)val.Get<double>() : (float)val.Get<int>();
				}
			}
			if (ext->second.Has("specularFactor")) {
				auto factor = ext->second.Get("specularFactor");
				for (uint32_t i = 0; i < factor.ArrayLen(); i++) {
					auto val = factor.Get(i);
					material.extension.specularFactor[i] = val.IsNumber() ? (float)val.Get<double>() : (float)val.Get<int>();
				}
			}
		}
	

		//material.


		//material.materialIndex = materials.size();
		materials.push_back(material);
	}
	// Push a default material at the end of the list for meshes with no material assigned
	
	materials.push_back(Material());


	

	//materials[materials.size()-1].materialIndex = materials.size();
}

void ENGModel::Draw(ID3D12GraphicsCommandList* commandList)
{
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->IASetIndexBuffer(&indexBufferView);

	for (auto& node : nodes)
	{
		DrawNode(node, commandList);
	}
	
}

void ENGModel::DrawNode(Node* node, ID3D12GraphicsCommandList* commandList)
{
	if (node->mesh)
	{
		
		
		commandList->SetGraphicsRootConstantBufferView(3, node->mesh->uniformBuffer.nodeMatrixBuffer->Handle()->GetGPUVirtualAddress());
		
		for (int j = 0; j < node->mesh->submeshes.size(); ++j)
		{
			commandList->SetGraphicsRootConstantBufferView(4, node->mesh->submeshes[j]->materialParamBuffer->Handle()->GetGPUVirtualAddress());
			//int heapIndex = node->mesh->submeshes[j]->material.baseColorTexture->textureIndex;
			//commandList->SetGraphicsRootDescriptorTable(3, m_textureHeap.handleGPU(heapIndex));
			commandList->DrawIndexedInstanced(node->mesh->submeshes[j]->indexCount, 1, node->mesh->submeshes[j]->firstIndex, 0, 0);
		}
	}

	for (auto& child : node->children)
	{
		DrawNode(child, commandList);
	}
}


void Texture::FromglTfImage(tinygltf::Image& gltfimage)
{
	//this->device = device;

	unsigned char* buffer = nullptr;
	size_t bufferSize = 0;
	bool deleteBuffer = false;
	if (gltfimage.component == 3) {
		// Most devices don't support RGB only on Vulkan so convert if necessary
		// TODO: Check actual format support and transform only if required
		bufferSize = gltfimage.width * gltfimage.height * 4;
		buffer = new unsigned char[bufferSize];
		unsigned char* rgba = buffer;
		unsigned char* rgb = &gltfimage.image[0];
		for (int32_t i = 0; i < gltfimage.width * gltfimage.height; ++i) {
			for (int32_t j = 0; j < 3; ++j) {
				rgba[j] = rgb[j];
			}
			rgba += 4;
			rgb += 3;
		}
		deleteBuffer = true;
	}
	else {
		buffer = &gltfimage.image[0];
		bufferSize = gltfimage.image.size();
	}

	
	textureData = DX::Texture();
	textureData.imageData = reinterpret_cast<BYTE*>(buffer);

	D3D12_RESOURCE_DESC resourceDescription = {};
	resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDescription.Alignment = 0;
	resourceDescription.Width = gltfimage.width;
	resourceDescription.Height = gltfimage.height;
	resourceDescription.DepthOrArraySize = 1;
	resourceDescription.MipLevels = 1;
	resourceDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	resourceDescription.SampleDesc.Count = 1;
	resourceDescription.SampleDesc.Quality = 0;
	resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE;

	textureData.LoadTexture(1,reinterpret_cast<BYTE*>(buffer), resourceDescription);
	textureData.imageBytesPerRow = (32 * gltfimage.width)/8;
	textureBuffer = ENGBuffer::Create(textureData);
	//VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
}

Node* ENGModel::FindNode(Node* parent, uint32_t index) {
	Node* nodeFound = nullptr;
	if (parent->index == index) {
		return parent;
	}
	for (auto& child : parent->children) {
		nodeFound = FindNode(child, index);
		if (nodeFound) {
			break;
		}
	}
	return nodeFound;
}

Node* ENGModel::NodeFromIndex(uint32_t index) {
	Node* nodeFound = nullptr;
	for (auto& node : nodes) {
		nodeFound = FindNode(node, index);
		if (nodeFound) {
			break;
		}
	}
	return nodeFound;
}