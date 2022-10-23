#version 330 core
out vec4 color;
in vec2 texCoord;

uniform sampler2D screenTex;

void main()
{    
    color = texture(screenTex, texCoord).rrrr;
}
