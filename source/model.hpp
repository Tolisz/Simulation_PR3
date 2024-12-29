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
	std::string GetName();

private:

	void ProcessScene(aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransformation);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4 transformation);
	void CalculateAABB();
	void CalculateMassCenter();
	void CalculateZeroOneBoxMatrix();

private:

	bool m_isValid = false;
	std::vector<mesh> m_meshes;
	fs::path m_loadedFromPath;

	std::pair<glm::vec3, glm::vec3> m_AABB;
	glm::vec3 m_massCenter;
	glm::mat4 m_toZeroOneBox;
};