#pragma once 

#include <glad/glad.h>
#include <glm/glm.hpp>

class obj_cube
{

public: 

	obj_cube();
	~obj_cube();

	obj_cube(const obj_cube&) = delete;
	obj_cube(obj_cube&&) = delete;
	obj_cube& operator=(const obj_cube&) = delete; 
	obj_cube& operator=(obj_cube&&) = delete;

	void Draw();

private:

	void InitGL();
	void DeInitGL();

private:

	static const glm::vec3	c_vertexNormals[];
	static const glm::uvec3	c_elements[];

	GLuint m_vertexArray;
	GLuint m_vertexNormalsBuffer;
	GLuint m_elementsBuffer;
};
