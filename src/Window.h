#pragma once

#include "StatusManager.h"

#include <GLFW/glfw3.h>

constexpr int SCREEN_INITIAL_WIDTH = 1920;
constexpr int SCREEN_INITIAL_HEIGHT = 991;

// keys
//wireframe
constexpr int WIREFRAME_KEY = GLFW_KEY_W;
//camera:
constexpr int RESET_CAMERA_KEY = GLFW_KEY_R;
constexpr int CAMERA_UP_KEY = GLFW_KEY_UP;
constexpr int CAMERA_DOWN_KEY = GLFW_KEY_DOWN;
constexpr int CAMERA_LEFT_KEY = GLFW_KEY_LEFT;
constexpr int CAMERA_RIGHT_KEY = GLFW_KEY_RIGHT;
//animation control
constexpr int PAUSE_KEY = GLFW_KEY_SPACE;
//undo and redo
constexpr int UNDO_CHANGE_KEY = GLFW_KEY_Z;
constexpr int REDO_CHANGE_KEY = GLFW_KEY_Y;
//selection mode
constexpr int SELECT_VERTEX_KEY = GLFW_KEY_1;
constexpr int SELECT_EDGE_KEY = GLFW_KEY_2;
constexpr int SELECT_FACE_KEY = GLFW_KEY_3;

GLFWwindow* CreateWindow();

//callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_press_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void on_mouse_click_callback(GLFWwindow* window, int button, int action, int mods);

//mouse movement
inline void (*process_mouse_movement)(GLFWwindow*, float, float);
void rotate(GLFWwindow* window, float xpos, float ypos);
void update_mouse_last_pos(GLFWwindow* window, float xpos, float ypos);
void tweak(GLFWwindow* window, float xpos, float ypos);