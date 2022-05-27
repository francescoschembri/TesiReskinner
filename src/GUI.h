#pragma once

#include "StatusManager.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <FileBrowser/ImFileDialog.h>

#include <algorithm>
#include <vector>
#include <format>

void SetupImGui(GLFWwindow* window);
void CloseImGui();
void SetupFileDialog();
void NewGUIFrame();
void RenderGUI(StatusManager& status);
void OpenFullMeshDialog(StatusManager& status);
void OpenMeshDialog(StatusManager& status);
void OpenAnimationDialog(StatusManager& status);
void RenderMenuBar(StatusManager& status);
void RenderFileMenuSection(StatusManager& status);
void RenderModelInfo(StatusManager& status);
void RenderAnimationsInfo(StatusManager& status);
void RenderRenderingInfo(StatusManager& status);
void RenderEditInfo(StatusManager& status);
bool ShowAnimationNInfo(Animator& animator, int n);