#include "cylinder.h"

// Helper: compute cross product of two 3D vectors (as float arrays)
void cross(const float a[3], const float b[3], float out[3]) {
    out[0] = a[1] * b[2] - a[2] * b[1];
    out[1] = a[2] * b[0] - a[0] * b[2];
    out[2] = a[0] * b[1] - a[1] * b[0];
}

// Helper: normalize a 3D vector
void normalize(float v[3]) {
    float len = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (len > 1e-8f) {
        v[0] /= len;
        v[1] /= len;
        v[2] /= len;
    }
}


/* using TRIANGLE_FAN and TRIANGLE_TRIPE mode */
void Cylinder::generateCylinder(int sector, std::vector<float>& rim, std::vector<float>& side) {
    double offset = 2.0 * my_pi / sector;

    rim.resize((sector + 2) * 3);
    // central point
    rim[0] = 0; rim[1] = 1.0f; rim[2] = 0;

    for (int i = 0; i <= sector; i++) {
        int j = i * 3 + 3;
        rim[j] = cosf(offset * i); rim[j + 1] = 1.0f; rim[j + 2] = sinf(offset * i);
    }

    side.resize((sector + 1) * 2 * 3);
    for (int i = 0; i <= sector; i++) {
        int j = i * 3 + 3;
        int k = i * 6;
        side[k] = rim[j]; side[k + 1] = 0.0f; side[k + 2] = rim[j + 2];
        side[k + 3] = rim[j]; side[k + 4] = 1.0f; side[k + 5] = rim[j + 2];
    }
}


void Cylinder::createCircleWireVertices(int sector, float z , std::vector<float>& target , bool vectical) {
    glm::vec3 vecticalVector(0, 0, (vectical ? 1 : -1));
    double offset = 2.0 * my_pi / sector;

    // 每个线段 3 个顶点 + 一个垂直 × 3 floats
    target.resize(sector * (2 + 2) * 3);

//    double angle1 = 0;
//    double angle2 = 0;
    for (int i = 0; i < sector; ++i) {
        double angle1 = offset * i;
        double angle2 = offset * (i + 1);
        //angle2 = offset + angle1;

        float x1 = static_cast<float>(cos(angle1));
        float y1 = static_cast<float>(sin(angle1));
        float x2 = static_cast<float>(cos(angle2));
        float y2 = static_cast<float>(sin(angle2));

        //std::cout << x1 << " " << y1 << " " << x2 << " " << y2 << "\n";

        int j = i * 12; // 2 vertices × 6

        // First point
        target[j + 0] = x1; target[j + 1] = y1; target[j + 2] = z;
        // Vectical1 vector
        target[j + 3] = vecticalVector.x; target[j + 4] = vecticalVector.y; target[j + 5] = vecticalVector.z;
        // Second point
        target[j + 6] = x2; target[j + 7] = y2; target[j + 8] = z;
        // Vectical2 vector
        target[j + 9] = vecticalVector.x; target[j + 10] = vecticalVector.y; target[j + 11] = vecticalVector.z;

//        angle1 += offset;
    }
}

