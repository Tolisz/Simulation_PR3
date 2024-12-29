#pragma once 

#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <assimp/matrix4x4.h>
#include "GL_shader.hpp"


struct Vertex 
{
    glm::vec3 position;
};

class mesh
{
public:

	mesh(
		std::vector<Vertex>&& vertices, 
		std::vector<unsigned int>&& indices,
		aiMatrix4x4 transformation);
	~mesh();

	void Draw(GL_shader& shader);

private:

	void InitGL();
	void DeInitGL();

private:

	std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
	glm::mat4 m_transformation;

private:

	GLuint m_VAO;
	GLuint m_VBO;
	GLuint m_EBO;
};