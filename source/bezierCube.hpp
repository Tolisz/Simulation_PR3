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

	std::vector<glm::vec3> GetCubePoints();
	void SetCubePoints(std::vector<glm::vec3>& newPositions);
	std::vector<glm::vec3> GetFramePoints();
	void SetFramePoints(std::vector<glm::vec3>& newPositions);

	const std::unordered_map<int, std::unordered_map<int, float>>& GetCubeSprings();
	const std::unordered_map<int, int>& GetFrameSprings();

	glm::vec3 GetChoosenPointPos();
	void SetChoosenPointPos(const glm::vec3& pos);
	int GetChosenPointIndex();
	void SetChosenPointIndex(int i);

	void Reset(float a);

private:

	void ResetCube(float a);
	void ResetFrame(float a);

private:

	/* Cube Points */
	std::mutex m_accessCubePoints;
	std::vector<glm::vec3> m_cubePoints;
	int m_chosenPoint = -1;

	// i -> j -> spring rest length, where j < i
	std::unordered_map<int, std::unordered_map<int, float>> m_springsRestLengths;
	
	/* Frame Points */
	std::mutex m_accessFramePoints;
	std::vector<glm::vec3> m_framePoints;

	bool b_frameSpringsInit = false;
	// Frame spring -> Cube spring
	std::unordered_map<int, int> m_frameSprings;
};