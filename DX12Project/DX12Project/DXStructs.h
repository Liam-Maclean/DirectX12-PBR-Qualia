#pragma once 
#include <Windows.h>
#include <d3d12.h>
#include "ENGTextureLoader.h"
namespace DX
{
	struct Texture
	{
		HRESULT hr;
		int imageBytesPerRow;
		BYTE* imageData;
		D3D12_RESOURCE_DESC textureDesc;
		D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceView;
		//ENGBuffer* textureBuffer;


		void LoadTexture(LPCWSTR inFilename, int mipLevels)
		{
			int imageSize = ENGTextureLoader::LoadImageDataFromFile(&imageData, textureDesc, inFilename, imageBytesPerRow);
			if (imageSize <= 0)
			{

			}

			shaderResourceView.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			shaderResourceView.Format = textureDesc.Format;
			shaderResourceView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			shaderResourceView.Texture2D.MipLevels = mipLevels;
			
			//textureBuffer = ENGBuffer::Create(braynzarTexture, "I am a texture buffer");
		}

		void LoadTexture(LPCWSTR inFilename, int mipLevels, bool skybox)
		{
			int imageSize = ENGTextureLoader::LoadImageDataFromFile(&imageData, textureDesc, inFilename, imageBytesPerRow);
			if (imageSize <= 0)
			{

			}

			shaderResourceView.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			shaderResourceView.Format = textureDesc.Format;
			shaderResourceView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			shaderResourceView.Texture2D.MipLevels = mipLevels;

		}

		void LoadTexture(int inMipLevels, BYTE* inImageData, D3D12_RESOURCE_DESC inTextureDesc)
		{
			imageData = inImageData;
			textureDesc = inTextureDesc;
			shaderResourceView.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			shaderResourceView.Format = textureDesc.Format;
			shaderResourceView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			shaderResourceView.Texture2D.MipLevels = inMipLevels;
		}
	};
}

