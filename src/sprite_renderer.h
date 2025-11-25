#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>
#include <string>
#include <vector>
#include <GLFW/glfw3.h>

#include "texture.h"
#include "shader.h"
#include "cylinder.h"
#include "camera.h"

class SpriteRenderer
{
public:
    // camera
    Camera camera;

    void MouseUpdate(float xpos, float ypos);
    void process_keyboard_input(bool keys[1024], float deltaTime);
    void MouseScroll(double yoffset);
    void adjustLightProperty(float adjustment);

    // Constructor (inits shaders/shapes)
    SpriteRenderer(std::map<std::string, Shader>& shaders, Texture2D diffuseMap, Texture2D specularMap, int width, int height);

    // Destructor
    ~SpriteRenderer();
    // Renders a defined quad textured with given sprite
    void DrawSprite(Texture2D texture, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
    void DrawLine(glm::vec2& start, glm::vec2& end, float lineWidth, glm::vec3& color);
    void DrawRectangle(glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color);

    void initLightingShader();

    glm::vec3 ScreenToWorldCoordinates(const glm::vec2& screenCoords, const glm::vec2& size);

    // Update projection and view matrices
    void UpdateProjection(float width, float height);
    void UpdateView();

    // Render 3D
    void DrawCube(glm::vec3 position);
    void initDiskShader();
    void initSideShader();
    void DrawCylinder(glm::mat4 cylinderModel, glm::vec3 color);
    void DrawCylinder2D(glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
    void DrawGround(float height);
    void DrawLightCube();
private:

    int SCR_WIDTH;
    int SCR_HEIGHT;

    // lighting
    glm::vec3 lightPos = { 1.2f, 1.0f, 2.0f };
    // 将原来的值改为更高的值
    glm::vec3 lightAmbient = { 0.5f, 0.5f, 0.5f };  // 从0.2增加到0.5
    glm::vec3 lightDiffuse = { 0.8f, 0.8f, 0.8f };  // 从0.5增加到0.8
    glm::vec3 lightSpecular = { 1.0f, 1.0f, 1.0f }; // 保持不变（已经是最大值）

    const float netSize = 20.0f;
    const float netStep = 1.0f; // 每格1米
    // 灯光控制
    float lightAdjustStep = 0.1f;
    int currentLightProperty = 0; // 0: ambient, 1: diffuse, 2: specular
    int rimVBOSize, topVBOSize, sideVBOSize;

    // Matrices
    glm::mat4 projection;
    glm::mat4 view;

    // Render state
    Shader       shader;
    Shader  rectShader;

    Shader lightingShader;
    Shader lightCubeShader;
    Shader groundShader;
    Shader lightMaterialShader;
    Shader diskShader;
    Shader sideShader;
    unsigned int quadVAO;
    // Initializes and configures the quad's buffer and vertex attributes
    unsigned int lineVAO, lineVBO;
    unsigned int cubeVBO, cubeVAO;
    unsigned int lightCubeVAO;
    unsigned int groundVAO, groundVBO;
    // 添加 rim 和 side 的 VAO 和 VBO
    unsigned int rimVAO, rimVBO_ID, sideVAO, sideVBO_ID;
    unsigned int topVAO, topVBO_ID;
    // load textures (we now use a utility function to keep the code more organized)
    // -----------------------------------------------------------------------------
    Texture2D diffuseMap;
    Texture2D specularMap;

    int groundVerticesSize;

    // positions of the point lights
    std::vector<glm::vec3> pointLightPositions;

    void initLineData();
    void initBoxData();
    void initGroundData();
    void initCylinderData();
    void initRenderData();
};

#endif