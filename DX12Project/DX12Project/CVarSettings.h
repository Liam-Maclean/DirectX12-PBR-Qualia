#pragma once
#include "cvars.h"
AutoCVar_Int CVAR_ResolutionWidth("ResolutionWidth", "Width of the screen", 1920, CVarFlags::EditReadOnly);
AutoCVar_Int CVAR_ResolutionHeight("ResolutionHeight", "Height of the screen", 1080, CVarFlags::EditReadOnly);
AutoCVar_Int CVAR_ClientWidth("ClientWidth", "Width of the client window screen", 1920, CVarFlags::Advanced);
AutoCVar_Int CVAR_ClientHeight("ClientHeight", "Height of the client window screen", 1080, CVarFlags::Advanced);
AutoCVar_Int CVAR_FullScreen("Fullscreen", "Fullscreen (0 = windowed, 1 = Fullscreen, 2 = borderless)", 1, CVarFlags::EditReadOnly);
AutoCVar_String CVAR_WindowName("Windowname", "Name of the main window screen", "DirectX Window", CVarFlags::EditReadOnly);
AutoCVar_Int CVAR_ImGuiEnabled("ImGuiEnabled", "ImGui enabled (0 = disabled, 1 = enabled)", 1, CVarFlags::EditReadOnly);
AutoCVar_Int CVAR_FXAAEnabled("FXAAEnabled", "Enable FXAA anti aliasing (0 = disabled, 1 = enabled)", 1, CVarFlags::EditReadOnly);
AutoCVar_Int CVAR_HDREnabled("HDREnabled", "HDR Enabled (0 = disabled, 1 = enabled)", 1, CVarFlags::EditReadOnly);
AutoCVar_Int CVAR_GammaCorrectionEnabled("GammaCorrectionEnabled", "Gamma correction enabled (0 = disabled, 1 = enabled)", 1, CVarFlags::EditReadOnly);
AutoCVar_Int CVAR_DebugViewMode("DebugViewMode", "Debug view mode for render targets (0 to 5)", 0, CVarFlags::EditReadOnly);
//AutoCVar_Int CVAR_RenderDocValidationLayer("RenderDocValidationLayer", "Validation layer for renderdoc enabled or disabled (1 enabled, 0 disabled)", 1, CVarFlags::EditReadOnly);
//AutoCVar_Int CVAR_ValidationLayerDebugLevel("ValidationLayerDebugLevel", "Level of debug layer enabled (0 = disabled, 1 = normal, 2 = verbose)", 2, CVarFlags::EditReadOnly);

//AutoCVar_Int CVAR_DebugDraw("DebugDraw", "Of fidelity layer enabled (0 = disabled, 1 = show geometry lines, 2 = show points)", 0, CVarFlags::EditReadOnly);
//AutoCVar_Int CVAR_CameraControl("CameraControl", "Take control of the camera (0 = disabled, 1 = POV camera control", 0, CVarFlags::EditReadOnly);
