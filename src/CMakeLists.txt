add_library(Engine
	Animation.cpp
	Animation.h
	Animator.cpp
	Animator.h
	AssimpNodeData.h
	Bone.cpp
	Bone.h
	BoneInfo.h
	BoundingBox.cpp
	BoundingBox.h
	Camera.cpp
	Camera.h
	Change.cpp
	Change.h
	eigen_glm_helpers.h
	Face.h
	GUI.cpp
	GUI.h
	Mesh.cpp
	Mesh.h
	Model.cpp
	Model.h
	Ray.cpp
	Ray.h
	Shader.cpp
	Shader.h
	StatusManager.cpp
	StatusManager.h
	Texture.h
	TextureManager.cpp
	TextureManager.h
	Utility.cpp
	Utility.h
	Vertex.h
	VertexBoneData.h
	Window.cpp
	Window.h
)

target_link_libraries(Engine
	${OpenGL_LIBRARIES} 
	${glad_LIBRARIES}
	${glfw_LIBRARIES} 
	${glm_LIBRARIES} 
	stb
	${assimp_LIBRARIES}
	imgui
	FileBrowser
)

target_include_directories(Engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_include_directories(Engine PUBLIC "${assimp_INCLUDE_DIRS}")
target_include_directories(Engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")