add_library(FileBrowser ImFileDialog.cpp ImFileDialog.h)

target_compile_features(FileBrowser PUBLIC cxx_std_17)
target_link_libraries(FileBrowser PRIVATE imgui ${stb_LIBRARIES})
target_include_directories(FileBrowser PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../" "${stb_INCLUDE_DIRS}")
target_include_directories(FileBrowser PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")