#include "GUI.h"

void RenderGUI(StatusManager& status)
{
	NewGUIFrame();
	RenderMenuBar(status);
	OpenMeshDialog(status);
	OpenFullMeshDialog(status);
	OpenAnimationDialog(status);
	RenderModelInfo(status);
	RenderAnimationsInfo(status);
	RenderEditInfo(status);
	RenderRenderingInfo(status);
	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	// Update and Render additional Platform Windows
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

void SetupImGui(GLFWwindow* window)
{
	// initialize imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigDockingWithShift = false;

	// When viewports are enabled we tweak WindowRounding 
	// WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 0.0f;
	style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	SetupFileDialog();
}

void CloseImGui() {
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void SetupFileDialog()
{
	// create a file browser instance
	ifd::FileDialog::Instance().CreateTexture = [](uint8_t* data, int w, int h, char fmt) -> void* {
		GLuint tex;

		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, (fmt == 0) ? GL_BGRA : GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		return (void*)tex;
	};

	ifd::FileDialog::Instance().DeleteTexture = [](void* tex) {
		GLuint texID = (GLuint)tex;
		glDeleteTextures(1, &texID);
	};
}

void NewGUIFrame() {
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void OpenFullMeshDialog(StatusManager& status)
{
	if (ifd::FileDialog::Instance().IsDone("Full Mesh Open Dialog")) {
		if (ifd::FileDialog::Instance().HasResult()) {
			std::string res = ifd::FileDialog::Instance().GetResult().u8string();
			status.CompleteLoad(res);
		}
		ifd::FileDialog::Instance().Close();
	}
}

void OpenMeshDialog(StatusManager& status)
{
	if (ifd::FileDialog::Instance().IsDone("Mesh Open Dialog")) {
		if (ifd::FileDialog::Instance().HasResult()) {
			std::string res = ifd::FileDialog::Instance().GetResult().u8string();
			status.LoadModel(res);
		}
		ifd::FileDialog::Instance().Close();
	}
}

void OpenAnimationDialog(StatusManager& status)
{
	if (ifd::FileDialog::Instance().IsDone("Animation Open Dialog")) {
		if (ifd::FileDialog::Instance().HasResult()) {
			std::string res = ifd::FileDialog::Instance().GetResult().u8string();
			status.AddAnimation(res.c_str());
		}
		ifd::FileDialog::Instance().Close();
	}
}

void RenderMenuBar(StatusManager& status)
{
	if (ImGui::BeginMainMenuBar())
	{
		RenderFileMenuSection(status);
		ImGui::EndMainMenuBar();
	}
}

void RenderFileMenuSection(StatusManager& status)
{
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Open new Model...", "")) {
			ifd::FileDialog::Instance().Open("Mesh Open Dialog", "Open a mesh", "Mesh file (*.dae){.dae},.*");
		}
		if (ImGui::MenuItem("Open new Model (all animations)...", "")) {
			ifd::FileDialog::Instance().Open("Full Mesh Open Dialog", "Open a mesh (all animations)", "Mesh file (*.dae){.dae},.*");
		}
		if (ImGui::MenuItem("Add animation...", "") && status.animatedModel) {
			ifd::FileDialog::Instance().Open("Animation Open Dialog", "Open an animation", "Animation file (*.dae){.dae},.*");
		}
		ImGui::EndMenu();
	}
}

void RenderModelInfo(StatusManager& status)
{
	if (!status.animatedModel.has_value()) return;
	ImGui::Begin("Model");
	ImGui::SetWindowPos(ImVec2(0, 464));
	ImGui::SetWindowSize(ImVec2(290, 276));
	static ImGuiTableFlags flags = ImGuiTableFlags_RowBg;
	if (ImGui::BeginTable("Meshes", 2, flags))
	{
		for (int i = 0; i < status.animatedModel->meshes.size(); i++)
		{
			Mesh& m = status.animatedModel->meshes[i];
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Checkbox(("Mesh " + std::to_string(i)).c_str(), &m.enabled);
			if (status.bakedModel.has_value()) {
				status.bakedModel->meshes[i].enabled = m.enabled;
			}
		}
		ImGui::EndTable();
	}
	ImGui::End();
}

void RenderRenderingInfo(StatusManager& status)
{
	ImGui::Begin("Rendering");
	ImGui::SetWindowPos(ImVec2(0, 19));
	ImGui::SetWindowSize(ImVec2(290, 315));
	ImGui::Text("Color:");
	if (ImGui::RadioButton("None##Rendering", status.visualMode == Mode_Grey)) { status.visualMode = Mode_Grey; }
	if (ImGui::RadioButton("Textures##Rendering", status.visualMode == Mode_Texture)) { status.visualMode = Mode_Texture; }
	if (ImGui::RadioButton("# bone links##Rendering", status.visualMode == Mode_NumBones)) { status.visualMode = Mode_NumBones; }
	if (ImGui::RadioButton("Influence of bone N##Rendering", status.visualMode == Mode_CurrentBoneIDInfluence)) { status.visualMode = Mode_CurrentBoneIDInfluence; }
	ImGui::SameLine();
	ImGui::PushItemWidth(75.0f);
	ImGui::InputInt(" ", &status.currentBoneID);
	ImGui::PopItemWidth();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Text("Lighting:");
	if (ImGui::RadioButton("None##Lighting", status.lightingMode == Mode_None)) { status.lightingMode = Mode_None; }
	ImGui::SameLine();
	if (ImGui::RadioButton("Flat##Lighting", status.lightingMode == Mode_Flat)) { status.lightingMode = Mode_Flat; }
	ImGui::SameLine();
	if (ImGui::RadioButton("Smooth##Lighting", status.lightingMode == Mode_Smooth)) { status.lightingMode = Mode_Smooth; }
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Checkbox("Wireframe", &status.wireframeEnabled);
	ImGui::SameLine();
	ImGui::TextDisabled("(W)");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Text("Camera:");
	if (ImGui::Button("Reset")) {
		status.ResetCamera();
	}
	ImGui::SameLine();
	ImGui::TextDisabled("(R)");
	ImGui::Checkbox("Autotracking", &status.autotracking);
	ImGui::End();
}

void RenderEditInfo(StatusManager& status)
{
	ImGui::Begin("Direct low-poly editing");

	ImGui::SetWindowPos(ImVec2(0, 334));
	if (status.animatedModel.has_value())
		ImGui::SetWindowSize(ImVec2(290, 130));
	else
		ImGui::SetWindowSize(ImVec2(290, (int)status.height - 334));

	ImGui::Text("Selection mode:");
	if (ImGui::RadioButton("Vertex##SelectionMode", status.selectionMode == Mode_Vertex)) { status.selectionMode = Mode_Vertex; }
	ImGui::SameLine();
	ImGui::TextDisabled("(1)");
	ImGui::SameLine();
	if (ImGui::RadioButton("Edge##SelectionMode", status.selectionMode == Mode_Edge)) { status.selectionMode = Mode_Edge; }
	ImGui::SameLine();
	ImGui::TextDisabled("(2)");
	ImGui::SameLine();
	if (ImGui::RadioButton("Face##SelectionMode", status.selectionMode == Mode_Face)) { status.selectionMode = Mode_Face; }
	ImGui::SameLine();
	ImGui::TextDisabled("(3)");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Text("Displace:\nOn plane (Alt+drag with left)\nAlong normal (Shift+drag with left)");
	ImGui::End();
}

void RenderAnimationsInfo(StatusManager& status)
{

	if (!status.animatedModel.has_value()) return;
	ImGui::Begin("Animations");
	ImGui::SetWindowPos(ImVec2(0, 640));
	ImGui::SetWindowSize(ImVec2(status.width, (int)status.height - 640));

	//ImGui::SameLine();
	if (ImGui::Button(status.pause ? "Play" : "Pause")) {
		status.Pause();
	}
	ImGui::SameLine();
	ImGui::TextDisabled("(Space)");
	ImGui::SameLine();

	//current animation
	Animation& anim = status.animator.animations[status.animator.currentAnimationIndex];
	anim.currentTime = std::clamp(anim.currentTime, anim.startFrom, anim.endAt);
	ImGui::SetNextItemWidth(150);
	ImGui::SliderFloat("Play speed", &anim.speed, 0.25f, 4.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
	ImGui::SameLine();

	ImGui::SetNextItemWidth(100);
	const char* modes[]{ "Loop", "Boomerang" };
	ImGui::Combo("Play Mode", &status.animator.playmode, modes, 2);
	// All animations
	static ImGuiTableFlags aflags = ImGuiTableFlags_SizingFixedFit;
	if (ImGui::BeginTable("Animations table", 2, aflags)) {

		for (int i = 0; i < status.animator.animations.size(); i++)
		{
			ImGui::TableNextRow();
			if (ShowAnimationNInfo(status.animator, i)) {
				status.RebakeModel();
			}
		}

		ImGui::TableNextColumn();
		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(anim.GetDuration() * 200);
		float tmp = anim.startFrom;
		ImGui::SliderFloat("Start range", &anim.startFrom, 0.0f, anim.GetDuration() - 0.1f);
		if (tmp < anim.startFrom && anim.startFrom >= anim.endAt - 0.1f)
			anim.startFrom = anim.endAt - 0.1f;
		ImGui::SetNextItemWidth(anim.GetDuration() * 200);
		tmp = anim.endAt;
		ImGui::SliderFloat("End range", &anim.endAt, 0.1f, anim.GetDuration());
		if (tmp > anim.endAt && anim.endAt <= anim.startFrom + 0.1f)
			anim.endAt = anim.startFrom + 0.1f;
		ImGui::EndTable();
	}
	ImGui::End();
}

bool ShowAnimationNInfo(Animator& animator, int n) {
	Animation& anim = animator.animations[n];

	ImGui::TableNextColumn();
	bool rebake = false;
	if (ImGui::RadioButton(anim.name.c_str(), animator.currentAnimationIndex == n)) {
		animator.PlayAnimationIndex(n);
		rebake = true;
	}
	ImGui::TableNextColumn();
	float currentTime = anim.currentTime;
	ImGui::SetNextItemWidth(anim.GetDuration() * 200);
	ImGui::SliderFloat(("##" + std::to_string(n)).c_str(), &currentTime, 0.0f, anim.GetDuration());
	if (currentTime != anim.currentTime)
	{
		anim.currentTime = currentTime;
		animator.PlayAnimationIndex(n);
		rebake = true;
	}
	return rebake;
}