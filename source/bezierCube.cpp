#include "bezierCube.hpp"

#include <glm/exponential.hpp>

#include <iostream>

bezierCube::bezierCube(float a)
{
	m_cubePoints.resize(64);

	ResetCube(a);
}

std::vector<glm::vec3> bezierCube::GetPoints()
{
	std::vector<glm::vec3> pointsCopy;
	
	{
		std::lock_guard<std::mutex> lock(m_accessPoints);
		pointsCopy = m_cubePoints;
	}
	
	return std::move(pointsCopy);
}

void bezierCube::SetPoints(std::vector<glm::vec3>& newPositions)
{
	std::lock_guard<std::mutex> lock(m_accessPoints);
	m_cubePoints = newPositions;
}

const std::unordered_map<int, std::unordered_map<int, float>>& 
	bezierCube::GetRestLengths()
{
	return m_springsRestLengths;	
}

glm::vec3 bezierCube::GetChoosenPointPos()
{
	if (m_chosenPoint != -1) 
	{
		std::lock_guard<std::mutex> lock(m_accessPoints);
		return m_cubePoints[m_chosenPoint];
	}
	else 
	{
		return glm::vec3(std::numeric_limits<float>::max());
	}
}

void bezierCube::SetChoosenPointPos(const glm::vec3& pos)
{
	if (m_chosenPoint != -1) 
	{
		std::lock_guard<std::mutex> lock(m_accessPoints);
		m_cubePoints[m_chosenPoint] = pos;
	}
}

int bezierCube::GetChosenPointIndex()
{
	return m_chosenPoint;
}

void bezierCube::SetChosenPointIndex(int i)
{
	m_chosenPoint = i;
}

void bezierCube::ResetCube(float a)
{
	// Points positions
	// ================
	float a_2 = a / 2.0f;
	float a_3 = a / 3.0f;

	int n = 0;
	for (int i = 0; i < 4; ++i) {
		// Points for a single layer
		// -------------------------
		for (int j = 0; j < 4; ++j) {
			for (int k = 0; k < 4; ++k) {
				m_cubePoints[n] = glm::vec3(
					- a_2 + k * a_3, 
					+ a_2 - j * a_3, 
					+ a_2 - i * a_3);

				++n; // 16 * i + 4 * j + k
			}
		}
	}

	// Springs rest length
	// ====================
	float da = glm::sqrt(2.0f) * a_3;
	
	for (int j = 0; j < 4; ++j) {
		for (int i = 0; i < 16; ++i)
		{
			int I = i + 16 * j;
			// To points on the same layer
			// ----------------------------

			if (i % 4 != 0) {
				// left 
				m_springsRestLengths[I][I - 1] = a_3;

				// diagonal
				if (i - 4 >= 0) {
					m_springsRestLengths[I - 1][I - 4] = da;
				}

				// left -> up
				if (i - 5 >= 0) {
					m_springsRestLengths[I][I - 5] = da;
				}
			}

			// up
			if (i - 4 >= 0) {
				m_springsRestLengths[I][I - 4] = a_3;
			}


			// To points on the previous layer 
			// -------------------------------

			if (I - 16 >= 0)
			{
				// front 
				m_springsRestLengths[I][I - 16] = a_3;

				// front -> left
				if (i % 4 != 0) {
					m_springsRestLengths[I][I - 16 - 1] = da;
				}

				// front -> right
				if (i % 4 != 3) {
					m_springsRestLengths[I][I - 16 + 1] = da;
				}

				// front -> up
				if (i - 4 >= 0) {
					m_springsRestLengths[I][I - 16 - 4] = da;
				}

				// front -> down
				if (i + 4 < 16) {
					m_springsRestLengths[I][I - 16 + 4] = da;
				}
			}

		}	
	}
}