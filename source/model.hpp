#pragma once 

#include <filesystem>
#include <assimp/matrix4x4.h>
#include <optional>
#include "GL_shader.hpp"
#include "mesh.hpp"
#include "modelLoadParams.hpp"
namespace fs = std::filesystem;

struct aiNode;
struct aiScene;
struct aiMesh;
struct aiTexture;

class model 
{
public:
	model() = default;
	~model() = default;

	bool IsValid();
	void LoadModel(fs::path path, modelLoadParams params);

	void Draw(GL_shader& shader);
	std::string GetName();

private:

	void ProcessScene(aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransformation);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4 transformation);

	std::vector<Texture> LoadTextures(aiMaterial* mat, aiTextureType type, const aiScene* scene);
	std::optional<Texture> IsTextureAlreadyLoaded(aiString path);
	GLuint LoadEmbeddedTexture(const aiTexture* embeddedTexture, aiString path);
	GLuint LoadFromFileTexture(aiString path);
	GLuint GenerateTexture2D(int width, int height, int nrComponents, unsigned char* data);

	void CalculateAABB();
	void CalculateMassCenter();
	void CalculateZeroOneBoxMatrix();

private:

	bool m_isValid = false;
	fs::path m_loadedFromPath;
	
	std::vector<mesh> m_meshes;
	std::vector<Texture> m_loadedTextures;

	std::pair<glm::vec3, glm::vec3> m_AABB;
	glm::vec3 m_massCenter;
	glm::mat4 m_toZeroOneBox;
};