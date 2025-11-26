#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos; // 保持纹理坐标不变，仍然使用原始坐标
    
    // 创建一个缩放矩阵，将坐标从-1到1扩展到-20到20
    mat4 scaleMatrix = mat4(20.0);
    scaleMatrix[3][3] = 1.0; // 确保w分量不变
    
    // 应用缩放矩阵到位置
    vec4 scaledPos = scaleMatrix * vec4(aPos, 1.0);
    gl_Position = projection * view * scaledPos;
}