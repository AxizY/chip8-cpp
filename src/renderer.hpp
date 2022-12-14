#ifndef RENDERER_HPP
#define RENDERER_HPP
#include "../include/imgui/imgui.h"
#include "../include/imgui/imgui_impl_glfw.h"
#include "../include/imgui/imgui_impl_opengl3.h"
#include "engine.hpp"
#include "shader.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <bitset>
#include <cmath>

uint8_t keys[16];

uint8_t keycodes[16] {
    GLFW_KEY_X,
    GLFW_KEY_1,
    GLFW_KEY_2,
    GLFW_KEY_3,
    GLFW_KEY_Q,
    GLFW_KEY_W,
    GLFW_KEY_E,
    GLFW_KEY_A,
    GLFW_KEY_S,
    GLFW_KEY_D,
    GLFW_KEY_Z,
    GLFW_KEY_C,
    GLFW_KEY_4,
    GLFW_KEY_R,
    GLFW_KEY_F,
    GLFW_KEY_V,
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void keyf(GLFWwindow* window, int key, int scancode, int action, int mods) {
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
    public:
        uint8_t pixels[width * height];

        Screen() = default;
        void changePixel(size_t w, size_t h, bool v) {
            if(w <= 63 && h <= 31) {
                pixels[w+h*width] = v*255;
            }
        }

        int getPixel(size_t w, size_t h) {
            if(w <= 63 && h <= 31) {
                return pixels[w+h*width];
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
        Screen<64, 32> screen;
        Drawer drawer;
        Shader shader;
        VertexArray* va;
        unsigned int texture;
        double lastCycle = 0;
        double lastTimer = 0;
        bool draw = false;

        // imgui setup
        void ImGUIinit() {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            ImGui::StyleColorsDark();
            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init("#version 330");
        }

        void ImGUInewFrame() {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }

        void ImGUIrender() {
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        void ImGUIFPS() {
            ImGui::Begin("test");
            ImGui::Text("hi");
            ImGui::End();
        }
        // imgui setup end

        void createPlane() {
            float vertices[] = {
              // positions         // tex coords
                -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
                1.0f,  -1.0f, 0.0f, 1.0f, 1.0f,
                1.0f,   1.0f, 0.0f, 1.0f, 0.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.0f,
            };
            unsigned int indices[] = {
                0, 1, 2,
                0, 2, 3
            };

            va = new VertexArray(vertices, sizeof(vertices), indices, sizeof(indices));

            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture); 

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // need this for it to work
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // need this for it to work bruh

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 32, 0, GL_RED, GL_UNSIGNED_BYTE, screen.pixels);

        }

    public:
        bool shouldClose() {
            return glfwWindowShouldClose(window);
        }

        Display() {
            screen.clear();

            glfwInit();

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

            window = glfwCreateWindow(1280, 640, "Chip-8", NULL, NULL);
            glfwMakeContextCurrent(window);

            gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
            glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
            glfwSetKeyCallback(window, keyf);
            glViewport(0, 0, 1280, 640);

            shader.vertex("src/resources/shader.vert");
            shader.fragment("src/resources/shader.frag");
            shader.program();

            createPlane();

            ImGUIinit();
        }

        uint8_t getKey(size_t i) {
            return keys[i];
        }

        void clear() {
            screen.clear();
        }

        void changePixel(size_t w, size_t h, bool v) {
            screen.changePixel(w, h, v);
        }

        int getPixel(size_t w, size_t h) {
            return screen.getPixel(w, h);
        }

        bool checkCycle() {
            if(glfwGetTime() - lastCycle > (1.0f/500.0f)) { 
                lastCycle = glfwGetTime();
                return true;
            }
            return false;
        }

        bool checkTimers() {
            if(glfwGetTime() - lastTimer > (1.0f/60.0f)) {
                lastTimer = glfwGetTime();
                return true;
            }
            return false;
        }

        void preCycle() {
            ImGUInewFrame();
        }

        void loop() {
            glClearColor(0.11f, 0.11f, 0.11f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            drawer.draw(va, &shader, 6);

            draw = true;
        }

        void updateTexture() {
            glBindTexture(GL_TEXTURE_2D, texture); 
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 64, 32, GL_RED, GL_UNSIGNED_BYTE, screen.pixels);
        }

        void postCycle() {
            ImGUIFPS();
            ImGUIrender();
            if(draw) {
                glfwSwapBuffers(window);
                draw = false;
            }
            glfwPollEvents();
        }

        int end() {
            glfwTerminate();
            glfwPollEvents();
            return 0;
        }
};
#endif
