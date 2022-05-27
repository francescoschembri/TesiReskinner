#include "Window.h"

GLFWwindow* CreateWindow() {
	GLFWwindow* window = glfwCreateWindow(SCREEN_INITIAL_WIDTH, SCREEN_INITIAL_HEIGHT, "Reskinner", NULL, NULL);
	if (window == NULL)
		return window;
	glfwMakeContextCurrent(window);

	//Enable vsync
	glfwSwapInterval(1);

	//Setup callbacks
	process_mouse_movement = &update_mouse_last_pos;
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_press_callback);
	glfwSetMouseButtonCallback(window, on_mouse_click_callback);
	glfwSetWindowPos(window, 0, 39);
	return window;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	StatusManager* status = (StatusManager*)glfwGetWindowUserPointer(window);
	status->width = float(width);
	status->height = float(height);
	status->projection = glm::perspective(glm::radians(FOV), (float)width / (float)height, NEAR_PLANE, FAR_PLANE);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	process_mouse_movement(window, xpos, ypos);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	StatusManager* status = (StatusManager*)glfwGetWindowUserPointer(window);
	status->camera.ProcessMouseScroll(yoffset);
	status->camera2.ProcessMouseScroll(yoffset);
}

void key_press_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_RELEASE || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		return;

	StatusManager* status = (StatusManager*)glfwGetWindowUserPointer(window);

	// move camera position
	bool cameraMove = false;
	if (glfwGetKey(window, CAMERA_UP_KEY) == GLFW_PRESS) {
		status->camera.ProcessKeyboard(UP, status->deltaTime);
		status->camera2.ProcessKeyboard(UP, status->deltaTime);
		cameraMove = true;
	}
	if (glfwGetKey(window, CAMERA_DOWN_KEY) == GLFW_PRESS) {
		status->camera.ProcessKeyboard(DOWN, status->deltaTime);
		status->camera2.ProcessKeyboard(DOWN, status->deltaTime);
		cameraMove = true;
	}
	if (glfwGetKey(window, CAMERA_LEFT_KEY) == GLFW_PRESS) {
		status->camera.ProcessKeyboard(LEFT, status->deltaTime);
		status->camera2.ProcessKeyboard(LEFT, status->deltaTime);
		cameraMove = true;
	}
	if (glfwGetKey(window, CAMERA_RIGHT_KEY) == GLFW_PRESS) {
		status->camera.ProcessKeyboard(RIGHT, status->deltaTime);
		status->camera2.ProcessKeyboard(RIGHT, status->deltaTime);
		cameraMove = true;
	}

	if (action != GLFW_PRESS || cameraMove)
		return;

	assert(action == GLFW_PRESS);

	// reset camera position
	if (key == RESET_CAMERA_KEY) {
		status->ResetCamera();
		return;
	}

	// enable/disable wireframe mode
	if (key == WIREFRAME_KEY) {
		status->wireframeEnabled = !status->wireframeEnabled;
		return;
	}

	// pause/unpause animation. If is a baked model there are no bones -> no animation -> always paused
	if (key == PAUSE_KEY)
	{
		status->Pause();
		if (status->pause)
			process_mouse_movement = &update_mouse_last_pos;
		return;
	}

	//selection mode
	if (key == SELECT_VERTEX_KEY) {
		status->selectionMode = Mode_Vertex;
		return;
	}
	if (key == SELECT_EDGE_KEY) {
		status->selectionMode = Mode_Edge;
		return;
	}
	if (key == SELECT_FACE_KEY) {
		status->selectionMode = Mode_Face;
		return;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE || !status->pause)
		return;
	//Undo change
	if (key == UNDO_CHANGE_KEY) {
		status->Undo();
		return;
	}
	//Redo change
	if (key == REDO_CHANGE_KEY) {
		status->Redo();
		return;
	}
}

void on_mouse_click_callback(GLFWwindow* window, int button, int action, int mods)
{
	StatusManager* status = (StatusManager*)glfwGetWindowUserPointer(window);

	if (!status->pause) {
		process_mouse_movement = &update_mouse_last_pos;
		return;
	}

	assert(status->bakedModel.has_value());

	if (action == GLFW_RELEASE)
	{
		process_mouse_movement = &update_mouse_last_pos;
		if (button == GLFW_MOUSE_BUTTON_LEFT && status->isChanging)
			status->EndChange();
		return;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		status->SetPivot();
		process_mouse_movement = &rotate;
		return;
	}

	if (action != GLFW_PRESS || button != GLFW_MOUSE_BUTTON_LEFT) return;

	// selection
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
		//check for multiple selection
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE) {
			status->selectedVertices.clear();
			status->selectedVerticesPointers.clear();
		}
		if (!status->info.hitPoint) return;

		if (status->selectionMode == Mode_Vertex)
		{
			status->SelectHoveredVertex();
		}
		else if (status->selectionMode == Mode_Edge)
		{
			status->SelectHoveredEdge();
		}
		else if (status->selectionMode == Mode_Face)
		{
			status->SelectHoveredFace();
		}
	}
	else if (status->selectedVertices.size() == 0 || !status->info.hitPoint) return;

	if(glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
		status->currentChangeType = ChangeOffset;
		status->StartChange();
		process_mouse_movement = &tweak;
	}
	else {
		status->currentChangeType = ChangeNormals;
		status->StartChange();
		process_mouse_movement = &tweak;
	}
}

void rotate(GLFWwindow* window, float xpos, float ypos) {
	StatusManager* status = (StatusManager*)glfwGetWindowUserPointer(window);
	float xoffset = xpos - status->mouseLastPos.x;
	float yoffset = status->mouseLastPos.y - ypos; // reversed since y-coordinates go from bottom to top
	status->camera.ProcessMouseMovement(xoffset, yoffset);
	status->camera2.ProcessMouseMovement(xoffset, yoffset);
}


void update_mouse_last_pos(GLFWwindow* window, float xpos, float ypos) {
	StatusManager* status = (StatusManager*)glfwGetWindowUserPointer(window);
	status->mouseLastPos.x = xpos;
	status->mouseLastPos.y = ypos;
	if(status->pause)
		status->info = status->Picking();
}

void tweak(GLFWwindow* window, float xpos, float ypos) {
	update_mouse_last_pos(window, xpos, ypos);
	StatusManager* status = (StatusManager*)glfwGetWindowUserPointer(window);
	status->TweakSelectedVertices();
}