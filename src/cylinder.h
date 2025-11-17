#pragma once

#include <vector>
#include <cmath>
#include <iostream>
#include <glm/glm.hpp>

namespace Cylinder {
	constexpr double my_pi = 3.14159265358979323846;
	void generateCylinder(int sector, std::vector<float>& rim, std::vector<float>& side);

	[[deprecated]]
	void createCircleWireVertices(int sector, float z, std::vector<float>& target, bool vectical);
	[[deprecated]]
	void createCylinderSideFewSector(int sector, std::vector<float>& up, std::vector<float>& down, std::vector<float>& side);
	[[deprecated]]
	void createCylinderSideLargeSector(int sector, std::vector<float>& up, std::vector<float>& down, std::vector<float>& side);
}
