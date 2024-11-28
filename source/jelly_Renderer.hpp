#pragma once

#include <glad/glad.h>
#include <glm/vec2.hpp>
#include "GL_shader.hpp"
#include "GL_UBO.hpp"
#include "camera.hpp"
#include "obj_cube.hpp"

class jelly_Renderer
{
public:

	jelly_Renderer();
	~jelly_Renderer();

	jelly_Renderer(const jelly_Renderer&) = delete;
	jelly_Renderer operator=(const jelly_Renderer&) = delete;
	jelly_Renderer(jelly_Renderer&&) = delete;
	jelly_Renderer operator=(jelly_Renderer&&) = delete;

	void Render();
	void UpdateRenderArea(int width, int height);

	GLuint GetRenderTexture();

	// Camera
	void CameraRotate(float deltaX, float deltaY);
	void CameraMove(float deltaX, float deltaY);
	void CameraZoom(float factor);

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

	// Buffers
	GL_UBO m_b_matrices;

	// Scene objects
	obj_cube m_o_cube;
}; 
