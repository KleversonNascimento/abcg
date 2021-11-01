#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <random>

#include "abcg.hpp"

class SnakePosition {
  public:
    std::vector<int> x;
    std::vector<int> y; 
};

class ApplePosition {
  public:
    int x;
    int y; 
};

enum class Direction { Up, Right, Down, Left, None };
enum class GameState { Playing, GameOver };

class OpenGLWindow : public abcg::OpenGLWindow {
    protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void terminateGL() override;
    void handleEvent(SDL_Event& event) override;

    private:
    void createNewApple();
    bool foundAnApple();
    void gameOver();
    bool hasAppleInPosition(int x, int y);
    bool hasSnakeInPosition(int x, int y);
    void initialState();
    void paintPosition(glm::vec3 color);
    void updatePosition();  

    GLuint m_vao{};
    GLuint m_vboPositions{};
    GLuint m_vboColors{};
    GLuint m_program{};

    Direction direction{Direction::None};
    SnakePosition snakePosition;
    ApplePosition applePosition;
    GameState gameState{GameState::Playing};

    std::default_random_engine m_randomEngine;
    abcg::ElapsedTimer m_elapsedTimer;

    float gameSpeed = 0.08f;

    int snakeSize{1};
    int boardSize = 24;
    int m_viewportWidth{};
    int m_viewportHeight{};
};

#endif
