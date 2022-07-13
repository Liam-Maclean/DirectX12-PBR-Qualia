#include "ENGModelLoader.h"
#include "ENGBufferUtility.h"
#include <fstream>
#include <sstream>


bool ENGModelLoader::LoadObj(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, std::string filename, ENGGraphicsStructs::Mesh& outMesh, bool isRhCoordSys, bool computeNormals)
{
	return false;
	//std::vector< DWORD > vertexIndices, uvIndices, normalIndices;
	//std::vector< XMFLOAT3 > temp_vertices;
	//std::vector< XMFLOAT2 > temp_uvs;
	//std::vector< XMFLOAT3 > temp_normals;
	////const char* tempFileName = filename.c_str();
	//FILE* file = fopen(filename.c_str(), "r");
	//if (file == NULL) {
	//	printf("Impossible to open the file !\n");
	//	return false;
	//}
	//
	//while (1) {
	//
	//	char lineHeader[128];
	//	// read the first word of the line
	//	int res = fscanf(file, "%s", lineHeader);
	//	if (res == EOF)
	//		break; // EOF = End Of File. Quit the loop.
	//
	//	// else : parse lineHeader
	//
	//	if (strcmp(lineHeader, "v") == 0) {
	//		XMFLOAT3 vertex;
	//		fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
	//		temp_vertices.push_back(vertex);
	//	}
	//	else if (strcmp(lineHeader, "vt") == 0) {
	//		glm::vec2 uv;
	//		fscanf(file, "%f %f\n", &uv.x, &uv.y);
	//		//uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
	//		temp_uvs.push_back(uv);
	//	}
	//	else if (strcmp(lineHeader, "vn") == 0) {
	//		XMFLOAT3 normal;
	//		fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
	//		temp_normals.push_back(normal);
	//	}
	//	else if (strcmp(lineHeader, "f") == 0) {
	//		std::string vertex1, vertex2, vertex3;
	//		unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
	//		int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
	//		if (matches != 9) {
	//			printf("File can't be read by our simple parser :-( Try exporting with other options\n");
	//			fclose(file);
	//			return false;
	//		}
	//		vertexIndices.push_back(vertexIndex[0] -1);
	//		vertexIndices.push_back(vertexIndex[1] -1);
	//		vertexIndices.push_back(vertexIndex[2] -1);
	//		uvIndices.push_back(uvIndex[0]);
	//		uvIndices.push_back(uvIndex[1]);
	//		uvIndices.push_back(uvIndex[2]);
	//		normalIndices.push_back(normalIndex[0]);
	//		normalIndices.push_back(normalIndex[1]);
	//		normalIndices.push_back(normalIndex[2]);
	//	}
	//	else {
	//		// Probably a comment, eat up the rest of the line
	//		char stupidBuffer[1000];
	//		fgets(stupidBuffer, 1000, file);
	//	}
	//
	//}
	//
	//
	//std::vector<ENGGraphicsStructs::NewVertex> vertices;
	//ENGGraphicsStructs::NewVertex tempVert;
	//
	//// For each vertex of each triangle
	//for (unsigned int i = 0; i < temp_vertices.size(); i++) {
	//
	//	// Get the attributes thanks to the index
	//	tempVert.pos = temp_vertices[i];
	//	tempVert.texCoord = temp_uvs[i];
	//	tempVert.normal = temp_normals[i];
	//
	//
	//	vertices.push_back(tempVert);
	//}
	//
	//
	////for (unsigned int i = 0; i < temp_vertices.size(); ++i)
	////{
	////	// Get the indices of its attributes
	////	unsigned int vertexIndex = vertexIndices[i];
	////	unsigned int uvIndex = uvIndices[i];
	////	unsigned int normalIndex = normalIndices[i];
	////
	////	// Get the attributes thanks to the index
	////	tempVert.pos = temp_vertices[vertexIndex - 1];
	////	tempVert.texCoord = temp_uvs[uvIndex - 1];
	////	tempVert.normal = temp_normals[normalIndex - 1];
	////}
	//
	//outMesh.indices = vertexIndices;
	//outMesh.vertices = vertices;
	//
	//D3D12_SUBRESOURCE_DATA vertexData = {};
	//vertexData.pData = reinterpret_cast<BYTE*>(outMesh.vertices.data());
	//vertexData.RowPitch = outMesh.GetVertexBufferSize();
	//vertexData.SlicePitch = outMesh.GetVertexBufferSize();
	//
	//ENGBufferUtility::CreateBufferDefaultToUploadHeap(commandList, outMesh.vertexBuffer, outMesh.GetVertexBufferSize(), vertexData);
	//
	//commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(outMesh.vertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
	//
	//D3D12_SUBRESOURCE_DATA indexData = {};
	//indexData.pData = reinterpret_cast<BYTE*>(outMesh.indices.data());
	//indexData.RowPitch = outMesh.GetIndexBufferSize();
	//indexData.SlicePitch = outMesh.GetIndexBufferSize();
	//
	//ENGBufferUtility::CreateBufferDefaultToUploadHeap(commandList, outMesh.indexBuffer, outMesh.GetIndexBufferSize(), indexData);
	//
	//commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(outMesh.indexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER));
	//outMesh.SetBufferView();
	//
	//
	//
	//fclose(file);
	//return true;
		// else : parse lineHeader

	//return false;
}

