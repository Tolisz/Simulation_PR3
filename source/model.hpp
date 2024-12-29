#pragma once 

#include <filesystem>
#include <assimp/matrix4x4.h>
#include "GL_shader.hpp"
#include "mesh.hpp"
namespace fs = std::filesystem;

struct aiNode;
struct aiScene;
struct aiMesh;

class model 
{
public:
	model() = default;
	~model() = default;

	bool IsValid();
	void LoadModel(fs::path path);

	void Draw(GL_shader& shader);

private:

	void ProcessScene(aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransformation);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4 transformation);

private:

	bool m_isValid = false;
	std::vector<mesh> m_meshes;
	fs::path m_loadedFromPath;
};