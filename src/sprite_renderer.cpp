#include "sprite_renderer.h"
#include <vector>
#include <GLFW/glfw3.h>
//#include <iostream>

void SpriteRenderer::MouseScroll(double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void SpriteRenderer::MouseUpdate(float xpos, float ypos)
{
    static float lastX = 0.0f;
    static float lastY = 0.0f;
    static bool firstMouse = true;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
    UpdateView();
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void SpriteRenderer::process_keyboard_input(bool keys[1024], float deltaTime)
{

    if (keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);

    // 选择要调整的灯光属性
    if (keys[GLFW_KEY_1]) {
        currentLightProperty = 0;
        std::cout << "Selected: Ambient Light" << std::endl;
    }
    if (keys[GLFW_KEY_2]) {
        currentLightProperty = 1;
        std::cout << "Selected: Diffuse Light" << std::endl;
    }
    if (keys[GLFW_KEY_3]) {
        currentLightProperty = 2;
        std::cout << "Selected: Specular Light" << std::endl;
    }

    // 调整选定的灯光属性
    if (keys[GLFW_KEY_EQUAL]) { // 加号键
        adjustLightProperty(lightAdjustStep);
    }
    if (keys[GLFW_KEY_MINUS]) { // 减号键
        adjustLightProperty(-lightAdjustStep);
    }

    // 增加调整步长
    if (keys[GLFW_KEY_LEFT_BRACKET]) {
        lightAdjustStep = std::max(0.01f, lightAdjustStep - 0.01f);
        std::cout << "Adjust step: " << lightAdjustStep << std::endl;
    }
    if (keys[GLFW_KEY_RIGHT_BRACKET]) {
        lightAdjustStep += 0.01f;
        std::cout << "Adjust step: " << lightAdjustStep << std::endl;
    }
}

void SpriteRenderer::adjustLightProperty(float adjustment)
{
    switch (currentLightProperty) {
    case 0: // Ambient
        lightAmbient += adjustment;
        lightAmbient = glm::clamp(lightAmbient, 0.0f, 1.0f);
        std::cout << "Ambient light: " << lightAmbient.r << ", "
            << lightAmbient.g << ", " << lightAmbient.b << std::endl;
        break;
    case 1: // Diffuse
        lightDiffuse += adjustment;
        lightDiffuse = glm::clamp(lightDiffuse, 0.0f, 1.0f);
        std::cout << "Diffuse light: " << lightDiffuse.r << ", "
            << lightDiffuse.g << ", " << lightDiffuse.b << std::endl;
        break;
    case 2: // Specular
        lightSpecular += adjustment;
        lightSpecular = glm::clamp(lightSpecular, 0.0f, 1.0f);
        std::cout << "Specular light: " << lightSpecular.r << ", "
            << lightSpecular.g << ", " << lightSpecular.b << std::endl;
        break;
    }
}

SpriteRenderer::SpriteRenderer(std::map<std::string, Shader>& shaders , 
    Texture2D diffuseMap , Texture2D specularMap , int width, int height) : pointLightPositions{
            glm::vec3(0.7f, 0.2f, 2.0f),
            glm::vec3(2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f, 2.0f, -12.0f),
            glm::vec3(0.0f, 0.0f, -3.0f),
} , camera() , diffuseMap(diffuseMap) , specularMap(specularMap)
{
    // 所有着色器都从传入的映射中获取
    this->shader = shaders["sprite"];
    this->rectShader = shaders["rectangle"];
    this->lightingShader = shaders["lighting"];
    this->lightCubeShader = shaders["lightCube"];
    this->groundShader = shaders["ground"];
    this->lightMaterialShader = shaders["lightMaterial"];
    this->diskShader = shaders["disk"];
    this->sideShader = shaders["cylinderSide"];

    this->initRenderData();
    this->initLineData();
    this->initBoxData();
    this->initGroundData();
    this->initCylinderData();

    // Initialize projection and view matrices
    UpdateProjection(width, height);
    UpdateView();
}

void SpriteRenderer::UpdateProjection(float width, float height)
{
    projection = glm::perspective(glm::radians(camera.Zoom), width / height, 0.1f, 100.0f);
}

void SpriteRenderer::UpdateView()
{
    view = camera.GetViewMatrix();
}

SpriteRenderer::~SpriteRenderer()
{
    glDeleteVertexArrays(1, &this->quadVAO);
    glDeleteVertexArrays(1, &this->lineVAO);
    glDeleteVertexArrays(1, &this->cubeVAO);
    glDeleteVertexArrays(1, &this->lightCubeVAO);
    glDeleteVertexArrays(1, &this->groundVAO);
    glDeleteVertexArrays(1, &this->rimVAO);
    glDeleteVertexArrays(1, &this->sideVAO);
    glDeleteVertexArrays(1, &this->topVAO);

    glDeleteBuffers(1, &this->lineVBO);
    glDeleteBuffers(1, &this->groundVBO);
    glDeleteBuffers(1, &this->rimVBO_ID);
    glDeleteBuffers(1, &this->sideVBO_ID);
    glDeleteBuffers(1, &this->topVBO_ID);
}

void SpriteRenderer::DrawSprite(Texture2D texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color)
{
    // prepare transformations
    this->shader.Use();
//    std::cout << this->shader.ID << std::endl;

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // move origin of rotation to center of quad
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // move origin back

    model = glm::scale(model, glm::vec3(size, 1.0f)); // last scale

    this->shader.SetMatrix4("model", model);

    // render textured quad
    this->shader.SetVector3f("spriteColor", color);

    glActiveTexture(GL_TEXTURE0);
    texture.Bind();

    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void SpriteRenderer::DrawRectangle(glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color)
{
    // prepare transformations
    this->rectShader.Use();
//    std::cout << this->rectShader.ID << std::endl;

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // move origin of rotation to center of quad
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // move origin back

    model = glm::scale(model, glm::vec3(size, 1.0f)); // last scale

    this->rectShader.SetMatrix4("model", model);

    // render textured quad
    this->rectShader.SetVector3f("rectColor", color);
    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void SpriteRenderer::DrawCube(glm::vec3 position)
{

    lightingShader.Use();
    lightingShader.SetVector3f("viewPos", camera.Position);
    lightingShader.SetFloat("material.shininess", 32.0f);

    // 设置光源属性
    // 定向光
    lightingShader.SetVector3f("dirLight.direction", -0.2f, -1.0f, -0.3f);
    lightingShader.SetVector3f("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.SetVector3f("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    lightingShader.SetVector3f("dirLight.specular", 0.5f, 0.5f, 0.5f);

    // 点光源
    for (unsigned int i = 0; i < pointLightPositions.size(); i++)
    {
        std::string index = std::to_string(i);
        lightingShader.SetVector3f(("pointLights[" + index + "].position").c_str(), pointLightPositions[i]);
        lightingShader.SetVector3f(("pointLights[" + index + "].ambient").c_str(), 0.05f, 0.05f, 0.05f);
        lightingShader.SetVector3f(("pointLights[" + index + "].diffuse").c_str(), 0.8f, 0.8f, 0.8f);
        lightingShader.SetVector3f(("pointLights[" + index + "].specular").c_str(), 1.0f, 1.0f, 1.0f);
        lightingShader.SetFloat(("pointLights[" + index + "].constant").c_str(), 1.0f);
        lightingShader.SetFloat(("pointLights[" + index + "].linear").c_str(), 0.09f);
        lightingShader.SetFloat(("pointLights[" + index + "].quadratic").c_str(), 0.032f);
    }

    // 聚光灯
    lightingShader.SetVector3f("spotLight.position", camera.Position);
    lightingShader.SetVector3f("spotLight.direction", camera.Front);
    lightingShader.SetVector3f("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    lightingShader.SetVector3f("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    lightingShader.SetVector3f("spotLight.specular", 1.0f, 1.0f, 1.0f);
    lightingShader.SetFloat("spotLight.constant", 1.0f);
    lightingShader.SetFloat("spotLight.linear", 0.09f);
    lightingShader.SetFloat("spotLight.quadratic", 0.032f);
    lightingShader.SetFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    lightingShader.SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

    // 设置视图和投影矩阵
    lightingShader.SetMatrix4("projection", projection);
    lightingShader.SetMatrix4("view", view);

    // 设置模型矩阵
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    lightingShader.SetMatrix4("model", model);

    // 计算法线矩阵
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
    lightingShader.SetMatrix3("normalMatrix", normalMatrix);

    // 绑定纹理
    glActiveTexture(GL_TEXTURE0);
    diffuseMap.Bind();
    glActiveTexture(GL_TEXTURE1);
    specularMap.Bind();

    // 绘制立方体
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void SpriteRenderer::DrawCylinder(glm::mat4 cylinderModel , glm::vec3 lightColor)
{
    glm::vec3 diffuseColor = lightColor * glm::vec3(1.0f);   // ← 原来是 0.5f
    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.4f); // ← 原来是 0.2f

    diskShader.Use();
    // 使用类成员变量中的光照参数，保持一致性
    diskShader.SetVector3f("light.ambient", diffuseColor);
    diskShader.SetVector3f("light.diffuse", ambientColor);
    diskShader.SetVector3f("light.specular", lightSpecular);

    // Material properties
    diskShader.SetVector3f("material.ambient", 0.8f, 0.8f, 0.8f);
    diskShader.SetVector3f("material.diffuse", 1.0f, 1.0f, 1.0f);
    diskShader.SetVector3f("material.specular", 0.8f, 0.8f, 0.8f);
    diskShader.SetFloat("material.shininess", 64.0f);
    diskShader.SetMatrix4("projection", projection);
    diskShader.SetMatrix4("view", view);

    // 绘制底部圆片
    diskShader.SetMatrix4("model", cylinderModel);
    diskShader.SetFloat("normalSign", -1.0f); // 底部圆片法线向下
    glBindVertexArray(rimVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, rimVBOSize / 3);

    // 绘制顶部圆片 - 使用正确的topVAO并设置法线向上
    diskShader.SetFloat("normalSign", 1.0f); // 顶部圆片法线向上
    glBindVertexArray(topVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, topVBOSize / 3);

    sideShader.Use();
    // 使用类成员变量中的光照参数，保持一致性
    sideShader.SetVector3f("light.ambient", diffuseColor);
    sideShader.SetVector3f("light.diffuse", ambientColor);
    sideShader.SetVector3f("light.specular", lightSpecular);

    // Material properties
    sideShader.SetVector3f("material.ambient", 0.8f, 0.8f, 0.8f);
    sideShader.SetVector3f("material.diffuse", 1.0f, 1.0f, 1.0f);
    sideShader.SetVector3f("material.specular", 0.8f, 0.8f, 0.8f);
    sideShader.SetFloat("material.shininess", 64.0f);

    sideShader.SetMatrix4("projection", projection);
    sideShader.SetMatrix4("view", view);
    sideShader.SetMatrix4("model", cylinderModel);
    glBindVertexArray(sideVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, sideVBOSize / 3);
}

void SpriteRenderer::DrawGround(float height)
{
    groundShader.Use();
    glBindVertexArray(groundVAO);
    groundShader.SetMatrix4("projection", projection);
    groundShader.SetMatrix4("view", view);
    groundShader.SetFloat("tileSize", netStep);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -height, 0.0f));
    groundShader.SetMatrix4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, groundVerticesSize / 3);
}

void SpriteRenderer::DrawLightCube()
{    
    // also draw the lamp object(s)
    lightCubeShader.Use();
    lightCubeShader.SetMatrix4("projection", projection);
    lightCubeShader.SetMatrix4("view", view);
    
    // we now draw as many light bulbs as we have point lights.
    glBindVertexArray(lightCubeVAO);
    for (unsigned int i = 0; i < pointLightPositions.size(); i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pointLightPositions[i]);
        model = glm::scale(model, glm::vec3(0.2f)); // 缩小光源立方体
        lightCubeShader.SetMatrix4("model", model);
        lightCubeShader.SetVector3f("lightColor", 1.0f, 1.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void SpriteRenderer::initRenderData()
{
    // configure VAO/VBO
    unsigned int cubeVBO;
    static float vertices[] = { 
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &cubeVBO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void SpriteRenderer::initLineData() {
    static float lineVertices[] = {
        0.0f, 0.0f,
        1.0f, 0.0f
    };

    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);

    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void SpriteRenderer::DrawLine(glm::vec2 start, glm::vec2 end, float lineWidth, glm::vec3 color) {
    this->rectShader.Use();
    glLineWidth(lineWidth);

    // 计算模型矩阵
    glm::vec2 direction = end - start;
    float length = glm::length(direction);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(start, 0.0f));
    model = glm::rotate(model, atan2(direction.y, direction.x), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(length, 1.0f, 1.0f));

    this->rectShader.SetMatrix4("model", model);
    this->rectShader.SetVector3f("rectColor", color);

    glBindVertexArray(lineVAO);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}

void SpriteRenderer::initBoxData() {
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };


    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // shader configuration
    // --------------------
    lightingShader.Use();
    lightingShader.SetInteger("material.diffuse", 0);
    lightingShader.SetInteger("material.specular", 1);
    lightingShader.SetFloat("material.shininess", 32.0f);
}

void SpriteRenderer::initGroundData()
{
    // 创建地面网格（例如 -10 到 +10，共 20x20 米）
    std::vector<float> groundVertices;
    for (float x = -netSize; x < netSize; x += netStep) {
        for (float z = -netSize; z < netSize; z += netStep) {
            // 每个格子用两个三角形绘制
            groundVertices.insert(groundVertices.end(), {
            x,     0.0f, z,
            x + netStep,0.0f, z,
            x + netStep,0.0f, z + netStep,

            x,     0.0f, z,
            x + netStep,0.0f, z + netStep,
            x,     0.0f, z + netStep
                });
        }
    }
    groundVerticesSize = groundVertices.size();
    // add ground
    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);
    glBindVertexArray(groundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, groundVertices.size() * sizeof(float), groundVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void SpriteRenderer::initCylinderData()
{
    std::vector<float> rimVBO{}, sideVBO{};
    Cylinder::generateCylinder(1000, rimVBO, sideVBO);

    glGenVertexArrays(1, &rimVAO);
    glGenBuffers(1, &rimVBO_ID);
    glGenVertexArrays(1, &sideVAO);
    glGenBuffers(1, &sideVBO_ID);
    glGenVertexArrays(1, &topVAO);
    glGenBuffers(1, &topVBO_ID);

    // 配置 rim 的 VAO 和 VBO
    glBindVertexArray(rimVAO);
    glBindBuffer(GL_ARRAY_BUFFER, rimVBO_ID);
    glBufferData(GL_ARRAY_BUFFER, rimVBO.size() * sizeof(float), rimVBO.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    rimVBOSize = rimVBO.size();

    // 配置 side 的 VAO 和 VBO
    glBindVertexArray(sideVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sideVBO_ID);
    glBufferData(GL_ARRAY_BUFFER, sideVBO.size() * sizeof(float), sideVBO.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    for (int i = 1; i < rimVBO.size(); i += 3) {
        if (i < rimVBO.size()) rimVBO[i] = 1.0f;
    }
    sideVBOSize = sideVBO.size();

    // 配置 top 的 VAO 和 VBO
    glBindVertexArray(topVAO);
    glBindBuffer(GL_ARRAY_BUFFER, topVBO_ID);
    glBufferData(GL_ARRAY_BUFFER, rimVBO.size() * sizeof(float), rimVBO.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    topVBOSize = rimVBO.size();

    glBindVertexArray(0); // 解绑 VAO
}