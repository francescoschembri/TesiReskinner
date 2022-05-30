add_library(FileBrowser ImFileDialog.cpp ImFileDialog.h)

target_link_libraries(FileBrowser PRIVATE ${imgui_LIBRARIES} ${stb_LIBRARIES})
target_include_directories(FileBrowser PUBLIC ${imgui_INCLUDE_DIRS} ${stb_INCLUDE_DIRS})
target_include_directories(FileBrowser PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")