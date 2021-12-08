#include "openglwindow.hpp"

#include <imgui.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <fmt/core.h>
#include <string> 

void OpenGLWindow::handleEvent(SDL_Event& ev) {
  if (ev.type == SDL_KEYDOWN) {
    if (ev.key.keysym.sym == SDLK_UP || ev.key.keysym.sym == SDLK_w) {
      m_dollySpeed = 0.3f;
    }
    if (ev.key.keysym.sym == SDLK_DOWN || ev.key.keysym.sym == SDLK_s) {
      m_dollySpeed = -0.3f;
    }
    if (ev.key.keysym.sym == SDLK_LEFT || ev.key.keysym.sym == SDLK_a) {
      m_panSpeed = -0.3f;
    }
    if (ev.key.keysym.sym == SDLK_RIGHT || ev.key.keysym.sym == SDLK_d) {
      m_panSpeed = 0.3f;
    }
    if (ev.key.keysym.sym == SDLK_f) {
      m_tiltSpeed = -0.3f;
    }
    if (ev.key.keysym.sym == SDLK_v) {
      m_tiltSpeed = 0.3f;
    }
    if (ev.key.keysym.sym == SDLK_g) {
      m_truckSpeed = -0.3f;
    }
    if (ev.key.keysym.sym == SDLK_h) {
      m_truckSpeed = 0.3f;
    }
  }
  if (ev.type == SDL_KEYUP) {
    if ((ev.key.keysym.sym == SDLK_UP || ev.key.keysym.sym == SDLK_w) && m_dollySpeed > 0) {
      m_dollySpeed = 0.0f;
    }
    if ((ev.key.keysym.sym == SDLK_DOWN || ev.key.keysym.sym == SDLK_s) && m_dollySpeed < 0) {
      m_dollySpeed = 0.0f;
    }
    if ((ev.key.keysym.sym == SDLK_LEFT || ev.key.keysym.sym == SDLK_a) && m_panSpeed < 0) {
      m_panSpeed = 0.0f;
    }
    if ((ev.key.keysym.sym == SDLK_RIGHT || ev.key.keysym.sym == SDLK_d) && m_panSpeed > 0) {
      m_panSpeed = 0.0f;
    }
    if (ev.key.keysym.sym == SDLK_f) {
      m_tiltSpeed = 0.0f;
    }
    if (ev.key.keysym.sym == SDLK_v) {
      m_tiltSpeed = 0.0f;
    }
    if (ev.key.keysym.sym == SDLK_g) {
      m_truckSpeed = 0.0f;
    }
    if (ev.key.keysym.sym == SDLK_h) {
      m_truckSpeed = 0.0f;
    }
  }
}

void OpenGLWindow::initializeGL() {
  glClearColor(0, 0, 0, 1);
  glEnable(GL_DEPTH_TEST);

  auto path{getAssetsPath() + "shaders/texture"};
  auto program{createProgramFromFile(path + ".vert", path + ".frag")};
  m_program = program;

  loadModel();
}

void OpenGLWindow::loadModel() {
  earth.m_model.loadObj(getAssetsPath() + "earth.obj");
  earth.m_model.loadDiffuseTexture(getAssetsPath() + "maps/earth.jpg");
  earth.m_model.setupVAO(m_program);
  earth.m_trianglesToDraw = earth.m_model.getNumTriangles();

  m_Ka = earth.m_model.getKa();
  m_Kd = earth.m_model.getKd();
  m_Ks = earth.m_model.getKs();
  m_shininess = earth.m_model.getShininess();
}

void OpenGLWindow::paintGL() {
  update();

  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  abcg::glUseProgram(m_program);

  const GLint viewMatrixLoc{abcg::glGetUniformLocation(m_program, "viewMatrix")};
  const GLint projMatrixLoc{abcg::glGetUniformLocation(m_program, "projMatrix")};
  const GLint modelMatrixLoc{abcg::glGetUniformLocation(m_program, "modelMatrix")};
  const GLint normalMatrixLoc{abcg::glGetUniformLocation(m_program, "normalMatrix")};
  const GLint lightDirLoc{abcg::glGetUniformLocation(m_program, "lightDirWorldSpace")};
  const GLint shininessLoc{abcg::glGetUniformLocation(m_program, "shininess")};
  const GLint IaLoc{abcg::glGetUniformLocation(m_program, "Ia")};
  const GLint IdLoc{abcg::glGetUniformLocation(m_program, "Id")};
  const GLint IsLoc{abcg::glGetUniformLocation(m_program, "Is")};
  const GLint KaLoc{abcg::glGetUniformLocation(m_program, "Ka")};
  const GLint KdLoc{abcg::glGetUniformLocation(m_program, "Kd")};
  const GLint KsLoc{abcg::glGetUniformLocation(m_program, "Ks")};
  const GLint diffuseTexLoc{abcg::glGetUniformLocation(m_program, "diffuseTex")};
  const GLint normalTexLoc{abcg::glGetUniformLocation(m_program, "normalTex")};
  const GLint mappingModeLoc{abcg::glGetUniformLocation(m_program, "mappingMode")};

  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_camera.m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_camera.m_projMatrix[0][0]);
  abcg::glUniform1i(diffuseTexLoc, 0);
  abcg::glUniform1i(normalTexLoc, 1);
  abcg::glUniform1i(mappingModeLoc, 3);

  abcg::glUniform4fv(lightDirLoc, 1, &m_lightDir.x);
  abcg::glUniform4fv(IaLoc, 1, &m_Ia.x);
  abcg::glUniform4fv(IdLoc, 1, &m_Id.x);
  abcg::glUniform4fv(IsLoc, 1, &m_Is.x);

  earth.m_modelMatrix = glm::mat4(1.0);
  earth.m_modelMatrix = glm::translate(earth.m_modelMatrix, glm::vec3(0.0f, 0.5f, 0.0f));
  earth.m_modelMatrix = glm::rotate(earth.m_modelMatrix, glm::radians(-rotationSpeed * numberFramers), glm::vec3(0, 1, 0));
  earth.m_modelMatrix = glm::scale(earth.m_modelMatrix, glm::vec3(2.0f));

  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &earth.m_modelMatrix[0][0]);

  auto modelViewMatrix{glm::mat3(m_camera.m_viewMatrix * earth.m_modelMatrix)};
  glm::mat3 normalMatrix{glm::inverseTranspose(modelViewMatrix)};
  
  glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  earth.m_model.render(earth.m_trianglesToDraw);

  glUniform1f(shininessLoc, m_shininess);
  glUniform4fv(KaLoc, 1, &m_Ka.x);
  glUniform4fv(KdLoc, 1, &m_Kd.x);
  glUniform4fv(KsLoc, 1, &m_Ks.x);

  numberFramers++;

  glUseProgram(0);
}

void OpenGLWindow::paintUI() { abcg::OpenGLWindow::paintUI(); }

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  m_camera.computeProjectionMatrix(width, height);
}

void OpenGLWindow::terminateGL() {
  abcg::glDeleteProgram(m_program);
}

void OpenGLWindow::update() {
  float deltaTime{static_cast<float>(getDeltaTime())};

  m_camera.dolly(m_dollySpeed * deltaTime);
  m_camera.pan(m_panSpeed * deltaTime);
  m_camera.tilt(m_tiltSpeed * deltaTime);
  m_camera.truck(m_truckSpeed * deltaTime);
}
