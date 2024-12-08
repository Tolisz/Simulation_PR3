#include "bezierCubeDrawer.hpp"

#include <iostream>

bezierCubeDrawer::bezierCubeDrawer(std::shared_ptr<bezierCube> cube): 
	m_cube{cube}
{
	m_pointsAttributes.assign(64, 0);

	InitGL();
}

bezierCubeDrawer::~bezierCubeDrawer()
{
	DeInitGL();
}

void bezierCubeDrawer::DrawCubePoints()
{
	glPointSize(4.0f);
	glBindVertexArray(m_VAO_points);
	glDrawArrays(GL_POINTS, 0, 64);
	glBindVertexArray(0);
	glPointSize(1.0f);
}

void bezierCubeDrawer::DrawShortSprings()
{
	glBindVertexArray(m_VAO_shortSprings);
	glDrawElements(GL_LINES, m_shortSpringsNum * 2, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);
}

void bezierCubeDrawer::DrawLongSprings()
{
	glBindVertexArray(m_VAO_longSprings);
	glDrawElements(GL_LINES, m_longSpringsNum * 2, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);
}

void bezierCubeDrawer::DrawControlFrame()
{
	glBindVertexArray(m_VAO_framePoints);
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);
}

int bezierCubeDrawer::GetChosenPoint()
{
	return m_cube->GetChosenPointIndex();
}

void bezierCubeDrawer::SetPointAttribute(int pointIndex, int attributeIndex, bool value)
{
	if (pointIndex < 0 || pointIndex >= static_cast<int>(m_pointsAttributes.size()))
	{
		throw std::out_of_range("Point index is out of range");
	}

	if (attributeIndex < 0 || attributeIndex >= static_cast<int>(sizeof(GLuint) * 8)) 
	{
		throw std::out_of_range("Attribute index is out of bounds");
	}

	GLint& attributes = m_pointsAttributes[pointIndex];

	if (value) {
		attributes |= (1 << attributeIndex); // Set the bit
	} else {
		attributes &= ~(1 << attributeIndex); // Clear the bit
	}
}

bool bezierCubeDrawer::IsControlFrameChoosen()
{
	return b_ControlFrameChoosen;
}

void bezierCubeDrawer::SetControlFrameChoosen(bool isChoosen)
{
	b_ControlFrameChoosen = isChoosen;
}

void bezierCubeDrawer::UpdateBuffers()
{
	std::vector<glm::vec3> cubePoints = m_cube->GetCubePoints();
	std::vector<glm::vec3> framePoints = m_cube->GetFramePoints();

	/* Points buffer update */
	glBindBuffer(GL_ARRAY_BUFFER, m_cubePointsBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 64 * sizeof(glm::vec3), cubePoints.data());

	/* Attributes buffer update */
	glBindBuffer(GL_ARRAY_BUFFER, m_cubePointsAtribBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 64 * sizeof(GLint), m_pointsAttributes.data());

	/* Frame Points buffer update */
	glBindBuffer(GL_ARRAY_BUFFER, m_framePointsBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * sizeof(glm::vec3), framePoints.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);	
}

void bezierCubeDrawer::InitGL()
{
	InitGL_MainCube();
	InitGL_ControlCube();
}

void bezierCubeDrawer::InitGL_MainCube()
{
	glGenVertexArrays(1, &m_VAO_points);
	glGenVertexArrays(1, &m_VAO_shortSprings);
	glGenVertexArrays(1, &m_VAO_longSprings);
	
	glGenBuffers(1, &m_cubePointsBuffer);
	glGenBuffers(1, &m_cubePointsAtribBuffer);

	glGenBuffers(1, &m_EBO_shortSprings);
	glGenBuffers(1, &m_EBO_longSprings);

	// Points
	glBindVertexArray(m_VAO_points);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_cubePointsBuffer);
	glBufferData(GL_ARRAY_BUFFER, 64 * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_cubePointsAtribBuffer);
	glBufferData(GL_ARRAY_BUFFER, 64 * sizeof(GLint), nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribIPointer(1, 1, GL_INT, 0, (void*)0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	std::vector<std::pair<unsigned int, unsigned int>> shortIndices;
	std::vector<std::pair<unsigned int, unsigned int>> longIndices;	
	GetCubeSpringIndices(shortIndices, longIndices);
	m_shortSpringsNum = shortIndices.size();
	m_longSpringsNum = longIndices.size();

	// Short springs
	glBindVertexArray(m_VAO_shortSprings);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_cubePointsBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO_shortSprings);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, shortIndices.size() * sizeof(std::pair<int, int>), shortIndices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	// Long springs
	glBindVertexArray(m_VAO_longSprings);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_cubePointsBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO_longSprings);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, longIndices.size() * sizeof(std::pair<int, int>), longIndices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void bezierCubeDrawer::InitGL_ControlCube()
{
	glGenVertexArrays(1, &m_VAO_framePoints);
	glGenBuffers(1, &m_framePointsBuffer);
	glGenBuffers(1, &m_EBO_frameSprings);

	auto indices = GetFrameSpringIndices();

	glBindVertexArray(m_VAO_framePoints);

	glBindBuffer(GL_ARRAY_BUFFER, m_framePointsBuffer);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO_frameSprings);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(std::pair<int, int>), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
}


void bezierCubeDrawer::DeInitGL()
{
	/* Main Cube */
	glDeleteVertexArrays(1, &m_VAO_points);
	glDeleteVertexArrays(1, &m_VAO_shortSprings);
	glDeleteVertexArrays(1, &m_VAO_longSprings);
	
	glDeleteBuffers(1, &m_cubePointsBuffer);
	glDeleteBuffers(1, &m_cubePointsAtribBuffer);

	glDeleteBuffers(1, &m_EBO_shortSprings);
	glDeleteBuffers(1, &m_EBO_longSprings);

	/* Control Frame */

}

void bezierCubeDrawer::GetCubeSpringIndices(
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

std::vector<std::pair<unsigned int, unsigned int>> bezierCubeDrawer::GetFrameSpringIndices()
{
	return {
		// front 
		{0, 1}, {0, 2}, {3, 1}, {3, 2},
		// back
		{4, 5}, {4, 6}, {7, 5}, {7, 6},
		// between
		{0, 4}, {1, 5}, {2, 6}, {3, 7},
	};
}