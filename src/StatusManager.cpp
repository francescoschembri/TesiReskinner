#include "StatusManager.h"

StatusManager::StatusManager(float screenWidth, float screenHeight)
	:
	width(screenWidth),
	height(screenHeight),
	mouseLastPos(glm::vec2(screenWidth / 2, screenHeight / 2)),
	camera(glm::vec3(-0.5f, 0.3f, 3.0f)),
	camera2(glm::vec3(-0.5f, 0.3f, 3.0f)),
	lastFrame(glfwGetTime()),
	deltaTime(0.0f),
	pause(false),
	wireframeEnabled(false),
	projection(glm::perspective(glm::radians(FOV), screenWidth / screenHeight, NEAR_PLANE, FAR_PLANE)),
	HVAO(0),
	HVBO(0),
	SVAO(0),
	SVBO(0),
	lightingMode(Mode_Flat),
	visualMode(Mode_Texture),
	selectionMode(Mode_Vertex),
	modelFlatShader(Shader("./Shaders/animated_model_loading.vs", "./Shaders/animated_model_loading.fs", "./Shaders/animated_model_loading.gs")),
	normalShader(Shader("./Shaders/normal_visualizer.vs", "./Shaders/normal_visualizer.fs", "./Shaders/normal_visualizer.gs")),
	modelNoLightShader(Shader("./Shaders/no_lighting_shader.vs", "./Shaders/no_lighting_shader.fs")),
	modelSmoothShader(Shader("./Shaders/smooth_lighting_shader.vs", "./Shaders/smooth_lighting_shader.fs")),
	modelGreyShader(Shader("./Shaders/grey_model.vs", "./Shaders/grey_model.fs")),
	wireframeShader(Shader("./Shaders/wireframe.vs", "./Shaders/wireframe.fs")),
	mouseShader(Shader("./Shaders/mouse_shader.vs", "./Shaders/mouse_shader.fs")),
	hoverShader(Shader("./Shaders/hover.vs", "./Shaders/hover.fs")),
	selectedShader(Shader("./Shaders/selected.vs", "./Shaders/selected.fs")),
	numBonesShader(Shader("./Shaders/num_bones_visualization.vs", "./Shaders/num_bones_visualization.fs")),
	currentBoneShader(Shader("./Shaders/influence_of_single_bone.vs", "./Shaders/influence_of_single_bone.fs")),
	floorShader(Shader("./Shaders/floor.vs", "./Shaders/floor.fs")),
	bbShader(Shader("./Shaders/bb.vs", "./Shaders/bb.fs")),
	currentChange(Change(selectedVerticesPointers, ChangeOffset)),
	changeIndex(-1),
	currentChangeType(ChangeOffset),
	floorVertices(std::vector<float>()),
	isChanging(false),
	changes(std::vector<Change>()),
	autotracking(false),
	followedVertex(std::optional<Vertex>())
{
	// setup selected vertices vao
	glGenVertexArrays(1, &SVAO);
	glGenBuffers(1, &SVBO);
	// setup floor vertices vao
	glGenVertexArrays(1, &FVAO);
	glGenBuffers(1, &FVBO);
	for (float i = -2.5f; i <= 2.5f; i += 0.25f) {
		floorVertices.push_back(-2.5f);
		floorVertices.push_back(0.0f);
		floorVertices.push_back(i);
		floorVertices.push_back(2.5f);
		floorVertices.push_back(0.0f);
		floorVertices.push_back(i);

		floorVertices.push_back(i);
		floorVertices.push_back(0.0f);
		floorVertices.push_back(2.5f);
		floorVertices.push_back(i);
		floorVertices.push_back(0.0f);
		floorVertices.push_back(-2.5f);
	}
	// load data into vertex buffer
	glBindVertexArray(FVAO);
	glBindBuffer(GL_ARRAY_BUFFER, FVBO);
	glBufferData(GL_ARRAY_BUFFER, floorVertices.size() * sizeof(float), &floorVertices[0], GL_STATIC_DRAW);
	// we only care about the position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	glGenVertexArrays(1, &BBVAO);
	glGenBuffers(1, &BBVBO);
	// setup hovered vertices vao
	glGenVertexArrays(1, &HVAO);
	glGenBuffers(1, &HVBO);
	glBindVertexArray(HVAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, HVBO);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	// we only care about the position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);
}

