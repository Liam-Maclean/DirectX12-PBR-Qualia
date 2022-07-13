#include "ENGTexture.h"
#include "ENGRendererContext.h"


ENGTexture::ENGTexture(TextureCreateInfo createInfo)
{
	//Store a copy of the texture create info
	m_textureCreateInfo = createInfo;


	//Populate the texture descriptor for the texture
	m_textureDesc = {};
	ZeroMemory(&m_textureDesc, sizeof(m_textureDesc));
	m_textureDesc.Dimension = m_textureCreateInfo.m_resourceDimension;
	m_textureDesc.Alignment = 0;
	m_textureDesc.SampleDesc.Count = m_textureCreateInfo.m_sampleCount;
	m_textureDesc.SampleDesc.Quality = 0;
	m_textureDesc.MipLevels = m_textureCreateInfo.m_mipLevels;

	m_textureDesc.DepthOrArraySize = m_textureCreateInfo.depth;
	m_textureDesc.Width = (UINT)m_textureCreateInfo.width;
	m_textureDesc.Height = (UINT)m_textureCreateInfo.height;
	m_textureDesc.Layout = m_textureCreateInfo.m_textureLayout;
	m_textureDesc.Flags = m_textureCreateInfo.m_resourceFlags;
	m_textureDesc.Format = m_textureCreateInfo.m_format;

	//Populate the RTV descriptor
	m_textureRTVDesc = {};
	ZeroMemory(&m_textureRTVDesc, sizeof(m_textureRTVDesc));
	m_textureRTVDesc.Texture2D.MipSlice = 0;
	m_textureRTVDesc.Texture2D.PlaneSlice = 0;
	m_textureRTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	m_textureRTVDesc.Format = m_textureCreateInfo.m_format;

	//Populate the SRV descriptor
	m_textureSRVDesc = {};
	ZeroMemory(&m_textureSRVDesc, sizeof(m_textureSRVDesc));
	m_textureSRVDesc.Texture2D.MipLevels = m_textureCreateInfo.m_mipLevels;
	m_textureSRVDesc.Texture2D.MostDetailedMip = 0;
	m_textureSRVDesc.Format = m_textureCreateInfo.m_format;
	m_textureSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	m_textureSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;


	//Create the committed resource for the texture
	if (m_textureCreateInfo.m_resourceFlags == D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
	{
		m_textureResourceBuffer = ENGBuffer::Create(m_textureCreateInfo.m_rtvClearValue, m_textureDesc, D3D12_RESOURCE_STATE_RENDER_TARGET);
	}
	else
	{
		m_textureResourceBuffer = ENGBuffer::Create(m_textureDesc, D3D12_RESOURCE_STATE_COMMON);
	}

	std::wstring stemp = std::wstring(m_textureCreateInfo.name.begin(), m_textureCreateInfo.name.end());
	LPCWSTR ltemp = stemp.c_str();

	m_textureResourceBuffer->Handle()->SetName(ltemp);
}

ENGTexture::ENGTexture(const ENGTexture& copy)
{

}

ENGTexture::~ENGTexture()
{
}

void ENGTexture::CreateFromGLTFImage()
{
}

void ENGTexture::CreateFromFile()
{
}


void ENGTexture::CreateRTV(DXDescriptorHeap* inRTVHeap, int& inHeapIndex)
{
	ENGContext.GetDevice()->CreateRenderTargetView(m_textureResourceBuffer->Handle(), &m_textureRTVDesc, inRTVHeap->handleCPU(inHeapIndex));
	inHeapIndex++; //Increment the heap index
}

void ENGTexture::CreateSRV(DXDescriptorHeap* inRTVHeap, int& inHeapIndex)
{
	ENGContext.GetDevice()->CreateShaderResourceView(m_textureResourceBuffer->Handle(), &m_textureSRVDesc, inRTVHeap->handleCPU(inHeapIndex));
	inHeapIndex++; //Increment the heap index
}
