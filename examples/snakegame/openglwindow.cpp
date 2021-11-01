#include "openglwindow.hpp"

#include <fmt/core.h>

#include <imgui.h>

#include <cppitertools/itertools.hpp>

#include "abcg.hpp"

void OpenGLWindow::handleEvent(SDL_Event &event) {
    if (event.key.keysym.sym == SDLK_UP && direction != Direction::Down) {
        direction = Direction::Up;
    }
    if (event.key.keysym.sym == SDLK_RIGHT && direction != Direction::Left) {
        direction = Direction::Right;
    }
    if (event.key.keysym.sym == SDLK_DOWN && direction != Direction::Up) {
        direction = Direction::Down;
    }
    if (event.key.keysym.sym == SDLK_LEFT && direction != Direction::Right) {
        direction = Direction::Left;
    }
    if (event.key.keysym.sym == SDLK_SPACE && gameState == GameState::GameOver) {
        initialState();
    }
}

void OpenGLWindow::initializeGL() {
    const auto *vertexShader{R"gl(
        #version 410
        layout(location = 0) in vec2 inPosition;
        layout(location = 1) in vec4 inColor;
        uniform vec2 translation;
        uniform float scale;
        out vec4 fragColor;
        void main() {
        vec2 newPosition = inPosition * scale + translation;
        gl_Position = vec4(newPosition, 0, 1);
        fragColor = inColor;
        }
    )gl"};

    const auto *fragmentShader{R"gl(
        #version 410
        in vec4 fragColor;
        out vec4 outColor;
        void main() { outColor = fragColor; }
    )gl"};

    m_program = createProgramFromString(vertexShader, fragmentShader);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    auto seed{std::chrono::steady_clock::now().time_since_epoch().count()};
    m_randomEngine.seed(seed);

    initialState();
}

void OpenGLWindow::initialState() {
    gameState = GameState::Playing;
    snakeSize = 1;
    snakePosition.x = {12};
    snakePosition.y = {12};
    direction = Direction::None;

    createNewApple();
}

void OpenGLWindow::gameOver() {
    gameState = GameState::GameOver;
}

bool checkCollision(SnakePosition snakePosition, int snakeSize) {
    for (int i = 0; i < snakeSize - 1; i++) {
        for (int j = i + 1; j < snakeSize; j++) { 
            if (snakePosition.x[i] == snakePosition.x[j] && snakePosition.y[i] == snakePosition.y[j]) {
                return true;
            }
        }
    }

    return false;
}

void OpenGLWindow::updatePosition() {
    int increment = -1;

    if (foundAnApple() && gameState == GameState::Playing) {
        snakeSize++;

        snakePosition.x.push_back(applePosition.x);
        snakePosition.y.push_back(applePosition.y);

        createNewApple();
    }

    if (direction == Direction::Right || direction == Direction::Down)
        increment = 1;

    for (int i = snakeSize - 1; i > 0; i--) {
        snakePosition.y[i] = snakePosition.y[i - 1];
        snakePosition.x[i] = snakePosition.x[i - 1];
    }

    if (direction == Direction::Right || direction == Direction::Left) {
        snakePosition.y[0] = (snakePosition.y[0] + increment) % boardSize;
        if (snakePosition.y[0] == -1) {
            snakePosition.y[0] = boardSize - 1;
        }  
    } else if (direction == Direction::Down || direction == Direction::Up) {
        snakePosition.x[0] = (snakePosition.x[0] + increment) % boardSize;
        if (snakePosition.x[0] == -1) {
            snakePosition.x[0] = boardSize - 1;
        }    
    }

    if (checkCollision(snakePosition, snakeSize) && gameState == GameState::Playing) {
        gameOver();
    }
}

bool OpenGLWindow::hasAppleInPosition(int x, int y) {
    return x == applePosition.x && y == applePosition.y;
}

bool OpenGLWindow::foundAnApple() {
    return snakePosition.x[0] == applePosition.x && snakePosition.y[0] == applePosition.y;
}


void OpenGLWindow::createNewApple() {
    std::uniform_int_distribution<int> rd(0, boardSize - 1);
    ApplePosition tempApplePosition;
    tempApplePosition.x = rd(m_randomEngine);
    tempApplePosition.y = rd(m_randomEngine);

    if(hasSnakeInPosition(tempApplePosition.x, tempApplePosition.y)) {
        return createNewApple();
    }

    applePosition = tempApplePosition;
}

bool OpenGLWindow::hasSnakeInPosition(int x, int y) {
    for (int k = 0; k < snakeSize; k++) {
        if (snakePosition.x[k] == x && snakePosition.y[k] == y) {
            return true;  
        }
    }

    return false;
}

void OpenGLWindow::paintGL() {
    if (m_elapsedTimer.elapsed() < gameSpeed) {
        return;
    }

    m_elapsedTimer.restart();
    
    glClear(GL_COLOR_BUFFER_BIT);
        
    int sides = 4;

    if (gameState == GameState::Playing) {
        updatePosition();
    }

    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < boardSize; j++) {
            if (hasSnakeInPosition(i, j)) {
                paintPosition(glm::vec3 {0.0f, 1.0f, 0.0f});
            }
            else if (hasAppleInPosition(i, j) && gameState == GameState::Playing) {
                paintPosition(glm::vec3 {1.0f, 0.0f, 0.0f});
            } else {
                paintPosition(glm::vec3 {1.0f, 1.0f, 1.0f});
            }

            glViewport(0, 0, m_viewportWidth, m_viewportHeight);

            glUseProgram(m_program);

            glm::vec2 translation{-0.7f + j * 0.06f, 0.7f - i * 0.06f};
            GLint translationLocation{glGetUniformLocation(m_program, "translation")};
            glUniform2fv(translationLocation, 1, &translation.x);
        
            auto scale{0.05f};
            GLint scaleLocation{glGetUniformLocation(m_program, "scale")};
            glUniform1f(scaleLocation, scale);

            glBindVertexArray(m_vao);
            glDrawArrays(GL_TRIANGLE_FAN, 0, sides + 2);
            glBindVertexArray(0);   
            glUseProgram(0);
        }
    }
}

void OpenGLWindow::resizeGL(int width, int height) {
    m_viewportWidth = width;
    m_viewportHeight = height;

    glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::terminateGL() {
    glDeleteProgram(m_program);
    glDeleteBuffers(1, &m_vboPositions);
    glDeleteBuffers(1, &m_vboColors);
    glDeleteVertexArrays(1, &m_vao);
}

void OpenGLWindow::paintPosition(glm::vec3 color) {
    glDeleteBuffers(1, &m_vboPositions);
    glDeleteBuffers(1, &m_vboColors);
    glDeleteVertexArrays(1, &m_vao);

    std::vector<glm::vec2> positions(0);
    std::vector<glm::vec3> colors(0);

    positions.emplace_back(0, 0);
    colors.push_back(color);

    auto step{M_PI / 2};

    for (auto angle : iter::range(M_PI / 4, M_PI * 4, step)) {
        positions.emplace_back(std::cos(angle), std::sin(angle));
        colors.push_back(color);
    }

    positions.push_back(positions.at(1));
    colors.push_back(color);

    glGenBuffers(1, &m_vboPositions);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                positions.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &m_vboColors);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3),
                colors.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};
    GLint colorAttribute{glGetAttribLocation(m_program, "inColor")};

    glGenVertexArrays(1, &m_vao);

    glBindVertexArray(m_vao);

    glEnableVertexAttribArray(positionAttribute);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
    glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray(colorAttribute);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
    glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}