void StatusManager::AddAnimation(const char* path)
{
	assert(animatedModel.has_value());
	animator.AddAnimation(Animation(std::string(path), *animatedModel));
}

void StatusManager::Pause()
{
	if (!animatedModel.has_value()) return;
	pause = !pause;
	if (pause)
		BakeModel();
	else {
		UnbakeModel();
		changes.clear();
		changeIndex = -1;
	}
}

void StatusManager::RebakeModel()
{
	UnbakeModel();
	BakeModel();
}

void StatusManager::SetPivot()
{
	assert(bakedModel.has_value());
	startInterpolation = glfwGetTime();

	if (info.hitPoint) {
		Mesh& hittedMesh = bakedModel->meshes[info.meshIndex];
		int closestIndex = getClosestVertexIndex(info.hitPoint.value(), hittedMesh, info.face.value());
		camera2.SetPivot(hittedMesh.vertices[closestIndex].Position);
		followedVertex = animatedModel->meshes[info.meshIndex].vertices[closestIndex];
		info = PickingInfo{};
	}
	else {
		glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
		camera2.SetPivot(center);
		followedVertex.reset();
	}
}

void StatusManager::UpdatePivot()
{
	if (autotracking && followedVertex.has_value()) {
		glm::vec3 pos = followedVertex->Position;
		glm::mat4 cumulativeMatrix = glm::mat4(0.0f);
		auto& matrices = animator.GetFinalBoneMatrices();
		for (int i = 0; i < followedVertex->BoneData.NumBones; i++) {
			cumulativeMatrix += (matrices[followedVertex->BoneData.BoneIDs[i]] * followedVertex->BoneData.Weights[i]);
		}
		pos = glm::vec3(cumulativeMatrix * glm::vec4(pos, 1.0f));
		camera.SetPivot(pos);
		camera2.SetPivot(pos);
	}
}

void StatusManager::UpdateDeltaTime()
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

void StatusManager::Update()
{
	UpdateDeltaTime();
	UpdatePivot();
	float step = std::min((float)glfwGetTime() - startInterpolation, CAMERA_STEP) / CAMERA_STEP;
	camera.Interpolate(camera2, std::min(step, 1.0f));
	if (pause)
		return;
	animator.UpdateAnimation(deltaTime);
}

void StatusManager::NextAnimation()
{
	animator.PlayNextAnimation();
}

void StatusManager::PrevAnimation()
{
	animator.PlayPrevAnimation();
}

void StatusManager::BakeModel() {
	assert(!bakedModel.has_value());
	bakedModel.emplace(animatedModel->Bake(animator.GetFinalBoneMatrices()));
}

void StatusManager::UnbakeModel()
{
	//assert(bakedModel);
	info.hitPoint.reset();
	bakedModel.reset();
	selectedVertices.clear();
}

PickingInfo StatusManager::Picking()
{
	assert(bakedModel.has_value());
	glm::vec2 mousePos = (mouseLastPos / glm::vec2(width, height)) * 2.0f - 1.0f;
	mousePos.y = -mousePos.y; //origin is top-left and +y mouse is down

	glm::vec4 rayStartPos = glm::vec4(mousePos, 0.0f, 1.0f);
	glm::vec4 rayEndPos = glm::vec4(mousePos, 1.0f, 1.0f);

	glm::mat4 toWorld = glm::inverse(projection * camera.GetViewMatrix());

	rayStartPos = toWorld * rayStartPos;
	rayEndPos = toWorld * rayEndPos;

	rayStartPos /= rayStartPos.w;
	rayEndPos /= rayEndPos.w;

	glm::vec3 dir = glm::normalize(glm::vec3(rayEndPos - rayStartPos));

	float minDist = 0.0f;

	PickingInfo res{};
	for (int i = 0; i < bakedModel->meshes.size(); i++)
	{
		Mesh& m = bakedModel->meshes[i];
		if (!animatedModel->meshes[i].enabled) continue;
		for (Face& f : m.faces)
		{
			Vertex& ver1 = m.vertices[f.indices[0]];
			Vertex& ver2 = m.vertices[f.indices[1]];
			Vertex& ver3 = m.vertices[f.indices[2]];
			IntersectionInfo tmpInfo = rayTriangleIntersection(rayStartPos, dir, ver1, ver2, ver3);
			if (tmpInfo.distance > FLT_EPSILON)
			{
				//object i has been clicked. probably best to find the minimum t1 (front-most object)
				if (!res.face || tmpInfo.distance < minDist)
				{
					res.hitPoint = tmpInfo.hitPoint;
					res.face.emplace(f);
					res.meshIndex = i;
					res.distance = minDist = tmpInfo.distance;
				}
			}
		}
	}

	return res;
}

