add_library(
	stb
	stb.cpp
	stb_image.h
	stb_image_resize.h
)

target_include_directories(stb PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../")
target_include_directories(stb PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")