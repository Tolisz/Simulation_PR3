#pragma once

#include <glad/glad.h>
#include <glm/vec2.hpp>

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

private:

	void RenderScene();
	void SetUpFramebuffer();

private:

	glm::uvec2 	m_sceneSize;
	GLuint m_Framebuffer;
	GLuint m_Renderbuffer;
	GLuint m_colorTexture;
}; 
