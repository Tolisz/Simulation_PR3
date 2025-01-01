#pragma once 

#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <assimp/matrix4x4.h>
#include "GL_shader.hpp"
#include <assimp/material.h>

struct Vertex 
{
    glm::vec3 position;
	glm::vec2 texCoords;
	glm::vec3 normal;
};

struct Texture
{
	aiTextureType type;
	GLuint id;
	std::string path;
};

class mesh
{
public:

	mesh(
		std::vector<Vertex>&& vertices, 
		std::vector<unsigned int>&& indices,
		std::vector<Texture>& textures,
		aiMatrix4x4 transformation);
	~mesh();

	void Draw(GL_shader& shader);
	std::pair<glm::vec3, glm::vec3> GetAABB();
	std::pair<glm::dvec3, size_t> GetMassCenter();
	
private:

	void InitGL();
	void DeInitGL();
	void CalculateAABB();

private:

	std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
	std::vector<Texture> m_textures;
	glm::mat4 m_transformation;
	std::pair<glm::vec3, glm::vec3> m_AABB;
	glm::dvec3 m_massCenter;

private:

	GLuint m_VAO;
	GLuint m_VBO;
	GLuint m_EBO;
};