void StatusManager::DrawSelectedVertices()
{
	if (selectedVertices.size() == 0) return;
	assert(bakedModel.has_value());
	selectedShader.use();
	glBindVertexArray(SVAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, SVBO);
	glBufferData(GL_ARRAY_BUFFER, selectedVertices.size() * sizeof(Vertex), &selectedVertices[0], GL_STATIC_DRAW);
	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
	// normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// bone ids
	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(2, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, BoneData.BoneIDs[0]));
	// weights
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, BoneData.Weights[0]));
	// num bones
	glEnableVertexAttribArray(4);
	glVertexAttribIPointer(4, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, BoneData.NumBones));
	// model/view/projection transformations
	selectedShader.setMat4("modelView", camera.GetViewMatrix());
	selectedShader.setMat4("projection", projection);
	// pass bones matrices to the shader
	auto transforms = animator.GetFinalBoneMatrices();
	for (int i = 0; i < transforms.size(); ++i)
		selectedShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);

	glPointSize(5.0f);
	glDrawArrays(GL_POINTS, 0, selectedVertices.size());

	if (currentChangeType == ChangeNormals)
	{
		normalShader.use();
		glBindVertexArray(SVAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, SVBO);
		// model/view/projection transformations
		normalShader.setMat4("modelView", camera.GetViewMatrix());
		normalShader.setMat4("projection", projection);
		// pass bones matrices to the shader
		auto transforms = animator.GetFinalBoneMatrices();
		for (int i = 0; i < transforms.size(); ++i)
			normalShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
		normalShader.setFloat("normal_length", 0.4f);

		//glDisable(GL_DEPTH_TEST);
		glLineWidth(5.0f);
		glDrawArrays(GL_POINTS, 0, selectedVertices.size());
		glLineWidth(1.0f);
		//glEnable(GL_DEPTH_TEST);
	}

	//unbind
	glBindVertexArray(0);
}

void StatusManager::Undo()
{
	if (changeIndex >= 0) {
		assert(changeIndex < changes.size());
		changes[changeIndex--].Undo();
		animatedModel.value().Reload();
		bakedModel.value().Reload();
	}
	UpdateSelectedVertices();
}

void StatusManager::Redo()
{
	assert(changeIndex >= -1);
	assert(-1 < 0);
	int size = changes.size();
	assert(changeIndex < size);
	if (changeIndex - size + 1) {
		assert(size > 0);
		changes[++changeIndex].Redo();
		animatedModel.value().Reload();
	}
	UpdateSelectedVertices();
}

void StatusManager::LoadModel(std::string& path)
{
	animator.animations.clear();
	animator.currentAnimationIndex = 0;
	texMan.ClearTextures();
	UnbakeModel();
	animatedModel.emplace(path, texMan);
	AddAnimation(path.c_str());

	Update();
	BakeModel();
	glm::vec3 center = bakedModel->bb.GetCenter();
	camera.SetPivot(center);
	camera2.SetPivot(center);
	if(!pause)
		UnbakeModel();
}

void StatusManager::CompleteLoad(std::string& path)
{
	animator.animations.clear();
	animator.currentAnimationIndex = 0;
	texMan.ClearTextures();
	UnbakeModel();
	animatedModel.emplace(path, texMan);
	std::string dir_name = path.substr(0, path.find_last_of("/"));
	dir_name = dir_name.substr(0, dir_name.find_last_of("/"));
	auto dir = std::filesystem::recursive_directory_iterator(dir_name);
	for (auto animation : dir) {
		if (animation.path().extension().u8string().compare(".dae") != 0)
			continue;
		dir_name = animation.path().u8string();
		std::replace(dir_name.begin(), dir_name.end(), '\\', '/');
		AddAnimation(dir_name.c_str());
	}

	Update();
	BakeModel();
	glm::vec3 center = bakedModel->bb.GetCenter();
	camera.SetPivot(center);
	camera2.SetPivot(center);
	if(!pause)
		UnbakeModel();
}

