#pragma once
#include <d3dcompiler.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <wincodec.h>

class ENGTextureLoader
{
public:
	//Static methods
	static int LoadImageDataFromFile(BYTE** imageData, D3D12_RESOURCE_DESC& resourceDescription, LPCWSTR filename, int& bytesPerRow);
	static DXGI_FORMAT GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wicFormatGUID);
	static WICPixelFormatGUID GetConvertToWICFormat(WICPixelFormatGUID& wicFormatGUID);
	static int GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat);
private:
	//Prevent creation of an instance of this object
	ENGTextureLoader() {};
};

