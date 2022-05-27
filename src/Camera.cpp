#include  "Camera.h"

// constructor with vectors
Camera::Camera(glm::vec3 position)
	:
	sPosition(position),
	position(position),
	yaw(YAW),
	pitch(PITCH),
	pivot(position),
	distance(4.0f)
{
	pivot.z = 0.0f;
	SetPivot(pivot);
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(position, position + Front(), Up());
}

void Camera::Interpolate(Camera& other, float step)
{
	distance += (other.distance - distance) * step;
	position += (other.position - position) * step;
	pitch += (other.pitch - pitch) * step;
	float yawAngle = other.yaw - yaw; 
	yawAngle = abs(yawAngle) > 180.0f ? yawAngle - 360.0f : yawAngle;
	yaw = fmod(yaw + yawAngle * step, 360.0f);
	if (step >= 1.0 - FLT_EPSILON)
		pivot = other.pivot;
}

void Camera::SetPivot(glm::vec3& p)
{
	pivot = p;
	position = pivot - Front() * distance;
}

glm::vec3 Camera::GetPivot() {	return pivot; }

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float velocity = MOVEMENT_SPEED * deltaTime;
	if (direction == UP)
		position += Up() * velocity;
	if (direction == DOWN)
		position -= Up() * velocity;
	if (direction == LEFT)
		position -= Right() * velocity;
	if (direction == RIGHT)
		position += Right() * velocity;
}

// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset)
{
	distance -= yoffset * ZOOM_SPEED;
	position += Front() * yoffset * ZOOM_SPEED;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset)
{
	pitch += yoffset * ROTATION_SPEED;
	yaw += xoffset * ROTATION_SPEED + 360.0f;

	pitch = std::clamp(pitch, -89.9f, 89.9f);
	yaw = fmod(yaw, 360.0f);

	position = pivot - Front() * distance;
}

void Camera::Reset()
{
	yaw = YAW;
	pitch = PITCH;
	distance = 4.0f;
	pivot = sPosition;
	pivot.z = 0.0f;
	SetPivot(pivot);
}

glm::vec3 Camera::Front() {
	glm::vec3 f;
	f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	f.y = sin(glm::radians(pitch));
	f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	return glm::normalize(f);
}

glm::vec3 Camera::Right() {
	return glm::normalize(glm::cross(Front(), worldUp));
}

glm::vec3 Camera::Up() {
	return glm::normalize(glm::cross(Right(), Front()));
}

