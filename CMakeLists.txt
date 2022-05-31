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

function(install_file_to_bin file_name)
    file(INSTALL "${CMAKE_SOURCE_DIR}/Shaders/${file_name}" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../Shaders")
endfunction()

install_file_to_bin(1.model_loading.fs)
install_file_to_bin(1.model_loading.vs)
install_file_to_bin(4.1.fshader.fs)
install_file_to_bin(4.1.vshader.vs)
install_file_to_bin(animated_model_loading.fs)
install_file_to_bin(animated_model_loading.gs)
install_file_to_bin(animated_model_loading.vs)
install_file_to_bin(bb.vs)
install_file_to_bin(bb.fs)
install_file_to_bin(default.frag)
install_file_to_bin(default.fs)
install_file_to_bin(default.geom)
install_file_to_bin(default.vert)
install_file_to_bin(default.vs)
install_file_to_bin(floor.fs)
install_file_to_bin(floor.vs)
install_file_to_bin(framebuffer.frag)
install_file_to_bin(framebuffer.vert)
install_file_to_bin(grey_model.fs)
install_file_to_bin(grey_model.vs)
install_file_to_bin(hover.fs)
install_file_to_bin(hover.vs)
install_file_to_bin(influence_of_single_bone.fs)
install_file_to_bin(influence_of_single_bone.vs)
install_file_to_bin(mouse_shader.fs)
install_file_to_bin(mouse_shader.vs)
install_file_to_bin(normal_visualizer.fs)
install_file_to_bin(normal_visualizer.gs)
install_file_to_bin(normal_visualizer.vs)
install_file_to_bin(no_lighting_shader.fs)
install_file_to_bin(no_lighting_shader.vs)
install_file_to_bin(num_bones_visualization.fs)
install_file_to_bin(num_bones_visualization.vs)
install_file_to_bin(screen_shader.fs)
install_file_to_bin(screen_shader.vs)
install_file_to_bin(selected.fs)
install_file_to_bin(selected.vs)
install_file_to_bin(smooth_lighting_shader.fs)
install_file_to_bin(smooth_lighting_shader.vs)
install_file_to_bin(wireframe.vs)
install_file_to_bin(wireframe.fs)

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