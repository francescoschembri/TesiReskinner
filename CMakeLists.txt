cmake_minimum_required(VERSION 3.21.3)

project(Reskinner)

set(CMAKE_CXX_STANDARD 17)     
set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(${PROJECT_NAME} Main.cpp)

add_subdirectory(external/glfw)
add_subdirectory(external/assimp)
add_subdirectory(external/glad)
add_subdirectory(external/imgui)
add_subdirectory(external/stb)
add_subdirectory(external/glm)
add_subdirectory(external/FileBrowser)
add_subdirectory(src)

add_custom_target(always_rebuild ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/res ${CMAKE_BINARY_DIR}/res
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/examples/res ${CMAKE_BINARY_DIR}/examples/res
)

target_include_directories(${PROJECT_NAME}
    PUBLIC external/glfw/include
    PUBLIC external/assimp/include
    PUBLIC external/glm
    PUBLIC external
    PUBLIC external
    PUBLIC external
    PUBLIC external
    PUBLIC src
)

target_link_directories(${PROJECT_NAME}
    PRIVATE external/glfw/src
    PRIVATE external/assimp/code
    PRIVATE external/glm/src
    PRIVATE external/glad
    PRIVATE external/imgui
    PRIVATE external/stb
    PRIVATE external/FileBrowser
    PRIVATE src
)

IF(WIN32)
    target_link_libraries(${PROJECT_NAME} glfw glad imgui stb glm Engine assimp FileBrowser opengl32)
ELSEIF(UNIX)
    target_link_libraries(${PROJECT_NAME} glfw glad imgui stb glm Engine assimp FileBrowser)
ELSEIF(APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
    target_link_libraries(${PROJECT_NAME} "-framework GLUT")
    target_link_libraries(${PROJECT_NAME} glfw glad imgui stb glm Engine assimp FileBrowser)
ENDIF()