bool StatusManager::SelectHoveredVertex()
{
	assert(info.hitPoint.has_value());
	assert(bakedModel.has_value());

	Mesh& bMesh = bakedModel.value().meshes[info.meshIndex];

	Face& f = info.face.value();
	int verIndex = getClosestVertexIndex(info.hitPoint.value(), bMesh, f);
	Vertex* v = &bMesh.vertices[verIndex];
	if (v->originalVertex->BoneData.NumBones < 4) return false;
	//avoid duplicates and allow removing selected vertices
	auto iter = std::find(selectedVertices.begin(), selectedVertices.end(), *v);
	int index = iter - selectedVertices.begin();
	if (iter == selectedVertices.end()) {
		selectedVertices.push_back(*v);
		selectedVerticesPointers.push_back(v);
	}
	else {
		selectedVertices.erase(iter);
		selectedVerticesPointers.erase(selectedVerticesPointers.begin() + index);
	}
	return true;
}

bool StatusManager::SelectHoveredEdge()
{
	Mesh& bMesh = bakedModel.value().meshes[info.meshIndex];
	Face& f = info.face.value();
	Line closestLine = getClosestLineIndex(info.hitPoint.value(), bMesh, f);
	Vertex* v1 = &bMesh.vertices[closestLine.v1];
	Vertex* v2 = &bMesh.vertices[closestLine.v2];
	auto iter1 = std::find(selectedVertices.begin(), selectedVertices.end(), *v1);
	auto iter2 = std::find(selectedVertices.begin(), selectedVertices.end(), *v2);
	bool ins1 = iter1 == selectedVertices.end();
	bool ins2 = iter2 == selectedVertices.end();
	bool allInside = !(ins1 || ins2);
	int index1 = iter1 - selectedVertices.begin();
	int index2 = iter2 - selectedVertices.begin();

	if (v1->originalVertex->BoneData.NumBones == 4 &&
		v2->originalVertex->BoneData.NumBones == 4) {
		//avoid duplicates and allow removing selected vertices
		if (allInside) {
			if (index2 < index1) {
				selectedVertices.erase(selectedVertices.begin() + index1);
				selectedVertices.erase(selectedVertices.begin() + index2);
				selectedVerticesPointers.erase(selectedVerticesPointers.begin() + index1);
				selectedVerticesPointers.erase(selectedVerticesPointers.begin() + index2);
			}
			else {
				selectedVertices.erase(selectedVertices.begin() + index2);
				selectedVertices.erase(selectedVertices.begin() + index1);
				selectedVerticesPointers.erase(selectedVerticesPointers.begin() + index2);
				selectedVerticesPointers.erase(selectedVerticesPointers.begin() + index1);
			}
			return false;
		}

		if (ins1) {
			selectedVertices.push_back(*v1);
			selectedVerticesPointers.push_back(v1);
		}
		if (ins2) {
			selectedVertices.push_back(*v2);
			selectedVerticesPointers.push_back(v2);
		}
	}
	return true;
}

bool StatusManager::SelectHoveredFace()
{
	Mesh& bMesh = bakedModel.value().meshes[info.meshIndex];
	Face& f = info.face.value();
	Vertex* v1 = &bMesh.vertices[f.indices[0]];
	Vertex* v2 = &bMesh.vertices[f.indices[1]];
	Vertex* v3 = &bMesh.vertices[f.indices[2]];

	auto iter1 = std::find(selectedVertices.begin(), selectedVertices.end(), *v1);
	auto iter2 = std::find(selectedVertices.begin(), selectedVertices.end(), *v2);
	auto iter3 = std::find(selectedVertices.begin(), selectedVertices.end(), *v3);
	bool ins1 = iter1 == selectedVertices.end();
	bool ins2 = iter2 == selectedVertices.end();
	bool ins3 = iter3 == selectedVertices.end();
	bool allInside = !(ins1 || ins2 || ins3);
	int index1 = iter1 - selectedVertices.begin();
	int index2 = iter2 - selectedVertices.begin();
	int index3 = iter3 - selectedVertices.begin();

	if (v1->originalVertex->BoneData.NumBones == 4 &&
		v2->originalVertex->BoneData.NumBones == 4 &&
		v3->originalVertex->BoneData.NumBones == 4) {
		//avoid duplicates and allow removing selected vertices
		if (allInside) {
			selectedVertices.erase(selectedVertices.begin() + index1);
			selectedVerticesPointers.erase(selectedVerticesPointers.begin() + index1);

			index2 = std::find(selectedVertices.begin(), selectedVertices.end(), *v2) - selectedVertices.begin();
			selectedVertices.erase(selectedVertices.begin() + index2);
			selectedVerticesPointers.erase(selectedVerticesPointers.begin() + index2);

			index3 = std::find(selectedVertices.begin(), selectedVertices.end(), *v3) - selectedVertices.begin();
			selectedVertices.erase(selectedVertices.begin() + index3);
			selectedVerticesPointers.erase(selectedVerticesPointers.begin() + index3);
			return false;
		}

		if (ins1) {
			selectedVertices.push_back(*v1);
			selectedVerticesPointers.push_back(v1);
		}
		if (ins2) {
			selectedVertices.push_back(*v2);
			selectedVerticesPointers.push_back(v2);
		}
		if (ins3) {
			selectedVertices.push_back(*v3);
			selectedVerticesPointers.push_back(v3);
		}
	}
	return true;
}

