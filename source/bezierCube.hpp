#pragma once

#include <array>
#include <unordered_map>
#include <glm/vec3.hpp>

class bezierCube
{

	bezierCube(float a);
	~bezierCube() = default;

	bezierCube(const bezierCube&) = delete;
	bezierCube& operator=(const bezierCube&) = delete;
	bezierCube(bezierCube&&) = delete;
	bezierCube& operator=(bezierCube&&) = delete;

private:

	std::array<glm::vec3, 64> m_points;
	std::unordered_map<int, std::unordered_map<int, float>> m_cubeSprings;
};