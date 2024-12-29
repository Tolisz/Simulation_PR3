#include "mesh.hpp"

#include <iostream>

mesh::mesh(
	std::vector<Vertex>&& vertices, 
	std::vector<unsigned int>&& indices,
	aiMatrix4x4 transformation):
	m_vertices{std::move(vertices)}, m_indices{std::move(indices)}
{
	// convert matrix from Assimp to GLM
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j) 
		{
			m_transformation[i][j] = transformation[j][i];
		}
	}

	CalculateAABB();
	InitGL();
}

mesh::~mesh()
{
	// DeInitGL();
}

void mesh::Draw(GL_shader& shader)
{
	shader.setM4fv("transformation", GL_FALSE, m_transformation);

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_indices.size()), GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);
}

void mesh::InitGL()
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);	

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);	
}

void mesh::DeInitGL()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}

std::pair<glm::vec3, glm::vec3> mesh::GetAABB()
{
	return m_AABB;
}

std::pair<glm::dvec3, size_t> mesh::GetMassCenter()
{
	return {m_massCenter, m_vertices.size()};
}

void mesh::CalculateAABB()
{    
	m_massCenter = glm::dvec3(0.0f);
	if (m_vertices.empty())
    {
        m_AABB = {glm::vec3(0.0f), glm::vec3(0.0f)};
        return;
    }

	glm::vec3 initial = m_transformation * glm::vec4(m_vertices[0].position, 1.0f); 
    glm::vec3 min = initial;
    glm::vec3 max = initial;

    for (const auto& vertex : m_vertices)
    {
		glm::vec3 tVertex = m_transformation * glm::vec4(vertex.position, 1.0f);
		m_massCenter += glm::dvec3(tVertex);

        min = glm::min(min, tVertex);
        max = glm::max(max, tVertex);
    }

    m_AABB = {min, max};
}