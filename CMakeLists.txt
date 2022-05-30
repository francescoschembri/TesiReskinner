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