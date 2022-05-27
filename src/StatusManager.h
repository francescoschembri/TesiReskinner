#pragma once

#include "Camera.h"
#include "Animator.h"
#include "TextureManager.h"
#include "Utility.h"
#include "Shader.h"
#include "Change.h"

#include <optional>
#include <utility>
#include <algorithm>
#include <cassert>
#include <filesystem>

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

constexpr float CAMERA_STEP = 0.05f;

enum SelectionMode
{
	Mode_Vertex,
	Mode_Edge,
	Mode_Face
};

enum LightingMode
{
	Mode_Smooth,
	Mode_Flat,
	Mode_None
};

enum VisualMode
{
	Mode_Grey,
	Mode_Texture,
	Mode_CurrentBoneIDInfluence,
	Mode_NumBones
};

class StatusManager
{
public:
	glm::vec2 mouseLastPos;
	Camera camera, camera2;
	bool autotracking;
	std::optional<Vertex> followedVertex;
	float startInterpolation = 0.3f;
	Animator animator;
	TextureManager texMan;
	std::optional<Model> animatedModel, bakedModel;
	float lastFrame;
	float deltaTime;
	//status of the render
	bool pause;
	bool wireframeEnabled;
	//info about the window
	float width = 1920.0f;
	float height = 991.0f;
	glm::mat4 projection = glm::mat4(1.0f);
	//additional info
	std::vector<Vertex> selectedVertices;
	std::vector<Vertex*> selectedVerticesPointers;
	PickingInfo info;
	//variables for tweaking
	bool isChanging = false;
	ChangeType currentChangeType;
	Change currentChange;
	std::vector<Change> changes;
	int changeIndex = -1;
	glm::vec3 startChangingPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 hotPoint = glm::vec3(0.0f, 0.0f, 0.0f);
	float rayLenghtOnChangeStart = -1.0f;
	//buffers to render hovered and selected stuffs
	GLuint HVBO, HVAO;
	GLuint SVBO, SVAO;
	GLuint FVBO, FVAO;
	GLuint BBVAO, BBVBO;
	//shaders to render the model and the gizmos
	LightingMode lightingMode;
	VisualMode visualMode;
	Shader modelFlatShader;
	Shader modelGreyShader;
	Shader modelSmoothShader;
	Shader modelNoLightShader;
	Shader wireframeShader;
	Shader mouseShader;
	Shader hoverShader;
	Shader selectedShader;
	Shader numBonesShader;
	Shader currentBoneShader;
	Shader normalShader;
	Shader floorShader;
	Shader bbShader;
	int currentBoneID = 0;
	SelectionMode selectionMode = Mode_Vertex;
	glm::vec3 lightDir = glm::vec3(0.0f, 0.0f, 1.0f);
	std::vector<float> floorVertices;

	StatusManager(float screenWidth, float screenHeight);

	//loading functions
	void AddAnimation(const char* path);
	void LoadModel(std::string& path);
	void CompleteLoad(std::string& path);

	//animation management
	void Pause();
	void RebakeModel();
	void NextAnimation();
	void PrevAnimation();
	void Update();
	void UpdateDeltaTime();

	//utilities
	PickingInfo Picking();
	void SetPivot();
	void UpdatePivot();
	bool SelectHoveredVertex();
	bool SelectHoveredEdge();
	bool SelectHoveredFace();

	//tweaking
	void StartChange();
	void EndChange();
	void TweakSelectedVertices();
	void MoveAlongCameraPlaneSelectedVertices();
	void MoveAlongNormalsSelectedVertices();
	void Undo();
	void Redo();

	//visual output
	void IncreaseCurrentBoneID();
	void DecreaseCurrentBoneID();
	void Render();
	void ResetCamera();
	

private:
	// rendering functions
	void DrawWireframe();
	void DrawFloor();
	void DrawModel(Shader& modelShader);
	void DrawSelectedVertices();
	void DrawHoveredFace();
	void DrawHoveredLine();
	void DrawHoveredPoint();
	void DrawHotPoint();
	void DrawBoundingBoxes();

	//utilities
	void UpdateSelectedVertices();
	void BakeModel();
	void UnbakeModel();
};