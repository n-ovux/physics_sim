#version 430 core

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aTextureCoordinate;

out vec2 vTextureCoordinate;

void main()
{
    vTextureCoordinate = aTextureCoordinate;
    gl_Position = vec4(aPosition, 0.0, 1.0);
}
