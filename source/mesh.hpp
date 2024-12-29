#pragma once 

#include <glad/glad.h>
#include <glm/vec3.hpp>

struct Vertex 
{
    glm::vec3 position;
};

class mesh
{
public:

	mesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices);
	~mesh();

	void Draw();

private:

	void InitGL();
	void DeInitGL();

private:

	std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;

private:

	GLuint m_VAO;
	GLuint m_VBO;
	GLuint m_EBO;
};