#pragma once

#include "blue/camera/Camera.hpp"

// A normalized vector is one which is often used just to denote pure directions
// without bothering about the magnitude (set to 1; hence their other, more common name unit vector)
// i.e. how far the vector pushes doesn't matter but in what direction does it point/push matters.

// FIXME: Quaternions may be faster than rotation matrices
class PerspectiveCamera : public Camera {

public:

	PerspectiveCamera(std::uint16_t viewport_width, std::uint16_t viewport_height)
		: Camera(viewport_width, viewport_height)
	{
		reset();
	}

	void reset();

	void look_at(const glm::vec3& target);

	glm::mat4 get_view() override;

	glm::mat4 get_projection() override;

	void set_pos(const glm::vec3&);

	void add_rotation(float yaw, float pitch);

	void set_near(float);

	void set_far(float);

	// moving

	void go_forward(float distance);

	void go_backward(float distance);

	void go_left(float distance);

	void go_right(float distance);

	void mouse_rotation(double xpos, double ypos);

	// getters

	float get_roll() const;

	float get_pitch() const;

	float get_yaw() const;

	float get_fov() const;

	glm::vec3 get_position() const;

	glm::vec3 get_front() const;

	glm::vec3 get_up() const;

	GLfloat get_last_x() const;

	GLfloat get_last_y() const;

private:

	// roll is always 0
	GLfloat _yaw = 0;
	GLfloat _pitch = 0;
	GLfloat _lastX = 0;
	GLfloat _lastY = 0;
	GLfloat _fov = 0;

	glm::vec3 _position = glm::vec3(0.0f, 0.0f, 10.0f);
	glm::vec3 _front = glm::vec3(0.0f, 0.0f, -1.0f);

	glm::vec3 _HELPER_CAMERA_TARGET = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 _HELPER_CAMERA_DIRECTION = glm::normalize(_position - _HELPER_CAMERA_TARGET);
	glm::vec3 _HELPER_UP = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 _CAMERA_RIGHT = glm::normalize(glm::cross(_HELPER_UP, _HELPER_CAMERA_DIRECTION));
	glm::vec3 _CAMERA_UP = glm::cross(_HELPER_CAMERA_DIRECTION, _CAMERA_RIGHT);

	float _near = 0.1f;
	float _far = 500.0f;

	float _aspect{}; // (view ratio)
};
