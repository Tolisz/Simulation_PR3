#pragma once

#include <glad/glad.h>
#include <glm/vec2.hpp>
#include "GL_shader.hpp"
#include "GL_UBO.hpp"
#include "camera.hpp"
#include "obj_cube.hpp"
#include "bezierCubeDrawer.hpp"

class jelly_Renderer
{
public:

	jelly_Renderer(std::unique_ptr<bezierCubeDrawer> cubeDrawer);
	~jelly_Renderer();

	jelly_Renderer(const jelly_Renderer&) = delete;
	jelly_Renderer operator=(const jelly_Renderer&) = delete;
	jelly_Renderer(jelly_Renderer&&) = delete;
	jelly_Renderer operator=(jelly_Renderer&&) = delete;

	void Render();
	void UpdateRenderArea(int width, int height);

	GLuint GetRenderTexture();
	glm::uvec2 GetRenderAreaSize();

	// Camera
	const camera& Camera();

	void CameraRotate(float deltaX, float deltaY);
	void CameraMove(float deltaX, float deltaY);
	void CameraZoom(float factor);

	glm::mat4 GetProjectionMatrix();
	glm::mat4 GetViewMatrix();
	glm::vec3 GetCameraPos();

	// 
	void SetPointAttribute(int pointIndex, int attributeIndex, bool value);

private:

	void RenderScene();
	void SetUpFramebuffer();

	void PrepareScene();
	void PrepareShaders();
	void PrepareSceneObjects();

private:

	// Framebuffer related things
	glm::uvec2 	m_sceneSize;
	GLuint m_Framebuffer;
	GLuint m_Renderbuffer;
	GLuint m_colorTexture;

	// Camera
	camera m_camera;

	// Shaders
	GL_shader m_s_test;
	GL_shader m_s_bCubePoints;
	GL_shader m_s_cubeSprings;
	
	// Buffers
	GL_UBO m_b_matrices;

	// Scene objects
	obj_cube m_o_cube;

	std::unique_ptr<bezierCubeDrawer> m_bCube;
}; 
