#pragma once
#include "DeferredRenderer.h"
#include "ENGRendererContext.h"
#include "cvars.h"
#include "ENGModel.h"


extern AutoCVar_Int CVAR_ResolutionWidth;
extern AutoCVar_Int CVAR_ResolutionHeight;
extern AutoCVar_Int CVAR_FXAAEnabled;
extern AutoCVar_Int CVAR_HDREnabled;
extern AutoCVar_Int CVAR_GammaCorrectionEnabled;
extern AutoCVar_Int CVAR_DebugViewMode;

///<summary>Constructor (default)</summary>
///<param name="width">Width of the output screen</param>
///<param name="height">Height of the output screen</param>
DeferredRenderer::DeferredRenderer(int width, int height)
{
	m_camera = 0;

	m_textureBuffer = 0;
	m_srvHeapIndex = 0;
	m_deferredRTVHeapIndex = 0;
	m_deferredSRVHeapIndex = 0;

}
///<summary>Intializer for the renderer</summary>
///<param name="FrameHeap">GPU Frame heap manager for the scene</param>
void DeferredRenderer::Initialize(DXFrameHeapManager* frameHeap)
{
	m_frame = frameHeap;
	m_srvHeapIndex = 0;
	m_deferredRTVHeapIndex = 0;
	m_deferredSRVHeapIndex = 0;
	m_screenQuad.Init();
	m_shaderDebugValues = new ShaderDebugValues();
	CreateCPUHeaps();
	CreateRenderTargets();
	CreatePSO();
	CreateConstantBuffers();
}

