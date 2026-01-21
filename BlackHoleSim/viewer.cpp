#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>

#include <vector>
#include <cstdint>
#include <iostream>

#include "global.h"
#include "controls.cpp"

std::vector<Light> lights;

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // Request OpenGL 3.2 Core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    GLFWwindow* window = glfwCreateWindow(5*WIDTH, 5*HEIGHT, "Black Hole Simulator", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    // Setup a fullscreen quad (two triangles)
    GLuint vao, vbo;
    float vertices[] = {
        // positions   // tex coords
        -1.f, -1.f,   0.f, 0.f,
         1.f, -1.f,   1.f, 0.f,
         1.f,  1.f,   1.f, 1.f,
        -1.f,  1.f,   0.f, 1.f
    };
    GLuint indices[] = { 0, 1, 2, 2, 3, 0 };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vboVertices, ebo;
    glGenBuffers(1, &vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // tex coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Simple shaders
    const char* vertexSrc = R"(
        #version 150 core
        in vec2 aPos;
        in vec2 aUV;
        out vec2 uv;
        void main() {
            gl_Position = vec4(aPos, 0.0, 1.0);
            uv = aUV;
        }
    )";

    const char* fragmentSrc = R"(
        #version 150 core
        in vec2 uv;
        out vec4 FragColor;
        uniform sampler2D tex;
        void main() {
            FragColor = texture(tex, uv);
        }
    )";

    auto compileShader = [](GLenum type, const char* src) -> GLuint {
        GLuint s = glCreateShader(type);
        glShaderSource(s, 1, &src, nullptr);
        glCompileShader(s);
        GLint success;
        glGetShaderiv(s, GL_COMPILE_STATUS, &success);
        if (!success) {
            char log[512];
            glGetShaderInfoLog(s, 512, nullptr, log);
            std::cerr << "Shader compilation error: " << log << "\n";
        }
        return s;
    };

    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glBindAttribLocation(shaderProgram, 0, "aPos");
    glBindAttribLocation(shaderProgram, 1, "aUV");
    glLinkProgram(shaderProgram);
    glDeleteShader(vs);
    glDeleteShader(fs);

    // Create texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Initial pixel upload

    lights.reserve(1000);
    lights.push_back({{0,0,0}, {0, 0, 0, 255}, 0});
    lights.push_back({{0,0,0}, {0, 20, 80, 255}, BOX});
    lights.push_back({{-5, 0, 0}, {255, 255, 0, 255}, 1});
    lights.push_back({{0, -5, 0}, {0, 255, 255, 255}, 1});
    lights.push_back({{0, 5, 0}, {255, 0, 255, 255}, 1});

    cursor.selected = &lights[1];
    cursor.enforce(lights);

    position.x = 10, position.y = 0, position.z = 0;
    buildScene(position, lights);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);

    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);


    // Force 15 FPS
    const double FRAME_TIME = 1.0 / 15.0;
    double lastFrameTime = glfwGetTime();


    // Main loop
    while (!glfwWindowShouldClose(window)) {

        double currentTime = glfwGetTime();
        double delta = currentTime - lastFrameTime;

        if (delta < FRAME_TIME) {
            // Sleep just enough to hit 15 FPS
            glfwWaitEventsTimeout(FRAME_TIME - delta);
            continue;
        }

        lastFrameTime = glfwGetTime();
        processArrowKeys(window, FRAME_TIME);

        buildScene(position, lights);

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);

        glfwWaitEvents();
        glfwPollEvents();

    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
