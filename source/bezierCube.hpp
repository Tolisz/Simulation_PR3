#pragma once

#include <array>
#include <unordered_map>
#include <glm/vec3.hpp>

class bezierCube
{
public:

	bezierCube(float a);
	~bezierCube() = default;

	bezierCube(const bezierCube&) = delete;
	bezierCube& operator=(const bezierCube&) = delete;
	bezierCube(bezierCube&&) = delete;
	bezierCube& operator=(bezierCube&&) = delete;

private:

	void ResetCube(float a);

private:
	std::vector<glm::vec3> m_points;

	// i -> j -> spring rest length, where j < i
	std::unordered_map<int, std::unordered_map<int, float>> m_cubeSprings;
};