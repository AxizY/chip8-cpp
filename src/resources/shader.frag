#version 330 core
out vec4 color;
in vec2 texCoord;

uniform sampler2D screenTex;

void main()
{    
    color = vec4(texture(screenTex, texCoord).rrr, 1.0);
}