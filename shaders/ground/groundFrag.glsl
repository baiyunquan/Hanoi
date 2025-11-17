#version 330 core
in vec3 worldPos;
out vec4 FragColor;

uniform float tileSize; // 每个格子的大小，比如 1.0

void main()
{
    // 计算当前片段所在的格子坐标
    int x = int(floor(worldPos.x / tileSize));
    int z = int(floor(worldPos.z / tileSize));

    // 黑白相间：(x + z) % 2 == 0 为白色，否则为黑色
    if ((x + z) % 2 == 0)
        FragColor = vec4(1.0, 1.0, 1.0, 1.0); // 白
    else
        FragColor = vec4(0.0, 0.0, 0.0, 1.0); // 黑
}