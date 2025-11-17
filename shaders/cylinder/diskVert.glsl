#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float normalSign; // +1.0 for top (0,0,1), -1.0 for bottom (0,0,-1)

out vec3 FragPos;
out vec3 Normal;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;

    // 圆片法线：始终垂直于平面
    vec3 localNormal = vec3(0.0, normalSign, 0.0);

    // 变换法线（注意：平移不影响法线，但旋转/缩放需要 normalMatrix）
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalMatrix * localNormal;

    gl_Position = projection * view * worldPos;
}