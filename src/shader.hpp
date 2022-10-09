#ifndef SHADER_H
#define SHADER_H
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// some code taken from learnopengl, but some also modified by me

class Shader {
    private:
        unsigned int vertexShader;
        unsigned int fragmentShader;
        unsigned int shaderProgram;
        const char* vSource;
        const char* fSource;
    public:
        void vertex(const char* path) {
            vertexShader = glCreateShader(GL_VERTEX_SHADER);
            std::ifstream vfile;
            std::string vcode;
            vfile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
            try {
                vfile.open(path);
                std::stringstream vstream;
                vstream << vfile.rdbuf();
                vfile.close();
                vcode = vstream.str();
            }
            catch(std::ifstream::failure e)
            {
                std::cout << "Vertex Bad Read" << std::endl;
            }
            vSource = vcode.c_str();
            glShaderSource(vertexShader, 1, &vSource, NULL);
            glCompileShader(vertexShader);
            int success;
            char infoLog[512];
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
                std::cout << "Vertex Error: \n" << infoLog << std::endl;
            };
        };
        void fragment(const char* path) {
            fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            std::ifstream ffile;
            std::string fcode;
            ffile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
            try {
                ffile.open(path);
                std::stringstream fstream;
                fstream << ffile.rdbuf();
                ffile.close();
                fcode = fstream.str();
            }
            catch(std::ifstream::failure e)
            {
                std::cout << "Fragment Bad Read" << std::endl;
            }
            fSource = fcode.c_str();
            glShaderSource(fragmentShader, 1, &fSource, NULL);
            glCompileShader(fragmentShader);
            int success;
            char infoLog[512];
            glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
                std::cout << "Fragment Error: \n" << infoLog << std::endl;
            };
        };
        void program() {
            shaderProgram = glCreateProgram();
            glAttachShader(shaderProgram, vertexShader);
            glAttachShader(shaderProgram, fragmentShader);
            glLinkProgram(shaderProgram);
            int success;
            char infoLog[512];
            glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
            if(!success)
            {
                glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
                std::cout << "Shader Program Error: \n" << infoLog << std::endl;
            }
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
        };
        void use() {
            glUseProgram(shaderProgram);
        };
        void setBool(const std::string &name, bool value) const
        {         
            glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), (int)value); 
        }
        void setInt(const std::string &name, int value) const
        { 
            glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value); 
        }
        void setFloat(const std::string &name, float value) const
        { 
            glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value); 
        }
        void setMat4(const std::string &name, glm::mat4 value) const {
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
        }
        void setVec4(const std::string &name, float vec[4]) {
            glUniform4f(glGetUniformLocation(shaderProgram, name.c_str()), vec[0], vec[1], vec[2], vec[3]);
        }
        void setVec2(const std::string &name, float a, float b) {
            glUniform2f(glGetUniformLocation(shaderProgram, name.c_str()), a, b);
        }
};
#endif