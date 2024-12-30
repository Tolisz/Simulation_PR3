#include "model.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <iostream>

#include <stb_image.h>

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

void model::LoadModel(fs::path path, modelLoadParams params)
{
	unsigned int flags = 0;
	flags |= aiProcess_Triangulate;  
	flags |= aiProcess_GenSmoothNormals; 
	flags |= aiProcess_CalcTangentSpace;
	if (params.bFlipUVs) flags |= aiProcess_FlipUVs; 

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.string(), flags);
	
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
		m_isValid = false;
		std::cout << importer.GetErrorString() << std::endl;
        return;
    }

	m_loadedFromPath = path;
	
	ProcessScene(scene->mRootNode, scene, scene->mRootNode->mTransformation);
	CalculateAABB();
	CalculateMassCenter();
	CalculateZeroOneBoxMatrix();

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
	std::vector<Texture> textures;

	for(unsigned int i = 0; i < _mesh->mNumVertices; ++i)
    {
		Vertex v;
		v.position = glm::vec3(
			_mesh->mVertices[i].x,
			_mesh->mVertices[i].y, 
			_mesh->mVertices[i].z);
		
		
		if(_mesh->HasTextureCoords(0))
		{
			v.texCoords = glm::vec2(
				_mesh->mTextureCoords[0][i].x,
				_mesh->mTextureCoords[0][i].y
			);
		}

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

	aiMaterial* mat = scene->mMaterials[_mesh->mMaterialIndex];

	std::vector<Texture> texDiffuse = LoadTextures(mat, aiTextureType_DIFFUSE, scene);
	textures.insert(textures.end(), texDiffuse.begin(), texDiffuse.end());

	m_meshes.push_back(mesh(std::move(vertices), std::move(indices), textures, transformation));
}

std::vector<Texture> model::LoadTextures(aiMaterial* mat, aiTextureType type, const aiScene* scene)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
	{
		aiString path;
		mat->GetTexture(type, i, &path);
		
		std::optional<Texture> isLoaded = IsTextureAlreadyLoaded(path);
		if (isLoaded.has_value())
		{
			textures.push_back(isLoaded.value());
		}
		else 
		{
			Texture newTexture;
			newTexture.type = type;
			newTexture.path = path.C_Str();
			std::cout << newTexture.path << std::endl;

			const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(path.C_Str());
			if (embeddedTexture)
			{
				newTexture.id = LoadEmbeddedTexture(embeddedTexture, path);
			}
			else 
			{
				newTexture.id = LoadFromFileTexture(path);
			}

			textures.push_back(newTexture);
			m_loadedTextures.push_back(newTexture);
		}
	}

	return textures;
}

std::optional<Texture> model::IsTextureAlreadyLoaded(aiString path)
{
	for(Texture& loadedTexture : m_loadedTextures)
	{
		if(std::strcmp(loadedTexture.path.c_str(), path.C_Str()) == 0)
		{
			return loadedTexture;
		}
	}

	return std::nullopt;
}

GLuint model::LoadEmbeddedTexture(const aiTexture* embeddedTexture, aiString path)
{
	GLuint textureID = 0;

	// texture is compressed
	if (embeddedTexture->mHeight == 0)
	{
		int width, height, nrComponents;
		unsigned char* data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(embeddedTexture->pcData), embeddedTexture->mWidth, &width, &height, &nrComponents, 0);

		if (data)
		{
			textureID = GenerateTexture2D(width, height, nrComponents, data);
			stbi_image_free(data);
		}
		else 
		{
			std::cout << "Can not load embedded texture from path [" << path.C_Str() << "]" << std::endl; 
		}
	}
	else 
	{
		std::cout << "NOT IMPLEMENTED [" << path.C_Str() << "]" << std::endl;
	}

	return textureID;
}

GLuint model::LoadFromFileTexture(aiString path)
{
	GLuint textureID = 0;

	fs::path fullPath = m_loadedFromPath.parent_path();
	fullPath.append(path.C_Str());

	int width, height, nrComponents;
	unsigned char* data = stbi_load(fullPath.string().c_str(), &width, &height, &nrComponents, 0);

	if (data)
	{
		textureID = GenerateTexture2D(width, height, nrComponents, data);
		stbi_image_free(data);
	}
	else 
	{
		std::cout << "Can not load file texture from path [" << path.C_Str() << "]" << std::endl; 
	}

	return textureID;
}

GLuint model::GenerateTexture2D(int width, int height, int nrComponents, unsigned char* data)
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	GLenum format;
	if (nrComponents == 1)
		format = GL_RED;
	else if (nrComponents == 3)
		format = GL_RGB;
	else if (nrComponents == 4)
		format = GL_RGBA;

	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return textureID;
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

void model::CalculateMassCenter()
{
	glm::dvec3 massCenter = glm::dvec3(0.0, 0.0, 0.0);
	size_t vertexNum = 0;

	for (int i = 0; i < m_meshes.size(); ++i)
	{
		std::pair<glm::dvec3, size_t> center = m_meshes[i].GetMassCenter();
		
		massCenter += center.first;
		vertexNum += center.second;
	}

	massCenter /= static_cast<double>(vertexNum);
	m_massCenter = massCenter;
}

void model::CalculateZeroOneBoxMatrix()
{
	glm::vec3 LengthP = glm::abs(m_AABB.second - m_massCenter);
	glm::vec3 LengthN = glm::abs(m_massCenter - m_AABB.first);

	float maxLength = glm::max(
		glm::max(LengthP.x, glm::max(LengthP.y, LengthP.z)),
		glm::max(LengthN.x, glm::max(LengthN.y, LengthN.z))
	);
	float scaleFactor = 0.5f / maxLength;

	glm::mat4 translateToOrigine = glm::translate(glm::mat4(1.0f), -m_massCenter);
	glm::mat4 scaleToCube = glm::scale(glm::mat4(1.0f), glm::vec3(scaleFactor));
	glm::mat4 translateToZeroOneCube = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));

	m_toZeroOneBox = translateToZeroOneCube * scaleToCube * translateToOrigine;
}