void StatusManager::StartChange()
{
	startChangingPos = info.hitPoint.value();
	rayLenghtOnChangeStart = info.distance;

	currentChange = Change(selectedVerticesPointers, currentChangeType);
	info = PickingInfo{};
}

void StatusManager::EndChange()
{
	currentChange.End();

	//remove changes that were rollbacked from the history
	int currSize = changes.size();
	for (int i = changeIndex + 1; i < currSize; i++) {
		changes.pop_back();
	}

	changes.push_back(currentChange);
	changeIndex++;
	isChanging = false;
	animatedModel->Reload();
}

void StatusManager::TweakSelectedVertices()
{
	isChanging = true;
	// make a new ray
	glm::vec2 mousePos = (mouseLastPos / glm::vec2(width, height)) * 2.0f - 1.0f;
	mousePos.y = -mousePos.y; //origin is top-left and +y mouse is down

	glm::vec4 rayStartPos = glm::vec4(mousePos, 0.0f, 1.0f);
	glm::vec4 rayEndPos = glm::vec4(mousePos, 1.0f, 1.0f);

	glm::mat4 toWorld = glm::inverse(projection * camera.GetViewMatrix());

	rayStartPos = toWorld * rayStartPos;
	rayEndPos = toWorld * rayEndPos;

	rayStartPos /= rayStartPos.w;
	rayEndPos /= rayEndPos.w;

	glm::vec3 dir = glm::normalize(glm::vec3(rayEndPos - rayStartPos));

	hotPoint = glm::vec3(rayStartPos) + dir * rayLenghtOnChangeStart;

	if (currentChangeType == ChangeNormals)
		MoveAlongNormalsSelectedVertices();
	else
		MoveAlongCameraPlaneSelectedVertices();

	bakedModel.value().Reload();
	UpdateSelectedVertices();
}

void StatusManager::MoveAlongCameraPlaneSelectedVertices()
{
	glm::vec3 offset = hotPoint - startChangingPos;
	currentChange.Modify(offset);
}

void StatusManager::MoveAlongNormalsSelectedVertices()
{
	float displacement = hotPoint.y - startChangingPos.y;
	currentChange.Modify(displacement);
}

void StatusManager::IncreaseCurrentBoneID()
{
	currentBoneID++;
}

void StatusManager::DecreaseCurrentBoneID()
{
	currentBoneID--;
}

void StatusManager::Render()
{
	//TODO REVIEW
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawFloor();
	if (!animatedModel)
		return;
	Update();
	//DrawBoundingBoxes();
	// draw wireframe if enabled
	if (wireframeEnabled)
		DrawWireframe();
	if (visualMode == Mode_Texture) {
		// draw model
		if (lightingMode == Mode_Flat)
			DrawModel(modelFlatShader);
		else if (lightingMode == Mode_Smooth)
			DrawModel(modelSmoothShader);
		else
			DrawModel(modelNoLightShader);
	}
	else if (visualMode == Mode_CurrentBoneIDInfluence) {
		DrawModel(currentBoneShader);
	}
	else if (visualMode == Mode_NumBones)
	{
		DrawModel(numBonesShader);
	}
	else
	{
		DrawModel(modelGreyShader);
	}

	// render selected vertices
	DrawSelectedVertices();

	if (!info.hitPoint)
		return;

	//DrawHotPoint();
	if (selectionMode == Mode_Vertex)
		DrawHoveredPoint();
	if (selectionMode == Mode_Edge)
		DrawHoveredLine();
	if (selectionMode == Mode_Face)
		DrawHoveredFace();
}

