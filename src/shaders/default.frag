#version 430 core

in vec2 vTextureCoordinate;

out vec4 FragColor;

uniform sampler2D screen;

void main()
{
    FragColor = texture(screen, vTextureCoordinate);
}
