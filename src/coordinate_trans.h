#pragma once

#include <glm/glm.hpp>
#include <cmath>
#include <stdexcept>

class CoordinateTrans {
private:
    // 私有构造函数
    CoordinateTrans();
    
    // 单例实例
    static CoordinateTrans* instance;
    
    // 成员变量
    int width, height;

public:
    // 获取单例实例
    static CoordinateTrans* getInstance();
    
    // 设置窗口尺寸
    void setDimensions(int w, int h);
    
    // 原有方法
    glm::vec2 solveQuadratic(float a, float b, float c);
    glm::vec3 ScreenToWorldCoordinates(const glm::vec2& screenCoords, const glm::vec2& size);
    bool cylinderIsChosen(const glm::vec3& viewPos, const glm::vec3& viewDirect, const glm::vec2& targetPos, const glm::vec2& targetSize);
    glm::vec2 transCylinderProp(const glm::vec2& targetSize);
    
    // 防止拷贝构造和赋值
    CoordinateTrans(const CoordinateTrans&) = delete;
    CoordinateTrans& operator=(const CoordinateTrans&) = delete;
    
    // 析构函数
    ~CoordinateTrans();
};