///<summary>Create the CPU heaps for the scene to go into the frame heap</summary>
void DeferredRenderer::CreateCPUHeaps()
{
	m_deferredSRVHeap.Create(ENGContext.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 5);
	m_postProcessHeap.Create(ENGContext.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 3, true);
	m_srvHeap.Create(ENGContext.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 100);
	m_cbvHeap.Create(ENGContext.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
	m_deferredRTVHeap.Create(ENGContext.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 3);
	m_rtvHeap.Create(ENGContext.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 4); //9 (position, albedo and normal x backbuffer count)
	m_dsvHeap.Create(ENGContext.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1); //1 (depth)
}

///<summary>Create the CBVs for the scene</summary>
void DeferredRenderer::CreateConstantBuffers()
{
	HRESULT hr;
	CD3DX12_RANGE readRange(0, 0);

	m_mvpConstantBuffer = ENGBuffer::Create(1024 * 64);
	hr = m_mvpConstantBuffer->Handle()->Map(0, &readRange, reinterpret_cast<void**>(&m_mvpConstantBufferAddress));
	if (FAILED(hr))
	{

	}

	//Camera CBV
	D3D12_CONSTANT_BUFFER_VIEW_DESC	descBuffer;
	descBuffer.BufferLocation = m_mvpConstantBuffer->Handle()->GetGPUVirtualAddress();
	//Constant buffer must be larger than 256 bytes
	descBuffer.SizeInBytes = (sizeof(MVPMatrix) + 255) & ~255;
	ENGContext.GetDevice()->CreateConstantBufferView(&descBuffer, m_cbvHeap.handleCPU(0));

	auto frameIndex = m_frame->CopyAllocate(1, m_cbvHeap);
	m_frameHeapParameters.main = frameIndex;

	m_debugShaderValuesBuffer = ENGBuffer::Create(sizeof(ShaderDebugValues));
	m_debugShaderValuesBuffer->Map();
	

}

///<summary>Create the RTVs for the scene</summary>
void DeferredRenderer::CreateRenderTargets()
{
	//Create render target resources
	D3D12_CLEAR_VALUE clearValue;
	clearValue.Color[0] = m_clearColor[0];
	clearValue.Color[1] = m_clearColor[1];
	clearValue.Color[2] = m_clearColor[2];
	clearValue.Color[3] = m_clearColor[3];
	clearValue.Format = m_rtvFormat[0];

	TextureCreateInfo textureCreateInfo = {};
	textureCreateInfo.width = (UINT)CVAR_ResolutionWidth.Get();
	textureCreateInfo.height = (UINT)CVAR_ResolutionHeight.Get();
	textureCreateInfo.m_format = m_rtvFormat[0];
	textureCreateInfo.m_mipLevels = 1;
	textureCreateInfo.m_resourceDimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureCreateInfo.m_resourceFlags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	textureCreateInfo.m_textureLayout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	textureCreateInfo.m_sampleCount = 1;
	textureCreateInfo.m_rtvClearValue = clearValue;
	textureCreateInfo.depth = 1;
	textureCreateInfo.name = "Albedo Roughness Texture";

	m_albedoRoughnessRTVTexture = ENGTexture::Create(textureCreateInfo);
	m_albedoRoughnessRTVTexture->CreateRTV(&m_deferredRTVHeap, m_deferredRTVHeapIndex);
	m_albedoRoughnessRTVTexture->CreateSRV(&m_deferredSRVHeap, m_deferredSRVHeapIndex);

	textureCreateInfo.m_format = m_rtvFormat[1];
	textureCreateInfo.m_rtvClearValue.Format = m_rtvFormat[1];
	textureCreateInfo.name = "Normal Texture";
	m_normalRTVTexture = ENGTexture::Create(textureCreateInfo);
	m_normalRTVTexture->CreateRTV(&m_deferredRTVHeap, m_deferredRTVHeapIndex);
	m_normalRTVTexture->CreateSRV(&m_deferredSRVHeap, m_deferredSRVHeapIndex);

	textureCreateInfo.m_format = m_rtvFormat[2];
	textureCreateInfo.m_rtvClearValue.Format = m_rtvFormat[2];
	textureCreateInfo.name = "Emissive Texture";
	m_emissiveRTVTexture = ENGTexture::Create(textureCreateInfo);
	m_emissiveRTVTexture->CreateRTV(&m_deferredRTVHeap, m_deferredRTVHeapIndex);
	m_emissiveRTVTexture->CreateSRV(&m_deferredSRVHeap, m_deferredSRVHeapIndex);


	for (int i = 0; i < 3; ++i)
	{
		HRESULT result;

		//Get a pointer to the first back buffer from the swapchain
		result = ENGContext.GetSwapchain()->GetBuffer(i, IID_PPV_ARGS(&m_backBufferRenderTarget[i]));
		if (FAILED(result))
		{
			//return false;
		}

		//Create a render target view for the first back buffer
		ENGContext.GetDevice()->CreateRenderTargetView(m_backBufferRenderTarget[i], NULL, m_rtvHeap.handleCPU(m_rtvHeapIndex));
		m_backBufferRenderTarget[i]->SetName(L"Swapchain Back Buffer");
		m_rtvHeapIndex++;
	}


	textureCreateInfo.m_format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureCreateInfo.m_rtvClearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureCreateInfo.name = "Post Process Texture";
	m_postProcessRTVTexture = ENGTexture::Create(textureCreateInfo);
	m_postProcessRTVTexture->CreateRTV(&m_rtvHeap, m_rtvHeapIndex);
	m_postProcessRTVTexture->CreateSRV(&m_postProcessHeap, m_postProcessHeapIndex);

	//Create depth stencil
	//====Depth stencil buffer and desc====
	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

	depthOptimizedClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0xff;

	m_depthStencilBuffer = ENGBuffer::Create(depthOptimizedClearValue);
	ENGContext.GetDevice()->CreateDepthStencilView(m_depthStencilBuffer->Handle(), &depthStencilDesc, m_dsvHeap.handleCPU(0));

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	ENGContext.GetDevice()->CreateShaderResourceView(m_depthStencilBuffer->Handle(), &srvDesc, m_deferredSRVHeap.handleCPU(m_deferredSRVHeapIndex));
	m_deferredSRVHeapIndex++;


	//Copy the descriptor heap into the one big descriptor heap for rendering
	auto currentBufferIndex = m_frame->CopyAllocate(m_deferredSRVHeapIndex, m_deferredSRVHeap);
	m_frameHeapParameters.GBuffer = currentBufferIndex;
}

///<summary>Create the PSO pipeline states for the scene</summary>
void DeferredRenderer::CreatePSO()
{
	m_deferredShaderLayout = ENGShaderLayout::Create();
	m_deferredShaderLayout->GetDefferredShaderLayout();

	m_fxaaShaderLayout = ENGShaderLayout::Create();
	m_fxaaShaderLayout->GetFXAAShaderLayout();


	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	inputLayoutDesc.NumElements = _countof(ENGInputLayouts::DefaultLayout);
	inputLayoutDesc.pInputElementDescs = ENGInputLayouts::DefaultLayout;

	D3D12_INPUT_LAYOUT_DESC quadInputLayoutDesc = {};
	quadInputLayoutDesc.NumElements = _countof(ENGInputLayouts::QuadLayout);
	quadInputLayoutDesc.pInputElementDescs = ENGInputLayouts::QuadLayout;

	PipelineCreateInfo forwardPipelineCreateInfo = {};
	forwardPipelineCreateInfo.vertShader = ENGShaderLoader::LoadShader(L"DeferredForwardVert.cso");
	forwardPipelineCreateInfo.pixelShader = ENGShaderLoader::LoadShader(L"DeferredForwardPixel.cso");
	forwardPipelineCreateInfo.rtvFormats.push_back(m_rtvFormat[0]); //Position
	forwardPipelineCreateInfo.rtvFormats.push_back(m_rtvFormat[1]); //normal
	forwardPipelineCreateInfo.rtvFormats.push_back(m_rtvFormat[2]); //albedo
	forwardPipelineCreateInfo.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	forwardPipelineCreateInfo.shaderLayout = m_deferredShaderLayout;
	forwardPipelineCreateInfo.vertexInputLayout = inputLayoutDesc;
	forwardPipelineCreateInfo.wireframeMode = false;
	forwardPipelineCreateInfo.depthTestEnabled = true;
	m_forwardPipeline = ENGPipeline::Create(forwardPipelineCreateInfo);


	PipelineCreateInfo compositionPipelineCreateInfo = {};
	compositionPipelineCreateInfo.vertShader = ENGShaderLoader::LoadShader(L"QuadVert.cso");
	compositionPipelineCreateInfo.pixelShader = ENGShaderLoader::LoadShader(L"DeferredCompositionPixel.cso");
	compositionPipelineCreateInfo.rtvFormats.push_back(DXGI_FORMAT_R8G8B8A8_UNORM); //Position
	compositionPipelineCreateInfo.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	compositionPipelineCreateInfo.shaderLayout = m_deferredShaderLayout;
	compositionPipelineCreateInfo.vertexInputLayout = quadInputLayoutDesc;
	compositionPipelineCreateInfo.wireframeMode = false;
	compositionPipelineCreateInfo.depthTestEnabled = false;
	m_compositionPipeline = ENGPipeline::Create(compositionPipelineCreateInfo);


	PipelineCreateInfo postProcessPipelineCreateInfo = {};
	postProcessPipelineCreateInfo.vertShader = ENGShaderLoader::LoadShader(L"QuadVert.cso");
	postProcessPipelineCreateInfo.pixelShader = ENGShaderLoader::LoadShader(L"FXAAPixel.cso");
	postProcessPipelineCreateInfo.rtvFormats.push_back(DXGI_FORMAT_R8G8B8A8_UNORM);
	postProcessPipelineCreateInfo.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	postProcessPipelineCreateInfo.shaderLayout = m_fxaaShaderLayout;
	postProcessPipelineCreateInfo.vertexInputLayout = quadInputLayoutDesc;
	postProcessPipelineCreateInfo.wireframeMode = false;
	postProcessPipelineCreateInfo.depthTestEnabled = false;
	m_fxaaPipeline = ENGPipeline::Create(postProcessPipelineCreateInfo);
}

///<summary>Create the CBV for the main directional light</summary>
void DeferredRenderer::CreateLightCBV(ENGGraphicsStructs::DirectionalLight* directionalLight)
{
	//Create the buffer and copy the directional light data into the CBV
	m_directionalLightConstantBuffer = ENGBuffer::Create(sizeof(ENGGraphicsStructs::DirectionalLight));
	m_directionalLightConstantBuffer->Map();
	m_directionalLightConstantBuffer->CopyData(static_cast<void*>(directionalLight), sizeof(ENGGraphicsStructs::DirectionalLight));
}

///<summary>Grabs all the model data for the scene and grabs all the texture data from it</summary>
void DeferredRenderer::SetUpModels(ENGModel* model)
{
	m_textures = model->textures;
	for (int i = 0; i < m_textures.size(); ++i)
	{
		ENGContext.GetDevice()->CreateShaderResourceView(m_textures[i].textureBuffer->Handle(), &m_textures[i].textureData.shaderResourceView, m_srvHeap.handleCPU(m_srvHeapIndex));
		m_srvHeapIndex++;
	}
	
	//Copy the descriptor heap into the one big descriptor heap for rendering
	auto currentBufferIndex = m_frame->CopyAllocate(m_srvHeapIndex, m_srvHeap);
	m_frameHeapParameters.Textures = currentBufferIndex;
}

///<summary>Prepare any changing data for the scene such as the camera, lights and MVP matrix</summary>
void DeferredRenderer::PrepareFrame(glm::mat4 mvp, Camera* camera, ENGGraphicsStructs::DirectionalLight* directionalLight)
{
	//Set up the camera
	m_camera = camera;

	glm::mat4 viewMat = m_camera->GetCameraView();
	glm::mat4 projMat = m_camera->GetCameraProjection();
	glm::mat4 mvpMat = mvp * viewMat * projMat;
	m_mvpMat.model = mvp;
	m_mvpMat.view = m_camera->GetCameraView();
	m_mvpMat.projection = m_camera->GetCameraProjection();
	
	m_mvpMat.cameraPos = glm::vec4(camera->GetPosition(), 1.0f);
	

	m_mvpMat.model = glm::transpose(m_mvpMat.model);
	m_mvpMat.view = glm::transpose(m_mvpMat.view);
	m_mvpMat.projection = glm::transpose(m_mvpMat.projection);
	m_mvpMat.invertedProjMat = glm::inverse(m_mvpMat.projection);
	m_mvpMat.invertedTransposeModelMat = glm::inverse(m_mvpMat.model);
	m_mvpMat.invertedViewMat = glm::inverse(m_mvpMat.view);
	// copy our ConstantBuffer instance to the mapped constant buffer resource
	memcpy(m_mvpConstantBufferAddress, &m_mvpMat, sizeof(m_mvpMat));
	
	m_directionalLightConstantBuffer->CopyData(static_cast<void*>(directionalLight), sizeof(ENGGraphicsStructs::DirectionalLight));

	//Set the values from the CVAR values
	m_shaderDebugValues->GammaCorrectionEnabled = CVAR_GammaCorrectionEnabled.Get();
	m_shaderDebugValues->HDREnabled = CVAR_HDREnabled.Get();
	m_shaderDebugValues->viewMode = CVAR_DebugViewMode.Get();
	m_debugShaderValuesBuffer->CopyData(static_cast<void*>(m_shaderDebugValues), sizeof(ShaderDebugValues));
	//copy the data into the debug buffer

}

///<summary>Draw any mesh data for the scene</summary>
///<param name="mesh">Mesh being renderered</param>
///<param name="commandlist">Command list for the frame</param>
void DeferredRenderer::Draw(ENGModel* mesh, ID3D12GraphicsCommandList* commandList)
{
	commandList->IASetVertexBuffers(0, 1, &mesh->vertexBufferView);
	commandList->IASetIndexBuffer(&mesh->indexBufferView);
	commandList->DrawIndexedInstanced(mesh->m_indexCount, 1, 0, 0, 0);
}

///<summary>Draw the screen quad for composition passes and post processing</summary>
///<param name="commandlist">Command list for the frame</param>
void DeferredRenderer::DrawScreenQuad(ID3D12GraphicsCommandList* commandList)
{	
	m_screenQuad.Draw(commandList);
}

///<summary>Set the render targets and PSO objects for the forward pass</summary>
///<param name="commandlist">Command list for the frame</param>
///<param name="bufferIndex">Current backbuffer being rendered to</param>
void DeferredRenderer::StartFrameForward(ID3D12GraphicsCommandList* commandList, int bufferIndex)
{


	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_backBufferRenderTarget[bufferIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	//Starts the frame, initializing the descriptor table index to 0
	m_frame->StartFrame();

	commandList->OMSetRenderTargets(3, &m_deferredRTVHeap.hCPUHeapStart, true, &m_dsvHeap.hCPUHeapStart);
	ID3D12DescriptorHeap* ppHeaps[1] = { m_frame->GetDescriptorHeap()};
	for (int i = 0; i < 3; i++)
		commandList->ClearRenderTargetView(m_deferredRTVHeap.handleCPU(i), m_clearColor, 0, nullptr);

	commandList->ClearDepthStencilView(m_dsvHeap.handleCPU(0), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0xff, 0, nullptr);
	commandList->SetDescriptorHeaps(1, ppHeaps);
	commandList->SetGraphicsRootDescriptorTable(0, m_frame->GetGPUHandle(m_frameHeapParameters.main));
	commandList->SetGraphicsRootDescriptorTable(1, m_frame->GetGPUHandle(m_frameHeapParameters.GBuffer));
	commandList->SetGraphicsRootDescriptorTable(2, m_frame->GetGPUHandle(m_frameHeapParameters.Textures));
}

///<summary>End the frame forward pass and transition render target states</summary>
///<param name="commandlist">Command list for the frame</param>
void DeferredRenderer::EndFrameForward(ID3D12GraphicsCommandList* commandList)
{
	//ends the frame, 
	m_frame->EndFrame();


	//commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_albedoCommitedResourceBuffer->Handle(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_albedoRoughnessRTVTexture->GetResourceBuffer()->Handle(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));
	//commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_positionCommitedResourceBuffer->Handle(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_normalRTVTexture->GetResourceBuffer()->Handle(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));
	//commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_normalCommitedResourceBuffer->Handle(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_emissiveRTVTexture->GetResourceBuffer()->Handle(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));
	
	
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_depthStencilBuffer->Handle(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ));
}


///<summary>Set the pipeline state for the forward pass</summary>
///<param name="commandlist">Command list for the frame</param>
///<param name="bufferIndex">Current backbuffer being rendered to</param>
void DeferredRenderer::SetPSOForward(ID3D12GraphicsCommandList* commandList, int bufferIndex)
{
	commandList->SetPipelineState(m_forwardPipeline->Handle());
	commandList->SetGraphicsRootSignature(m_deferredShaderLayout->Handle());
}

///<summary>Set the up the composition passes render targets</summary>
///<param name="commandlist">Command list for the frame</param>
///<param name="bufferIndex">Current backbuffer being rendered to</param>
void DeferredRenderer::StartFrameComposition(ID3D12GraphicsCommandList* commandList, int bufferIndex)
{
	//Starts the frame, initializing the descriptor table index to 0
	m_frame->StartFrame();

	const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	
	commandList->OMSetRenderTargets(1, &m_rtvHeap.handleCPU(3), FALSE, NULL);
	commandList->ClearRenderTargetView(m_rtvHeap.handleCPU(3), clearColor, 0, nullptr);

	//Set the descriptor heap for the rendering to begin
	ID3D12DescriptorHeap* frameHeap[] = { m_frame->GetDescriptorHeap() };
	commandList->SetDescriptorHeaps(_countof(frameHeap), frameHeap);
	commandList->SetGraphicsRootDescriptorTable(0, m_frame->GetGPUHandle(m_frameHeapParameters.main));
	commandList->SetGraphicsRootDescriptorTable(1, m_frame->GetGPUHandle(m_frameHeapParameters.GBuffer));
	commandList->SetGraphicsRootConstantBufferView(5, m_directionalLightConstantBuffer->Handle()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(6, m_debugShaderValuesBuffer->Handle()->GetGPUVirtualAddress());
}

///<summary>Set the pipeline state for the forward pass</summary>
///<param name="commandlist">Command list for the frame</param>
///<param name="bufferIndex">Current backbuffer being rendered to</param>
void DeferredRenderer::EndFrameComposition(ID3D12GraphicsCommandList* commandList, int bufferIndex)
{
	//ends the frame, 
	m_frame->EndFrame();

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_postProcessRTVTexture->GetResourceBuffer()->Handle(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));

	//Convert the G-Buffer resources back to render targets for the next frame
	//commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_albedoCommitedResourceBuffer->Handle(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_albedoRoughnessRTVTexture->GetResourceBuffer()->Handle(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));
	//commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_positionCommitedResourceBuffer->Handle(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_normalRTVTexture->GetResourceBuffer()->Handle(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));
 	//commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_normalCommitedResourceBuffer->Handle(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));
 	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_emissiveRTVTexture->GetResourceBuffer()->Handle(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_depthStencilBuffer->Handle(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));

}

///<summary>Set the PSO state for composition</summary>
///<param name="commandlist">Command list for the frame</param>
void DeferredRenderer::SetPSOComposition(ID3D12GraphicsCommandList* commandList)
{
	commandList->SetPipelineState(m_compositionPipeline->Handle());
	commandList->SetGraphicsRootSignature(m_deferredShaderLayout->Handle());
}

///<summary>Set the PSO state for post processing</summary>
///<param name="commandlist">Command list for the frame</param>
///<param name="bufferIndex">Current backbuffer being rendered to</param>
void DeferredRenderer::SetPSOPostProcess(ID3D12GraphicsCommandList* commandList, int bufferIndex)
{
	commandList->SetPipelineState(m_fxaaPipeline->Handle());
	commandList->SetGraphicsRootSignature(m_fxaaShaderLayout->Handle());
}

///<summary>Set up the frame process</summary>
///<param name="commandlist">Command list for the frame</param>
///<param name="bufferIndex">Current backbuffer being rendered to</param>
void DeferredRenderer::StartFramePostProcess(ID3D12GraphicsCommandList* commandList, int bufferIndex)
{
	const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	//We dont want to clear the render target here, only alter what it currently contains
	commandList->OMSetRenderTargets(1, &m_rtvHeap.handleCPU(bufferIndex), FALSE, NULL);

	//Set the descriptor heap for the rendering to begin
	ID3D12DescriptorHeap* frameHeap[] = { m_postProcessHeap };
	commandList->SetDescriptorHeaps(_countof(frameHeap), frameHeap);
	commandList->SetGraphicsRootDescriptorTable(0, m_postProcessHeap.handleGPU(0));
}

///<summary>End the frame process and transition to post processing</summary>
///<param name="commandlist">Command list for the frame</param>
///<param name="bufferIndex">Current backbuffer being rendered to</param>
void DeferredRenderer::EndFramePostProcess(ID3D12GraphicsCommandList* commandList, int bufferIndex)
{
	//Ready the swapchains backbuffer resources to be presented
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_backBufferRenderTarget[bufferIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));


	//Transition the post process SRV back to RTV for the next frame
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_postProcessRTVTexture->GetResourceBuffer()->Handle(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));

}

