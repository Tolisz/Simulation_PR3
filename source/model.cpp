#include "model.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <iostream>

bool model::IsValid()
{
	return m_isValid;
}

void model::Draw(GL_shader& shader)
{
	shader.setM4fv("toZeroOneBox", false, m_toZeroOneBox);
	
	for(auto& _mesh : m_meshes)
	{
		_mesh.Draw(shader);
	}
}

std::string model::GetName()
{
	return m_loadedFromPath.filename().string();
}

void model::LoadModel(fs::path path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.string(), 
		aiProcess_Triangulate | 
		aiProcess_GenSmoothNormals | 
		aiProcess_FlipUVs | 
		aiProcess_CalcTangentSpace | 
		aiProcess_GenBoundingBoxes);
	
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
		m_isValid = false;
		std::cout << importer.GetErrorString() << std::endl;
        return;
    }
	ProcessScene(scene->mRootNode, scene, scene->mRootNode->mTransformation);
	CalculateAABB();
	CalculateZeroOneBoxMatrix();
	// std::cout << "Min = " << m_AABB.first.x << ", " << m_AABB.first.y << ", "<< m_AABB.first.z << std::endl;
	// std::cout << "Max = " << m_AABB.second.x << ", " << m_AABB.second.y << ", "<< m_AABB.second.z << std::endl;

	m_loadedFromPath = path;
	m_isValid = true;
}

void model::ProcessScene(aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransformation)
{
	for(unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* _mesh = scene->mMeshes[node->mMeshes[i]];
		ProcessMesh(_mesh, scene, parentTransformation);
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		aiMatrix4x4 nextTransformation = parentTransformation * node->mChildren[i]->mTransformation;
		ProcessScene(node->mChildren[i], scene, nextTransformation);
	}
}

void model::ProcessMesh(aiMesh* _mesh, const aiScene* scene, aiMatrix4x4 transformation)
{
	std::vector<Vertex> vertices(_mesh->mNumVertices);
	std::vector<unsigned int> indices;

	for(unsigned int i = 0; i < _mesh->mNumVertices; ++i)
    {
		Vertex v;
		v.position = glm::vec3(
			_mesh->mVertices[i].x,
			_mesh->mVertices[i].y, 
			_mesh->mVertices[i].z);
		
		vertices[i] = v;
	}

	for(unsigned int i = 0; i < _mesh->mNumFaces; ++i)
	{
		aiFace face = _mesh->mFaces[i];
		for(unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	m_meshes.push_back(mesh(std::move(vertices), std::move(indices), transformation));
}

void model::CalculateAABB()
{
	if (m_meshes.empty())
	{
		m_AABB = {glm::vec3(0.0f), glm::vec3(0.0f)};
        return;
	}

	m_AABB = m_meshes[0].GetAABB();
	
	for (int i = 1; i < m_meshes.size(); ++i)
	{
		auto mesh_aabb = m_meshes[i].GetAABB();
		m_AABB.first = glm::min(m_AABB.first, mesh_aabb.first);
		m_AABB.second = glm::max(m_AABB.second, mesh_aabb.second);
	}
}

void model::CalculateZeroOneBoxMatrix()
{
	glm::vec3 lengths = glm::abs(m_AABB.second - m_AABB.first);
	float maxLength = glm::max(lengths.x, glm::max(lengths.y, lengths.z));
	
	float scaleFactor = 1.0f / maxLength;
	glm::vec3 translation = -m_AABB.first;

	glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(scaleFactor));
    glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), translation);

	m_toZeroOneBox = scale_matrix * translation_matrix;
}