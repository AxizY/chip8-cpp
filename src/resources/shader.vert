#version 330 core
layout (location = 0) in vec3 vertex;

uniform mat4 perspective;
uniform vec2 position;

void main()
{
    gl_Position = perspective * vec4(vertex.x + position.x*20, vertex.y + position.y*20, 1.0, 1.0);
}