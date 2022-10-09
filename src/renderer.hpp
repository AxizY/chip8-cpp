#ifndef RENDERER_HPP
#define RENDERER_HPP
#include <GLFW/glfw3.h>
#include <iostream>
#include "engine.hpp"
#include "shader.hpp"
#include <bitset>
#include <cmath>

uint8_t keys[16];

uint8_t keycodes[16] {
    GLFW_KEY_1,
    GLFW_KEY_2,
    GLFW_KEY_3,
    GLFW_KEY_4,
    GLFW_KEY_Q,
    GLFW_KEY_W,
    GLFW_KEY_E,
    GLFW_KEY_R,
    GLFW_KEY_A,
    GLFW_KEY_S,
    GLFW_KEY_D,
    GLFW_KEY_F,
    GLFW_KEY_Z,
    GLFW_KEY_X,
    GLFW_KEY_C,
    GLFW_KEY_V,
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void key(GLFWwindow* window, int key, int scancode, int action, int mods) {
    for (size_t i = 0; i < 16; i++)
    {
        if(key == keycodes[i]) {
            if(action == GLFW_PRESS) {
                keys[i] = 1;
            }
            if(action == GLFW_RELEASE) {
                keys[i] = 0;
            }
        }
    }
}

template <size_t width, size_t height>
class Screen {
    private:
        std::bitset<width * height> pixels;
    public:
        Screen() = default;
        void changePixel(size_t w, size_t h, bool v) {
            if(w <= 63 && h <= 31) {
                pixels.set(w+width*h, v);
            }
        }

        int getPixel(size_t w, size_t h) {
            if(w <= 63 && h <= 31) {
                return pixels[w+width*h];
            }
            return 0;
        }

        void clear() {
            for (size_t x = 0; x < 64; x++)
            {
                for (size_t y = 0; y < 32; y++)
                {
                    changePixel(x, y, 0);
                }
                
            }
            
        }
};

class Display {
    private:
        GLFWwindow* window;
        Screen<64, 32>* screen;
        Drawer drawer;
        Shader shader;
        VertexArray* va;
        unsigned int texture;
        double lastCycle = 0;
        double lastTimer = 0;
    public:
        bool shouldClose() {
            return glfwWindowShouldClose(window);
        }

        void createPlane() {
            float vertices[] = {
                 0.0f,  0.0f, 0.0f,
                20.0f,  0.0f, 0.0f,
                20.0f, 20.0f, 0.0f,
                 0.0f, 20.0f, 0.0f
            };
            unsigned int indices[] = {
                0, 1, 2,
                0, 2, 3
            };

            va = new VertexArray(vertices, sizeof(vertices), indices, sizeof(indices));
        }

        Display(Screen<64, 32>* pScreen) {
            screen = pScreen;
            glfwInit();

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

            window = glfwCreateWindow(1280, 640, "Chip-8", NULL, NULL);
            glfwMakeContextCurrent(window);

            gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
            glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
            glfwSetKeyCallback(window, key);
            glViewport(0, 0, 1280, 640);

            shader.vertex("src/resources/shader.vert");
            shader.fragment("src/resources/shader.frag");
            shader.program();

            createPlane();
        }

        void pollEvents() {
            glfwPollEvents();
        }

        void loop() {
            glClearColor(0.11f, 0.11f, 0.11f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            projectionMatrix(&shader, window);
            for(int h = 0; h < 32; h++) {
                for (int w = 0; w < 64; w++) {
                    if (screen->getPixel(w, h) == 1) {
                        shader.setVec2("position", (float)w, (float)h);
                        drawer.draw(va, &shader, 6);
                    }
                }
            }
            glfwSwapBuffers(window);
        }

        int end() {
            glfwTerminate();
            glfwPollEvents();
            return 0;
        }

        void clear() {
            screen->clear();
        }

        void changePixel(size_t w, size_t h, bool v) {
            screen->changePixel(w, h, v);
        }

        int getPixel(size_t w, size_t h) {
            return screen->getPixel(w, h);
        }

        bool checkCycle() {
            if(glfwGetTime() - lastCycle > (1.0f/500.0f)) { 
                return true;
                lastCycle = glfwGetTime();
            }
            return false;
        }

        bool checkTimer() {
            if(glfwGetTime() - lastTimer > (1.0f/60.0f)) {
                return true;
                lastTimer = glfwGetTime();
            }
            return false;
        }

        uint8_t getKey(size_t i) {
            return keys[i];
        }
};
#endif