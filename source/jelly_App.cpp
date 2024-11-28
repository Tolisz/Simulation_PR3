#include "jelly_App.hpp"

jelly_App::jelly_App()
{
	m_renderer = std::make_unique<jelly_Renderer>();
}

GLuint jelly_App::GetRenderTexture()
{
	return m_renderer->GetRenderTexture();
}

void jelly_App::SetRenderArea(int width, int height)
{
	m_renderer->UpdateRenderArea(width, height);
}

void jelly_App::RenderScene()
{
	m_renderer->Render();
}

void jelly_App::CameraRotate(float deltaX, float deltaY)
{
	m_renderer->CameraRotate(deltaX, deltaY);
}

void jelly_App::CameraMove(float deltaX, float deltaY)
{
	m_renderer->CameraMove(deltaX, deltaY);
}

void jelly_App::CameraZoom(float factor)
{
	m_renderer->CameraZoom(factor);
}