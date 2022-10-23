#ifndef ENGINE_HPP
#define ENGINE_HPP

#ifndef GLAD
#define GLAD
#include <glad/glad.h>
#endif

#include <math.h>
#include "shader.hpp"
#include <iostream>
#include <GLFW/glfw3.h>

class VertexArray {
    private:
        unsigned int VBO, VAO, EBO;
    public:
        VertexArray(float* vertices, unsigned int size, unsigned int* indices, unsigned int indicesSize) {
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);
            glGenVertexArrays(1, &VAO);

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
        }

        void bind() {
            glBindVertexArray(VAO);
        }
};

class Drawer {
    public:
        void draw(VertexArray* va, Shader* _shader, int count) {
            _shader->use();
            va->bind();
            glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
        }
};

void projectionMatrix(Shader* shader, GLFWwindow* window) {
    int width;
    int height;
    glfwGetFramebufferSize(window, &width, &height);
    glm::mat4 projection;
    projection = glm::ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);
    shader->setMat4("perspective", projection);
}
#endif