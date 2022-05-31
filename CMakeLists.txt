cmake_minimum_required(VERSION 3.21.3)

project(Reskinner)

set(CMAKE_CXX_STANDARD 17)     
set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(${PROJECT_NAME} Main.cpp)

include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

#   Retrieve conan.cmake
if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake" "${CMAKE_BINARY_DIR}/conan.cmake")
endif ()

include("${CMAKE_BINARY_DIR}/conan.cmake")

conan_cmake_run(CONANFILE "conanfile.txt" BASIC_SETUP UPDATE BUILD missing)
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

find_package(OpenGL REQUIRED)
find_package(glad REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)

add_subdirectory(external/stb)
add_subdirectory(external/imgui)
add_subdirectory(external/FileBrowser)
add_subdirectory(src)

file(MAKE_DIRECTORY
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../Shaders")
		
file(MAKE_DIRECTORY
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../Animations")

function(install_file_to_bin dir_name file_name)
    file(INSTALL "${CMAKE_SOURCE_DIR}/${dir_name}/${file_name}" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../${dir_name}")
endfunction()

install_file_to_bin(Shaders 1.model_loading.fs)
install_file_to_bin(Shaders 1.model_loading.vs)
install_file_to_bin(Shaders 4.1.fshader.fs)
install_file_to_bin(Shaders 4.1.vshader.vs)
install_file_to_bin(Shaders animated_model_loading.fs)
install_file_to_bin(Shaders animated_model_loading.gs)
install_file_to_bin(Shaders animated_model_loading.vs)
install_file_to_bin(Shaders bb.vs)
install_file_to_bin(Shaders bb.fs)
install_file_to_bin(Shaders default.frag)
install_file_to_bin(Shaders default.fs)
install_file_to_bin(Shaders default.geom)
install_file_to_bin(Shaders default.vert)
install_file_to_bin(Shaders default.vs)
install_file_to_bin(Shaders floor.fs)
install_file_to_bin(Shaders floor.vs)
install_file_to_bin(Shaders framebuffer.frag)
install_file_to_bin(Shaders framebuffer.vert)
install_file_to_bin(Shaders grey_model.fs)
install_file_to_bin(Shaders grey_model.vs)
install_file_to_bin(Shaders hover.fs)
install_file_to_bin(Shaders hover.vs)
install_file_to_bin(Shaders influence_of_single_bone.fs)
install_file_to_bin(Shaders influence_of_single_bone.vs)
install_file_to_bin(Shaders mouse_shader.fs)
install_file_to_bin(Shaders mouse_shader.vs)
install_file_to_bin(Shaders normal_visualizer.fs)
install_file_to_bin(Shaders normal_visualizer.gs)
install_file_to_bin(Shaders normal_visualizer.vs)
install_file_to_bin(Shaders no_lighting_shader.fs)
install_file_to_bin(Shaders no_lighting_shader.vs)
install_file_to_bin(Shaders num_bones_visualization.fs)
install_file_to_bin(Shaders num_bones_visualization.vs)
install_file_to_bin(Shaders screen_shader.fs)
install_file_to_bin(Shaders screen_shader.vs)
install_file_to_bin(Shaders selected.fs)
install_file_to_bin(Shaders selected.vs)
install_file_to_bin(Shaders smooth_lighting_shader.fs)
install_file_to_bin(Shaders smooth_lighting_shader.vs)
install_file_to_bin(Shaders wireframe.vs)
install_file_to_bin(Shaders wireframe.fs)

install_file_to_bin(Animations "Nonna")
install_file_to_bin(Animations "Spadaccina")
install_file_to_bin(Animations "Topo")
install_file_to_bin(Animations "Nonna/Capoeira")
install_file_to_bin(Animations "Nonna/Flair")
install_file_to_bin(Animations "Nonna/Silly Dancing")
install_file_to_bin(Animations "Nonna/Capoeira/Capoeira.dae")
install_file_to_bin(Animations "Nonna/Capoeira/textures")
install_file_to_bin(Animations "Nonna/Capoeira/textures/FitnessGrandma_diffuse.jpg")
install_file_to_bin(Animations "Nonna/Capoeira/textures/FitnessGrandma_FaceAnimMap.png")
install_file_to_bin(Animations "Nonna/Capoeira/textures/FitnessGrandma_normal.jpg")
install_file_to_bin(Animations "Nonna/Capoeira/textures/FitnessGrandma_spec.jpg")
install_file_to_bin(Animations "Nonna/Flair/Flair.dae")
install_file_to_bin(Animations "Nonna/Flair/textures")
install_file_to_bin(Animations "Nonna/Flair/textures/FitnessGrandma_diffuse.jpg")
install_file_to_bin(Animations "Nonna/Flair/textures/FitnessGrandma_FaceAnimMap.png")
install_file_to_bin(Animations "Nonna/Flair/textures/FitnessGrandma_normal.jpg")
install_file_to_bin(Animations "Nonna/Flair/textures/FitnessGrandma_spec.jpg")
install_file_to_bin(Animations "Nonna/Silly Dancing/Silly Dancing.dae")
install_file_to_bin(Animations "Nonna/Silly Dancing/textures")
install_file_to_bin(Animations "Nonna/Silly Dancing/textures/FitnessGrandma_diffuse.jpg")
install_file_to_bin(Animations "Nonna/Silly Dancing/textures/FitnessGrandma_FaceAnimMap.png")
install_file_to_bin(Animations "Nonna/Silly Dancing/textures/FitnessGrandma_normal.jpg")
install_file_to_bin(Animations "Nonna/Silly Dancing/textures/FitnessGrandma_spec.jpg")
install_file_to_bin(Animations "Spadaccina/Great Sword Casting")
install_file_to_bin(Animations "Spadaccina/Great Sword Casting/Great Sword Casting.dae")
install_file_to_bin(Animations "Spadaccina/Great Sword Casting/textures")
install_file_to_bin(Animations "Spadaccina/Great Sword Casting/textures/maria_diffuse.png")
install_file_to_bin(Animations "Spadaccina/Great Sword Casting/textures/maria_normal.png")
install_file_to_bin(Animations "Spadaccina/Great Sword Casting/textures/maria_specular.png")
install_file_to_bin(Animations "Topo/Rumba Topo")
install_file_to_bin(Animations "Topo/Rumba Topo/Rumba Dancing.dae")
install_file_to_bin(Animations "Topo/Rumba Topo/textures")
install_file_to_bin(Animations "Topo/Rumba Topo/textures/Ch14_1001_Diffuse.png")
install_file_to_bin(Animations "Topo/Rumba Topo/textures/Ch14_1001_Glossiness.png")
install_file_to_bin(Animations "Topo/Rumba Topo/textures/Ch14_1001_Normal.png")
install_file_to_bin(Animations "Topo/Rumba Topo/textures/Ch14_1001_Specular.png")


set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE 1)
set(CMAKE_LINK_WHAT_YOU_USE 1)
set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

include_directories(
    ${OpenGL_INCLUDE_DIRS}
    ${glad_INCLUDE_DIRS}
    ${glfw3_INCLUDE_DIRS}
    ${glm_INCLUDE_DIRS}
    ${assimp_INCLUDE_DIRS}
    external
    src
)

target_link_libraries(${PROJECT_NAME} 
	${OpenGL_LIBRARIES} 
	${glad_LIBRARIES}
	${glfw3_LIBRARIES} 
	${glm_LIBRARIES} 
	stb
	${assimp_LIBRARIES}
	imgui
	FileBrowser
	Engine 
)