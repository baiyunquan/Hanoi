#version 330 core
layout (location = 0) in vec4 vertex; // 包含 vec2 position 和 vec2 texCoords

uniform mat4 projection;
uniform mat4 model;

void main()
{
    gl_Position = projection * model * vec4(vertex.xy, -0.95, 1.0);
}