#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class OpenGLWindow;

class Camera {
 public:
  void computeViewMatrix();
  void computeProjectionMatrix(int width, int height);

  void dolly(float speed);
  void pan(float speed);
  void tilt(float speed);
  void truck(float speed);

 private:
  friend OpenGLWindow;

  glm::vec3 m_eye{glm::vec3(0.0f, 0.0f, 3.0f)};
  glm::vec3 m_at{glm::vec3(0.0f, 0.5f, 0.0f)};
  glm::vec3 m_up{glm::vec3(0.0f, 1.0f, 0.0f)};

  glm::mat4 m_viewMatrix;
  glm::mat4 m_projMatrix;
};

#endif
