#include "ImGuiRenderer.h"
#include "ENGRendererContext.h"
#include "imgui.h"
#include "cvars.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx12.h"


extern AutoCVar_Int CVAR_ResolutionWidth;
extern AutoCVar_Int CVAR_ResolutionHeight;
extern AutoCVar_Int CVAR_FullScreen;
extern AutoCVar_Int CVAR_TesselationFactor;
extern AutoCVar_Int CVAR_InnerTesselationFactor;
extern AutoCVar_Int CVAR_HDREnabled;
extern AutoCVar_Int CVAR_GammaCorrectionEnabled;
extern AutoCVar_Int CVAR_DebugViewMode;


ImGuiRenderer::ImGuiRenderer(int width, int height)
{
	
}

void ImGuiRenderer::Initialize(HWND winHandle)
{
    D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
    descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    descriptorHeapDesc.NumDescriptors = 1;
    descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;


    ENGContext.GetDevice()->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_cbvSrvHeap));

    //ImGuiContext = ImGui::Getconfig
	ImGuiIO& io = ImGui::GetIO(); (void)io;
    SetImGuiStyle(&io);
    io.DisplaySize.x = CVAR_ResolutionWidth.Get();
    io.DisplaySize.y = CVAR_ResolutionHeight.Get();

	ImGui_ImplWin32_Init(winHandle);
	ImGui_ImplDX12_Init(ENGContext.GetDevice(), 3, DXGI_FORMAT_R8G8B8A8_UNORM, m_cbvSrvHeap, m_cbvSrvHeap->GetCPUDescriptorHandleForHeapStart(), m_cbvSrvHeap->GetGPUDescriptorHandleForHeapStart());
}

void ImGuiRenderer::GrabMainLight(ENGGraphicsStructs::DirectionalLight* inMainLight)
{
    mainLight = inMainLight;
}


void ImGuiRenderer::StartImGuiFrame()
{
    bool HDREnabled = CVAR_HDREnabled.Get();
    bool GammaCorrectionEnabled = CVAR_GammaCorrectionEnabled.Get();
    int ViewMode = CVAR_DebugViewMode.Get();

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

    ImVec2 mainMenuBarHeight;
    if (ImGui::BeginMainMenuBar())
    {
        mainMenuBarHeight = ImGui::GetWindowSize();
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
            if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
            if (ImGui::MenuItem("Close", "Esc")) {/* Break out of the problem*/ }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Windows"))
        {
            if (ImGui::MenuItem("GPU", "")) { /* Do stuff */ }
            if (ImGui::MenuItem("CPU", "")) { /* Do stuff */ }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("About"))
        {
            if (ImGui::MenuItem("Info", "")) { /* Do stuff */ }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
    ImGui::SetNextWindowPos(ImVec2(0.0f, mainMenuBarHeight.y));
    ImGui::SetNextWindowSize(ImVec2(CVAR_ResolutionWidth.Get() / 10, CVAR_ResolutionHeight.Get()));
    ImGui::Begin("Info", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);                          // Create a window called "Hello, world!" and append into it.
    ImGui::Text("Resolution X = %d", CVAR_ResolutionWidth.Get());
    ImGui::Text("Resolution Y = %d", CVAR_ResolutionHeight.Get());
    ImGui::Text("% .3f ms / frame(% .1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("");
    ImGui::Text("G-Buffer draw call count: ");
    ImGui::Text("Composition draw call count: ");
    ImGui::Text("Total Draw Count: ");
    ImGui::Text("");
    ImGui::Text("Vertex Count: ");
    ImGui::Text("Poly count: ");
    ImGui::Text("Texture Count: ");
    ImGui::Text("");
    ImGui::Text("Main Light Direction");
    ImGui::SliderFloat("X:", &mainLight->direction.x, -1.0f, 1.0f);
    ImGui::SliderFloat("Y:", &mainLight->direction.y, -1.0f, 1.0f);
    ImGui::SliderFloat("Z:", &mainLight->direction.z, -1.0f, 1.0f);
    ImGui::Text("");
    ImGui::Text("Main Light Color");
    ImGui::SliderFloat("R:", &mainLight->color.x, 0.0f, 1.0f);
    ImGui::SliderFloat("G:", &mainLight->color.y, 0.0f, 1.0f);
    ImGui::SliderFloat("B:", &mainLight->color.z, 0.0f, 1.0f);
    ImGui::Text("");
    ImGui::Checkbox("HDR Enabled", &HDREnabled);
    ImGui::Checkbox("Gamma Correction Enabled", &GammaCorrectionEnabled);
    ImGui::SliderInt("View Mode", &ViewMode, 0, 4);
    ImGui::End();
    ImGui::Render();
    CVAR_HDREnabled.Set(HDREnabled);
    CVAR_GammaCorrectionEnabled.Set(GammaCorrectionEnabled);
    CVAR_DebugViewMode.Set(ViewMode);
}

void ImGuiRenderer::RenderImGuiData()
{
    ID3D12GraphicsCommandList* commandList = ENGContext.GetCommandList();
    //Set the descriptor heap for the rendering to begin
    ID3D12DescriptorHeap* frameHeap[] = { m_cbvSrvHeap };
    commandList->SetDescriptorHeaps(_countof(frameHeap), frameHeap);
    commandList->SetDescriptorHeaps(1, frameHeap);
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
}

void ImGuiRenderer::SetImGuiStyle(ImGuiIO* io)
{

    //io->Fonts->AddFontFromFileTTF("C:/Users/liamm/Downloads/DirectX12-Engine-DeferredRendering/DirectX12-Engine-DeferredRendering/DX12Project/Libs/imgui-master/misc/fonts/Karla-Regular.ttf", 15.0f);
    //io->Fonts->AddFontFromFileTTF("C:/Users/liamm/Downloads/DirectX12-Engine-DeferredRendering/DirectX12-Engine-DeferredRendering/DX12Project/Libs/imgui-master/misc/fonts/Roboto-Medium.ttf", 12.0f);
    //io->Fonts->AddFontFromFileTTF("C:/Users/liamm/Downloads/DirectX12-Engine-DeferredRendering/DirectX12-Engine-DeferredRendering/DX12Project/Libs/imgui-master/misc/fonts/ProggyClean.ttf", 12.0f);
    io->Fonts->AddFontFromFileTTF("C:/Users/liamm/Downloads/DirectX12-Engine-DeferredRendering/DirectX12-Engine-DeferredRendering/DX12Project/Libs/imgui-master/misc/fonts/DroidSans.ttf", 15.0f);
    ImGui::GetStyle().FrameRounding = 4.0f;
    ImGui::GetStyle().GrabRounding = 4.0f;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}
