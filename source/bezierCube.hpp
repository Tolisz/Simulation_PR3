#pragma once

#include <array>
#include <unordered_map>
#include <glm/vec3.hpp>

#include <mutex>

class bezierCube
{
public:

	bezierCube(float a);
	~bezierCube() = default;

	bezierCube(const bezierCube&) = delete;
	bezierCube& operator=(const bezierCube&) = delete;
	bezierCube(bezierCube&&) = delete;
	bezierCube& operator=(bezierCube&&) = delete;

	std::vector<glm::vec3> GetPoints();
	void SetPoints(std::vector<glm::vec3>& newPositions);

	const std::unordered_map<int, std::unordered_map<int, float>>& GetRestLengths();

	glm::vec3 GetChoosenPointPos();
	void SetChoosenPointPos(const glm::vec3& pos);
	int GetChosenPointIndex();
	void SetChosenPointIndex(int i);

	void ResetCube(float a);

private:

	std::mutex m_accessPoints;
	std::vector<glm::vec3> m_cubePoints;
	int m_chosenPoint = -1;

	// i -> j -> spring rest length, where j < i
	std::unordered_map<int, std::unordered_map<int, float>> m_springsRestLengths;
};