cmake_minimum_required(VERSION 3.21.3)

project(Reskinner)

set(CMAKE_CXX_STANDARD 17)     
set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(${PROJECT_NAME} Main.cpp)

set(ASSIMP_LIBRARY_DIRS "external/assimp/code")
set(ASSIMP_INCLUDE_DIRS "external/assimp/include")
include_directories(${ASSIMP_INCLUDE_DIRS})
link_directories(${ASSIMP_LIBRARY_DIRS})


add_subdirectory(external/glfw)
add_subdirectory(external/assimp)
add_subdirectory(external/glad)
add_subdirectory(external/imgui)
add_subdirectory(external/stb)
add_subdirectory(external/glm)
add_subdirectory(external/FileBrowser)
add_subdirectory(src)

find_package(OpenGL REQUIRED)
find_package(GLM REQUIRED)

target_include_directories(${PROJECT_NAME}
    PUBLIC external/glfw/include
    PUBLIC external/assimp/include
    PUBLIC ${GLM_INCLUDE_DIR}
    PUBLIC external
    PUBLIC src
)

target_link_directories(${PROJECT_NAME}
    PRIVATE ${OPENGL_LIBRARY}
)

target_link_libraries(${PROJECT_NAME} glfw glad imgui stb glm Engine assimp FileBrowser)