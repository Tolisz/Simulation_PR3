#pragma once 

#include <memory>
#include "jelly_Renderer.hpp"

class jelly_App
{
public:

	jelly_App();
	~jelly_App() = default;

	jelly_App(const jelly_App&) = delete;
	jelly_App& operator=(const jelly_App&) = delete;
	jelly_App(jelly_App&&) = delete;
	jelly_App& operator=(jelly_App&&) = delete;

	// Renderer
	GLuint GetRenderTexture();
	void SetRenderArea(int width, int height);
	void RenderScene();

	void CameraRotate(float deltaX, float deltaY);
	void CameraMove(float deltaX, float deltaY);
	void CameraZoom(float factor);
private:

	std::unique_ptr<jelly_Renderer> m_renderer;

};