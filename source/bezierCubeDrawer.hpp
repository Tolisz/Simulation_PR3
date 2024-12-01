#pragma once

#include <glad/glad.h>
#include <memory>
#include "bezierCube.hpp"

class bezierCubeDrawer
{
public:

	bezierCubeDrawer(std::shared_ptr<bezierCube> cube);
	~bezierCubeDrawer();

	bezierCubeDrawer(const bezierCubeDrawer&) = delete;
	bezierCubeDrawer(bezierCubeDrawer&&) = delete;
	bezierCubeDrawer& operator=(const bezierCubeDrawer&) = delete;
	bezierCubeDrawer& operator=(bezierCubeDrawer&&) = delete;

	void DrawPoints();
	void DrawShortSprings();
	void DrawLongSprings();

private:

	void UpdatePointsBuffer();

	void InitGL();
	void DeInitGL();
	void GetSpringIndices(
		std::vector<std::pair<unsigned int, unsigned int>>& shortSprings, 
		std::vector<std::pair<unsigned int, unsigned int>>& longSprings);

private:

	std::shared_ptr<bezierCube> m_cube;

	// Indices buffers
	int m_shortSpringsNum;
	int m_longSpringsNum;
	// std::vector<std::pair<int, int>> m_shortSpringsIndices;
	// std::vector<std::pair<int, int>> m_longSpringsIndices;

	// OpenGL
	GLuint m_VAO_points;
	GLuint m_VAO_shortSprings;
	GLuint m_VAO_longSprings;
	GLuint m_PointsBuffer;
	GLuint m_EBO_shortSprings;
	GLuint m_EBO_longSprings;


};