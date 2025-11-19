#include "coordinate_trans.h"

// 初始化静态成员
CoordinateTrans* CoordinateTrans::instance = nullptr;

// 私有构造函数
CoordinateTrans::CoordinateTrans() : width(1600), height(1000) {} // 默认尺寸

// 获取单例实例
CoordinateTrans* CoordinateTrans::getInstance() {
    if (instance == nullptr) {
        instance = new CoordinateTrans();
    }
    return instance;
}

// 设置窗口尺寸
void CoordinateTrans::setDimensions(int w, int h) {
    width = w;
    height = h;
}

// 析构函数
CoordinateTrans::~CoordinateTrans() {}

glm::vec2 CoordinateTrans::solveQuadratic(float a, float b, float c) {
    if (a == 0.0f) {
        throw std::invalid_argument("Coefficient 'a' cannot be zero in a quadratic equation.");
    }

    float discriminant = b * b - 4.0f * a * c;

    if (discriminant < 0.0f) {
        throw std::domain_error("No real roots: discriminant is negative.");
    }

    float sqrtDisc = std::sqrt(discriminant);
    float root1 = (-b - sqrtDisc) / (2.0f * a);
    float root2 = (-b + sqrtDisc) / (2.0f * a);

    return glm::vec2(root1, root2);
}

glm::vec3 CoordinateTrans::ScreenToWorldCoordinates(const glm::vec2& screenCoords, const glm::vec2& size) {
    float worldX = ((screenCoords.x + size.x / 2) / width) * 4.0f;
    float worldY = (1.0f - (screenCoords.y + size.y) / height) * 4.0f;

    return glm::vec3(worldX, worldY, 0.0f);
}

bool CoordinateTrans::cylinderIsChosen(const glm::vec3& viewPos, const glm::vec3& viewDirect, const glm::vec2& targetPos, const glm::vec2& targetSize) {
    glm::vec3 targetBottomCenter = ScreenToWorldCoordinates(targetPos, targetSize);
    glm::vec2 cylinderProp = transCylinderProp(targetSize);

    float xoffset = viewPos.x - targetBottomCenter.x;
    float a = viewDirect.z * viewDirect.z + viewDirect.x * viewDirect.x;
    float b = (2 * viewPos.z * viewDirect.z) + (2 * viewDirect.x * xoffset);
    float c = (viewPos.z * viewPos.z) + (xoffset * xoffset) - (cylinderProp.x * cylinderProp.x);

    try {
        glm::vec2 result = solveQuadratic(a, b, c);
        float rim = targetBottomCenter.y + viewDirect.y * result.x;
        float top = targetBottomCenter.y + viewDirect.y * result.y;
        return rim >= targetBottomCenter.y || top <= targetBottomCenter.y + cylinderProp.y;
    } catch (...) {
        return false;
    }
}

glm::vec2 CoordinateTrans::transCylinderProp(const glm::vec2& targetSize) {
    float heightConvert = 4.0f / height;
    float widthConvert = 2.0f / width; // radius is half of size.x
    return glm::vec2(targetSize.x * widthConvert, targetSize.y * heightConvert); // radius and height
}