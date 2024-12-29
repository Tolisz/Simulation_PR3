#pragma once 

#include <filesystem>
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

	void Draw();

private:

	void ProcessScene(aiNode* node, const aiScene* scene);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene);

private:

	bool m_isValid = false;
	std::vector<mesh> m_meshes;
	fs::path m_loadedFromPath;
};