void Cylinder::createCylinderSideFewSector(int sector,
    std::vector<float>& up,
    std::vector<float>& down,
    std::vector<float>& side) {
    // Each sector: 9 floats (A, B, vertical)
    if (up.size() != static_cast<size_t>(sector * 12) ||
        down.size() != static_cast<size_t>(sector * 12)) {
        return;
    }

    // Output: 6 vertices per sector (2 triangles)
    // Each vertex: position (3) + normal (3) = 6 floats
    side.resize(sector * 6 * 6); // = sector * 36

    for (int i = 0; i < sector; ++i) {
        int j = i * 12;          // input offset
        int k = i * 36;         // output offset

        // Top points
        float A_top[3] = { up[j + 0], up[j + 1], up[j + 2] };
        float B_top[3] = { up[j + 6], up[j + 7], up[j + 8] };

        // Bottom points
        float A_bot[3] = { down[j + 0], down[j + 1], down[j + 2] };
        float B_bot[3] = { down[j + 6], down[j + 7], down[j + 8] };

        // Edge vectors
        float head[3] = { B_top[0] - A_top[0], B_top[1] - A_top[1], B_top[2] - A_top[2] };
        float end[3] = { B_bot[0] - A_bot[0], B_bot[1] - A_bot[1], B_bot[2] - A_bot[2] };

        // Mid vector (connect top to bottom)
        float mid[3];
        if (i & 0x1) {
            // Odd sector: connect B_top → A_bot
            mid[0] = A_bot[0] - B_top[0];
            mid[1] = A_bot[1] - B_top[1];
            mid[2] = A_bot[2] - B_top[2];
        }
        else {
            // Even sector: connect A_top → B_bot
            mid[0] = B_bot[0] - A_top[0];
            mid[1] = B_bot[1] - A_top[1];
            mid[2] = B_bot[2] - A_top[2];
        }

        // Normal for triangle 1: cross(head, mid)
        float normal1[3];
        float normal2[3];

            normal1[0] = A_top[0] + B_top[0] + A_bot[0];
            normal1[1] = A_top[1] + B_top[1] + A_bot[1];
            normal1[2] = 0;
            normal2[0] = A_bot[0] + B_bot[0] + A_top[0];
            normal2[1] = A_bot[1] + B_bot[1] + A_top[1];
            normal2[2] = 0;

        normalize(normal1);
        normalize(normal2);

        //std::cout << normal1[0]<< " " << normal1[1] << " " << normal1[2] << "\n";
        //std::cout << normal2[0] << " " << normal2[1] << " " << normal2[2] << "\n";

        // Helper lambda to write vertex: pos + normal
        auto writeVertex = [&](int offset, const float pos[3], const float norm[3]) {
            side[offset + 0] = pos[0]; side[offset + 1] = pos[1]; side[offset + 2] = pos[2];
            side[offset + 3] = norm[0]; side[offset + 4] = norm[1]; side[offset + 5] = norm[2];
            };

        // Triangle 1: A_top, A_bot, B_top
        writeVertex(k + 0, A_top, normal1);
        writeVertex(k + 6, A_bot, normal1);
        writeVertex(k + 12, B_top, normal1);

        // Triangle 2: A_bot, B_bot, B_top
        writeVertex(k + 18, A_bot, normal2);
        writeVertex(k + 24, B_bot, normal2);
        writeVertex(k + 30, B_top, normal2);
    }
}

void Cylinder::createCylinderSideLargeSector(int sector,
    std::vector<float>& up,
    std::vector<float>& down,
    std::vector<float>& side) {
    // Each sector: 9 floats (A, B, vertical)
    if (up.size() != static_cast<size_t>(sector * 12) ||
        down.size() != static_cast<size_t>(sector * 12)) {
        return;
    }

    // Output: 6 vertices per sector (2 triangles)
    // Each vertex: position (3) + normal (3) = 6 floats
    side.resize(sector * 6 * 6); // = sector * 36

    for (int i = 0; i < sector; ++i) {
        int j = i * 12;          // input offset
        int k = i * 36;         // output offset

        // Top points
        float A_top[3] = { up[j + 0], up[j + 1], up[j + 2] };
        float B_top[3] = { up[j + 6], up[j + 7], up[j + 8] };

        // Bottom points
        float A_bot[3] = { down[j + 0], down[j + 1], down[j + 2] };
        float B_bot[3] = { down[j + 6], down[j + 7], down[j + 8] };

        // Normal for triangle 1: cross(head, mid)
        float normal1[3];
        normalize(normal1);

        float normal2[3];
        normalize(normal2);

        std::cout << normal1[0] << " " << normal1[1] << " " << normal1[2] << "\n";
        std::cout << normal2[0] << " " << normal2[1] << " " << normal2[2] << "\n";

        // Helper lambda to write vertex: pos + normal
        auto writeVertex = [&](int offset, const float pos[3], const float norm[3]) {
            side[offset + 0] = pos[0]; side[offset + 1] = pos[1]; side[offset + 2] = pos[2];
            side[offset + 3] = norm[0]; side[offset + 4] = norm[1]; side[offset + 5] = norm[2];
            };

        // Triangle 1: A_top, A_bot, B_top
        writeVertex(k + 0, A_top, normal1);
        writeVertex(k + 6, A_bot, normal1);
        writeVertex(k + 12, B_top, normal1);

        // Triangle 2: A_bot, B_bot, B_top
        writeVertex(k + 18, A_bot, normal2);
        writeVertex(k + 24, B_bot, normal2);
        writeVertex(k + 30, B_top, normal2);
    }
}