void StatusManager::ResetCamera()
{
	camera2.Reset();
	startInterpolation = glfwGetTime();
}

void StatusManager::DrawWireframe() {
	wireframeShader.use();
	glLineWidth(1.0f);
	// model/view/projection transformations
	wireframeShader.setMat4("modelView", camera.GetViewMatrix());
	wireframeShader.setMat4("projection", projection);

	// pass bones matrices to the shader
	auto transforms = animator.GetFinalBoneMatrices();
	for (int i = 0; i < transforms.size(); ++i)
		wireframeShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (isChanging)
		bakedModel.value().Draw(wireframeShader);
	else
		animatedModel.value().Draw(wireframeShader);
}

void StatusManager::DrawFloor()
{
	floorShader.use();
	glLineWidth(3.0f);
	floorShader.setMat4("modelView", camera.GetViewMatrix());
	floorShader.setMat4("projection", projection);
	glBindVertexArray(FVAO);
	glDrawArrays(GL_LINES, 0, 84);
}

void StatusManager::DrawModel(Shader& modelShader) {
	modelShader.use();
	// model/view/projection transformations
	modelShader.setMat4("modelView", camera.GetViewMatrix());
	modelShader.setMat4("projection", projection);
	if (visualMode == Mode_Texture)
		modelShader.setVec3("light_dir", lightDir);
	else if (visualMode == Mode_CurrentBoneIDInfluence)
		modelShader.setInt("currentBoneID", currentBoneID);

	// pass bones matrices to the shader
	auto transforms = animator.GetFinalBoneMatrices();
	for (int i = 0; i < transforms.size(); ++i)
		modelShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);
	if (isChanging)
		bakedModel.value().Draw(modelShader);
	else
		animatedModel.value().Draw(modelShader);
}

void StatusManager::DrawHoveredFace() {
	assert(bakedModel.has_value());
	assert(info.hitPoint.has_value());
	Mesh& m = bakedModel.value().meshes[info.meshIndex];
	Face& f = info.face.value();

	//vertex
	float hoveredVertices[9] = {
		m.vertices[f.indices[0]].Position.x, m.vertices[f.indices[0]].Position.y, m.vertices[f.indices[0]].Position.z,
		m.vertices[f.indices[1]].Position.x, m.vertices[f.indices[1]].Position.y, m.vertices[f.indices[1]].Position.z,
		m.vertices[f.indices[2]].Position.x, m.vertices[f.indices[2]].Position.y, m.vertices[f.indices[2]].Position.z,
	};
	hoverShader.use();
	glBindVertexArray(HVAO);
	glBindBuffer(GL_ARRAY_BUFFER, HVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(hoveredVertices), &hoveredVertices);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(0.0, 0.0);

	// model/view/projection transformations
	hoverShader.setMat4("modelView", camera.GetViewMatrix());
	hoverShader.setMat4("projection", projection);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	//unbind
	glBindVertexArray(0);
}

void StatusManager::DrawHoveredPoint() {
	assert(bakedModel.has_value());
	assert(info.hitPoint.has_value());
	Mesh& m = bakedModel.value().meshes[info.meshIndex];
	Face& f = info.face.value();
	int index = getClosestVertexIndex(info.hitPoint.value(), m, f);

	//vertex
	float hoveredVertices[3] = { m.vertices[index].Position.x, m.vertices[index].Position.y, m.vertices[index].Position.z };

	hoverShader.use();
	glBindVertexArray(HVAO);
	glBindBuffer(GL_ARRAY_BUFFER, HVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float), &hoveredVertices);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// model/view/projection transformations
	hoverShader.setMat4("modelView", camera.GetViewMatrix());
	hoverShader.setMat4("projection", projection);

	glPointSize(8.0f);
	glDrawArrays(GL_POINTS, 0, 1);

	//unbind
	glBindVertexArray(0);
}

