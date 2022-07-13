#include "MeshSubdivideRenderer.h"
#include "ENGRendererContext.h"
#include "ENGSwapchain.h"
#include "cvars.h"
extern AutoCVar_Int CVAR_ResolutionWidth;
extern AutoCVar_Int CVAR_ResolutionHeight;
MeshSubdivideRenderer::MeshSubdivideRenderer(int width, int height)
{
	m_camera = 0;
	m_textureBuffer = 0;
	srvHeapIndex = 0;
}

void MeshSubdivideRenderer::Initialize()
{
	m_frame = std::unique_ptr<DXFrameHeapManager>(new DXFrameHeapManager(ENGContext.GetDevice()));
	srvHeapIndex = 0;
	CreateCPUHeaps();
	CreateRenderTargets();
	CreatePSO();
	CreateBuffers();
}

void MeshSubdivideRenderer::CreateCPUHeaps()
{
	//For this pipeline, we need 1 texture, 3 render views (back buffers for swapchain) and 1 depth view
	m_srvHeap.Create(ENGContext.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
	m_rtvHeap.Create(ENGContext.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 3);
	m_dsvHeap.Create(ENGContext.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);
}

void MeshSubdivideRenderer::CreateBuffers()
{
	braynzarTexture.LoadTexture(L"braynzar.jpg",1);
	m_textureBuffer = new ENGBuffer(braynzarTexture);
	ENGContext.GetDevice()->CreateShaderResourceView(m_textureBuffer->Handle(), &braynzarTexture.shaderResourceView, m_srvHeap.handleCPU(srvHeapIndex));




	srvHeapIndex++;
	//Copy the descriptor heap into the one big descriptor heap for rendering
	auto currentBufferIndex = m_frame->CopyAllocate(srvHeapIndex, m_srvHeap);
	m_frameHeapParameters.Textures = currentBufferIndex;
}

void MeshSubdivideRenderer::CreateRenderTargets()
{
	HRESULT hr;
	for (int i = 0; i < 3; i++)
	{
		//Get a pointer to the first back buffer from the swapchain
		HRESULT result = ENGContext.GetSwapchain()->GetBuffer(i, IID_PPV_ARGS(&m_backBufferRenderTarget[i]));
		if (FAILED(result))
		{
			//return false;
		}

		//Create a render target view for the first back buffer
		ENGContext.GetDevice()->CreateRenderTargetView(m_backBufferRenderTarget[i], NULL, m_rtvHeap.handleCPU(i));
	}

	//====Depth stencil buffer and desc====
	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	m_depthStencilBuffer = ENGBuffer::Create(depthOptimizedClearValue);
	ENGContext.GetDevice()->CreateDepthStencilView(m_depthStencilBuffer->Handle(), &depthStencilDesc, m_dsvHeap.handleCPU(0));
}


void MeshSubdivideRenderer::CreatePSO()
{
	m_shaderLayout = ENGShaderLayout::Create();
	m_shaderLayout->GetTessellationShaderLayout();

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	inputLayoutDesc.NumElements = _countof(ENGInputLayouts::DefaultLayout);
	inputLayoutDesc.pInputElementDescs = ENGInputLayouts::DefaultLayout;

	PipelineCreateInfo pipelineCreateInfo = {};
	pipelineCreateInfo.vertShader = ENGShaderLoader::LoadShader(L"VertexShader.cso");
	pipelineCreateInfo.pixelShader = ENGShaderLoader::LoadShader(L"PixelShader.cso");
	pipelineCreateInfo.hullShader = ENGShaderLoader::LoadShader(L"HullShader.cso");
	pipelineCreateInfo.domainShader = ENGShaderLoader::LoadShader(L"DomainShader.cso");
	pipelineCreateInfo.rtvFormats.push_back(DXGI_FORMAT_R8G8B8A8_UNORM);
	pipelineCreateInfo.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
	pipelineCreateInfo.shaderLayout = m_shaderLayout;
	pipelineCreateInfo.vertexInputLayout = inputLayoutDesc;
	pipelineCreateInfo.wireframeMode = true;
	m_pipeline = ENGPipeline::Create(pipelineCreateInfo);
}

void MeshSubdivideRenderer::Draw(ENGGraphicsStructs::Mesh* mesh, ID3D12GraphicsCommandList* commandList)
{
	commandList->IASetVertexBuffers(0, 1, &mesh->vertexBufferView);
	commandList->IASetIndexBuffer(&mesh->indexBufferView);
	commandList->DrawIndexedInstanced(mesh->GetNumIndices(), 1, 0, 0, 0);
}

void MeshSubdivideRenderer::StartFrame(ID3D12GraphicsCommandList* commandList, int bufferIndex)
{
	//Starts the frame, initializing the descriptor table index to 0
	m_frame->StartFrame();

	const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_backBufferRenderTarget[bufferIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	commandList->OMSetRenderTargets(1, &m_rtvHeap.handleCPU(bufferIndex), FALSE, &m_dsvHeap.handleCPU(0));
	commandList->ClearRenderTargetView(m_rtvHeap.handleCPU(bufferIndex), clearColor, 0, nullptr);

	//Set the descriptor heap for the rendering to begin
	ID3D12DescriptorHeap* frameHeap[] = { m_frame->GetDescriptorHeap() };
	commandList->SetDescriptorHeaps(_countof(frameHeap), frameHeap);
	commandList->SetGraphicsRootDescriptorTable(1, m_frame->GetGPUHandle(0));
}

void MeshSubdivideRenderer::PrepareFrame(std::vector<ENGGraphicsStructs::Model> models, Camera* camera)
{
	//Set up the camera
	m_camera = camera;

	//Update the constant buffer stuff here also
}

void MeshSubdivideRenderer::EndFrame(ID3D12GraphicsCommandList* commandList, int bufferIndex)
{
	//ends the frame, 
	m_frame->EndFrame();
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_backBufferRenderTarget[bufferIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
}

void MeshSubdivideRenderer::SetPSO(ID3D12GraphicsCommandList* commandList)
{
	//OM (Output merger)
	commandList->ClearDepthStencilView(m_dsvHeap.handleCPU(0), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	commandList->SetPipelineState(m_pipeline->Handle());
	commandList->SetGraphicsRootSignature(m_shaderLayout->Handle());
}
