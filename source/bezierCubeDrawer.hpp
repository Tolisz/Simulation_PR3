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
	void DrawSprings();

private:

	void UpdatePointsBuffer();
	void InitGL();
	void DeInitGL();

private:

	std::shared_ptr<bezierCube> m_cube;

	// OpenGL
	GLuint m_VAO_points;
	GLuint m_VAO_edges;
	GLuint m_PointsBuffer;
	GLuint m_ElementsBuffer;
	

};