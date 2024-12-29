#include "model.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>

bool model::IsValid()
{
	return m_isValid;
}

void model::Draw()
{
	for(auto& _mesh : m_meshes)
	{
		_mesh.Draw();
	}
}

void model::LoadModel(fs::path path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
		m_isValid = false;
		std::cout << importer.GetErrorString() << std::endl;
        return;
    }

	ProcessScene(scene->mRootNode, scene);

	m_isValid = true;
}

void model::ProcessScene(aiNode* node, const aiScene* scene)
{
	for(unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* _mesh = scene->mMeshes[node->mMeshes[i]];
		ProcessMesh(_mesh, scene);
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		ProcessScene(node->mChildren[i], scene);
	}
}

void model::ProcessMesh(aiMesh* _mesh, const aiScene* scene)
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

	m_meshes.push_back(mesh(std::move(vertices), std::move(indices)));
}