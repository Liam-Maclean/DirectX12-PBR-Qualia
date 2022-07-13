#pragma once
#include "ENGBase.h"
#include "ENGBuffer.h"
#include "DirectXWrappers.h"
#include <d3d12.h>

struct TextureCreateInfo {
	int width;
	int height;
	int depth;
	DXGI_FORMAT m_format;
	int m_sampleCount;
	D3D12_RESOURCE_DIMENSION m_resourceDimension;
	D3D12_RESOURCE_FLAGS m_resourceFlags;
	D3D12_TEXTURE_LAYOUT m_textureLayout;
	D3D12_CLEAR_VALUE m_rtvClearValue;
	uint32_t m_mipLevels;
	std::string name;
};

class ENGTexture : public IHandle<ENGTexture, BYTE*>
{
public:
	ENGTexture();
	ENGTexture(TextureCreateInfo createInfo);
	ENGTexture(const ENGTexture& copy);
	~ENGTexture();
	
	void CreateFromGLTFImage();
	void CreateFromFile();

	//Getters
	inline ENGBuffer* GetResourceBuffer() { return m_textureResourceBuffer; };
	inline D3D12_SHADER_RESOURCE_VIEW_DESC GetSRVDesc() { return m_textureSRVDesc; };
	inline D3D12_RENDER_TARGET_VIEW_DESC GetRTVDesc() { return m_textureRTVDesc; };
	void CreateRTV(DXDescriptorHeap* inRTVHeap, int& inHeapIndex);
	void CreateSRV(DXDescriptorHeap* inRTVHeap, int& inHeapIndex);
private:
	ENGBuffer* m_textureResourceBuffer;
	D3D12_RESOURCE_DESC m_textureDesc;
	D3D12_SHADER_RESOURCE_VIEW_DESC m_textureSRVDesc;
	D3D12_RENDER_TARGET_VIEW_DESC m_textureRTVDesc;
	TextureCreateInfo m_textureCreateInfo;
	int m_imageBytesPerRow;
};

