#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <string_view>

#include "abcg.hpp"
#include "model.hpp"
#include "camera.hpp"
#include <string>

class Earth {
  public:
    Model m_model;
    int m_trianglesToDraw{};
    glm::mat4 m_modelMatrix{1.0f};
};

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& ev) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  int m_viewportWidth{};
  int m_viewportHeight{};

  long long int numberFramers{1};

  float m_dollySpeed{0.0f};
  float m_panSpeed{0.0f};
  float m_tiltSpeed{0.0f};
  float m_truckSpeed{0.0f};
  float m_shininess{};
  float rotationSpeed{0.05f};

  Camera m_camera;

  Earth earth;

  GLuint m_program;

  glm::vec4 m_Ia{1.0f};
  glm::vec4 m_Id{1.0f};
  glm::vec4 m_Is{1.0f};
  glm::vec4 m_Ka{};
  glm::vec4 m_Kd{};
  glm::vec4 m_Ks{};
  glm::vec4 m_lightDir{-1.0f, -1.0f, -1.0f, 0.0f};

  void loadModel();
  void update();
};

#endif
