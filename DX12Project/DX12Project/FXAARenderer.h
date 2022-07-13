#pragma once
#include <vector>
#include <memory>
#include <d3dcompiler.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include "d3dx12.h"
#include "ENGBase.h"
#include "ENGScreenQuad.h"
#include "ENGGraphicsStructs.h"
#include "ENGPipeline.h"
#include "ENGBuffer.h"
#include "ENGDXResourceManager.h"
#include "ENGInputLayouts.h"
#include "ENGShaderLoader.h"
#include "ENGBufferUtility.h"
#include "DXFrameHeapManager.h"
#include "ENGModel.h"
#include "Camera.h"
class FXAARenderer
{
public:
	FXAARenderer();
	~FXAARenderer();
private:
	std::unique_ptr<DXFrameHeapManager> m_frame;
};