void StatusManager::DrawHotPoint()
{
	float hotVertices[3] = { hotPoint.x, hotPoint.y, hotPoint.z };

	hoverShader.use();
	glBindVertexArray(HVAO);
	glBindBuffer(GL_ARRAY_BUFFER, HVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float), &hotVertices);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// model/view/projection transformations
	hoverShader.setMat4("modelView", camera.GetViewMatrix());
	hoverShader.setMat4("projection", projection);

	glPointSize(8.0f);
	glDrawArrays(GL_POINTS, 0, 1);

	//unbind
	glBindVertexArray(0);
}

void StatusManager::DrawBoundingBoxes()
{
	if (!bakedModel.has_value()) return;
	bbShader.use();
	glLineWidth(5.0f);
	bbShader.setMat4("modelView", camera.GetViewMatrix());
	bbShader.setMat4("projection", projection);
	Mesh& m = bakedModel->meshes[4];
		float bbVertices[72] = {
			//verticals
			m.bb.minX, m.bb.minY, m.bb.minZ,
			m.bb.minX, m.bb.maxY, m.bb.minZ,
			m.bb.minX, m.bb.minY, m.bb.maxZ,
			m.bb.minX, m.bb.maxY, m.bb.maxZ,
			m.bb.maxX, m.bb.minY, m.bb.minZ,
			m.bb.maxX, m.bb.maxY, m.bb.minZ,
			m.bb.maxX, m.bb.minY, m.bb.maxZ,
			m.bb.maxX, m.bb.maxY, m.bb.maxZ,
			//bottom left
			m.bb.minX, m.bb.minY, m.bb.minZ,
			m.bb.minX, m.bb.minY, m.bb.maxZ,
			//bottom right
			m.bb.maxX, m.bb.minY, m.bb.minZ,
			m.bb.maxX, m.bb.minY, m.bb.maxZ,
			//bottom front	 
			m.bb.minX, m.bb.minY, m.bb.minZ,
			m.bb.maxX, m.bb.minY, m.bb.minZ,
			//bottom back
			m.bb.minX, m.bb.minY, m.bb.maxZ,
			m.bb.maxX, m.bb.minY, m.bb.maxZ,
			//up front	 
			m.bb.minX, m.bb.maxY, m.bb.minZ,
			m.bb.maxX, m.bb.maxY, m.bb.minZ,
			//up right
			m.bb.maxX, m.bb.maxY, m.bb.minZ,
			m.bb.maxX, m.bb.maxY, m.bb.maxZ,
			//up back
			m.bb.minX, m.bb.maxY, m.bb.maxZ,
			m.bb.maxX, m.bb.maxY, m.bb.maxZ,
			//up left
			m.bb.minX, m.bb.maxY, m.bb.minZ,
			m.bb.minX, m.bb.maxY, m.bb.maxZ
		};
		// load data into vertex buffer
		glBindVertexArray(BBVAO);
		glBindBuffer(GL_ARRAY_BUFFER, BBVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(bbVertices) * sizeof(float), &bbVertices[0], GL_STATIC_DRAW);
		// we only care about the position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glDrawArrays(GL_LINES, 0, 24);
	
}

void StatusManager::UpdateSelectedVertices()
{
	selectedVertices.clear();
	for (Vertex* v : selectedVerticesPointers)
		selectedVertices.push_back(*v);
}

void StatusManager::DrawHoveredLine() {
	assert(bakedModel.has_value());
	assert(info.hitPoint.has_value());
	Mesh& m = bakedModel.value().meshes[info.meshIndex];
	Face& f = info.face.value();
	auto line = getClosestLineIndex(info.hitPoint.value(), m, f);

	//vertex
	float hoveredVertices[6] = {
		m.vertices[line.v1].Position.x, m.vertices[line.v1].Position.y, m.vertices[line.v1].Position.z,
		m.vertices[line.v2].Position.x, m.vertices[line.v2].Position.y, m.vertices[line.v2].Position.z
	};

	hoverShader.use();
	glBindVertexArray(HVAO);
	glBindBuffer(GL_ARRAY_BUFFER, HVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(hoveredVertices), &hoveredVertices);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// model/view/projection transformations
	hoverShader.setMat4("modelView", camera.GetViewMatrix());
	hoverShader.setMat4("projection", projection);

	glLineWidth(3.0f);
	glDrawArrays(GL_LINES, 0, 2);
	glLineWidth(1.0f);

	//unbind
	glBindVertexArray(0);
}