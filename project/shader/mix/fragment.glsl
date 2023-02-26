#version 330 core
out vec4 FragColor;
in vec3 normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{
    FragColor = vec4(abs(normal),1.0)*texture(texture_diffuse1, TexCoords);
}