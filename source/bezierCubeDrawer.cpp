#include "bezierCubeDrawer.hpp"

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

	glPointSize(6.0f);
	glBindVertexArray(m_VAO_points);
	glDrawArrays(GL_POINTS, 0, 64);
	glBindVertexArray(0);
}

void bezierCubeDrawer::DrawSprings()
{

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
	glGenBuffers(1, &m_PointsBuffer);

	// Points
	glBindVertexArray(m_VAO_points);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_PointsBuffer);
	glBufferData(GL_ARRAY_BUFFER, 64 * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void bezierCubeDrawer::DeInitGL()
{
	glDeleteVertexArrays(1, &m_VAO_points);
	glDeleteBuffers(1, &m_PointsBuffer);
}
