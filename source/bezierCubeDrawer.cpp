#include "bezierCubeDrawer.hpp"

#include <iostream>

bezierCubeDrawer::bezierCubeDrawer(std::shared_ptr<bezierCube> cube): 
	m_cube{cube}
{
	InitGL();
}

bezierCubeDrawer::~bezierCubeDrawer()
{
	DeInitGL();
}

void bezierCubeDrawer::DrawPoints()
{
	UpdatePointsBuffer();

	glPointSize(2.0f);
	glBindVertexArray(m_VAO_points);
	glDrawArrays(GL_POINTS, 0, 64);
	glBindVertexArray(0);
	glPointSize(1.0f);
}

void bezierCubeDrawer::DrawShortSprings()
{
	UpdatePointsBuffer();

	glBindVertexArray(m_VAO_shortSprings);
	glDrawElements(GL_LINES, m_shortSpringsNum * 2, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);
}

void bezierCubeDrawer::DrawLongSprings()
{
	UpdatePointsBuffer();

	glBindVertexArray(m_VAO_longSprings);
	glDrawElements(GL_LINES, m_longSpringsNum * 2, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);
}

void bezierCubeDrawer::UpdatePointsBuffer()
{
	std::vector<glm::vec3> points = m_cube->GetPoints();

	glBindBuffer(GL_ARRAY_BUFFER, m_PointsBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 64 * sizeof(glm::vec3), points.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void bezierCubeDrawer::InitGL()
{
	glGenVertexArrays(1, &m_VAO_points);
	glGenVertexArrays(1, &m_VAO_shortSprings);
	glGenVertexArrays(1, &m_VAO_longSprings);
	
	glGenBuffers(1, &m_PointsBuffer);
	glGenBuffers(1, &m_EBO_shortSprings);
	glGenBuffers(1, &m_EBO_longSprings);

	// Points
	glBindVertexArray(m_VAO_points);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_PointsBuffer);
	glBufferData(GL_ARRAY_BUFFER, 64 * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	std::vector<std::pair<unsigned int, unsigned int>> shortIndices;
	std::vector<std::pair<unsigned int, unsigned int>> longIndices;	
	GetSpringIndices(shortIndices, longIndices);
	m_shortSpringsNum = shortIndices.size();
	m_longSpringsNum = longIndices.size();

	// Short springs
	glBindVertexArray(m_VAO_shortSprings);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_PointsBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO_shortSprings);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, shortIndices.size() * sizeof(std::pair<int, int>), shortIndices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	// Long springs
	glBindVertexArray(m_VAO_longSprings);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_PointsBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VAO_longSprings);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, longIndices.size() * sizeof(std::pair<int, int>), longIndices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void bezierCubeDrawer::DeInitGL()
{
	glDeleteVertexArrays(1, &m_VAO_points);
	glDeleteBuffers(1, &m_PointsBuffer);
}

void bezierCubeDrawer::GetSpringIndices(
	std::vector<std::pair<unsigned int, unsigned int>>& shortSprings, 
	std::vector<std::pair<unsigned int, unsigned int>>& longSprings)
{
	for (int j = 0; j < 4; ++j) {
		for (int i = 0; i < 16; ++i)
		{
			int I = i + 16 * j;
			// To points on the same layer
			// ----------------------------

			if (i % 4 != 0) {
				// left 
				shortSprings.push_back(std::make_pair(I, I - 1));

				// diagonal
				if (i - 4 >= 0) {
					longSprings.push_back(std::make_pair(I - 1, I - 4));
				}

				// left -> up
				if (i - 5 >= 0) {
					longSprings.push_back(std::make_pair(I, I - 5));
				}
			}

			// up
			if (i - 4 >= 0) {
				shortSprings.push_back(std::make_pair(I, I - 4));
			}

			// To points on the previous layer 
			// -------------------------------

			if (I - 16 >= 0)
			{
				// front 
				shortSprings.push_back(std::make_pair(I, I - 16));

				// front -> left
				if (i % 4 != 0) {
					longSprings.push_back(std::make_pair(I, I - 16 - 1));
				}

				// front -> right
				if (i % 4 != 3) {
					longSprings.push_back(std::make_pair(I, I - 16 + 1));
				}

				// front -> up
				if (i - 4 >= 0) {
					longSprings.push_back(std::make_pair(I, I - 16 - 4));
				}

				// front -> down
				if (i + 4 < 16) {
					longSprings.push_back(std::make_pair(I, I - 16 + 4));
				}
			}

		}	
	}
}