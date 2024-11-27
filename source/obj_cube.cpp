#include "obj_cube.hpp"

#include <iostream>

/* static */ const glm::vec3 obj_cube::c_vertexNormals[] = {
	// Bottom 
	{-1.0f, -1.0f, +1.0f}, {+0.0f, -1.0f, +0.0f},	// 0
	{-1.0f, -1.0f, -1.0f}, {+0.0f, -1.0f, +0.0f},	// 1
	{+1.0f, -1.0f, -1.0f}, {+0.0f, -1.0f, +0.0f},	// 2
	{+1.0f, -1.0f, +1.0f}, {+0.0f, -1.0f, +0.0f},	// 3

	// Top
	{+1.0f, +1.0f, +1.0f}, {+0.0f, +1.0f, +0.0f},	// 4
	{+1.0f, +1.0f, -1.0f}, {+0.0f, +1.0f, +0.0f},	// 5
	{-1.0f, +1.0f, -1.0f}, {+0.0f, +1.0f, +0.0f},	// 6
	{-1.0f, +1.0f, +1.0f}, {+0.0f, +1.0f, +0.0f},	// 7

	// Left 
	{-1.0f, +1.0f, +1.0f}, {-1.0f, +0.0f, +0.0f},	// 8
	{-1.0f, +1.0f, -1.0f}, {-1.0f, +0.0f, +0.0f},	// 9
	{-1.0f, -1.0f, -1.0f}, {-1.0f, +0.0f, +0.0f},	// 10
	{-1.0f, -1.0f, +1.0f}, {-1.0f, +0.0f, +0.0f},	// 11
	
	// Right
	{+1.0f, -1.0f, +1.0f}, {+1.0f, +0.0f, +0.0f},	// 12
	{+1.0f, -1.0f, -1.0f}, {+1.0f, +0.0f, +0.0f},	// 13
	{+1.0f, +1.0f, -1.0f}, {+1.0f, +0.0f, +0.0f},	// 14
	{+1.0f, +1.0f, +1.0f}, {+1.0f, +0.0f, +0.0f},	// 15

	// Back
	{+1.0f, +1.0f, -1.0f}, {+0.0f, +0.0f, -1.0f},	// 16
	{+1.0f, -1.0f, -1.0f}, {+0.0f, +0.0f, -1.0f},	// 17
	{-1.0f, -1.0f, -1.0f}, {+0.0f, +0.0f, -1.0f},	// 18
	{-1.0f, +1.0f, -1.0f}, {+0.0f, +0.0f, -1.0f},	// 19

	// Front
	{-1.0f, +1.0f, +1.0f}, {+0.0f, +0.0f, +1.0f},	// 20
	{-1.0f, -1.0f, +1.0f}, {+0.0f, +0.0f, +1.0f},	// 21
	{+1.0f, -1.0f, +1.0f}, {+0.0f, +0.0f, +1.0f},	// 22
	{+1.0f, +1.0f, +1.0f}, {+0.0f, +0.0f, +1.0f},	// 23
};

/* static */ const glm::uvec3	obj_cube::c_elements[] = {
	// Bottom
	{0, 1, 2}, {2, 3, 0},
	
	// Top
	{4, 5, 6}, {6, 7, 4},

	// Left 
	{8, 9, 10}, {10, 11, 8},
	
	// Right
	{12, 13, 14}, {14, 15, 12},
	
	// Back
	{16, 17, 18}, {18, 19, 16},
	
	// Front
	{20, 21, 22}, {22, 23, 20},
};

obj_cube::obj_cube()
{
	InitGL();
}

obj_cube::~obj_cube()
{
	DeInitGL();
}

void obj_cube::Draw()
{
	glBindVertexArray(m_vertexArray);
	glDrawElements(GL_TRIANGLES, 3 * sizeof(c_elements) / sizeof(glm::uvec3), GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);
}

void obj_cube::InitGL()
{
	glGenVertexArrays(1, &m_vertexArray);
	glGenBuffers(1, &m_vertexNormalsBuffer);
	glGenBuffers(1, &m_elementsBuffer);

	glBindVertexArray(m_vertexArray);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexNormalsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(c_vertexNormals), &c_vertexNormals, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)(sizeof(glm::vec3)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementsBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(c_elements), &c_elements, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void obj_cube::DeInitGL()
{
	glDeleteVertexArrays(1, &m_vertexArray);
	glDeleteBuffers(1, &m_vertexNormalsBuffer);
	glDeleteBuffers(1, &m_elementsBuffer);
}
