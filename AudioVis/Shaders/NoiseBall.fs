#version 330 core

out vec4 FragColor;
in vec4 outColor;
in vec2 texCoord;
uniform sampler2D tex;

void main() 
{
    vec4 texcolor = texture(tex, texCoord);
    FragColor = texcolor*outColor;
}