#include "jelly_App.hpp"

jelly_App::jelly_App()
{
	m_simulationParams = std::make_shared<simulationParameters>();
	m_bCube = std::make_shared<bezierCube>(m_simulationParams->a);
	
	auto m_bCubeDrawer = std::make_unique<bezierCubeDrawer>(m_bCube);
	m_renderer = std::make_unique<jelly_Renderer>(std::move(m_bCubeDrawer));
	m_simThread = std::make_unique<jelly_simThread>();
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

void jelly_App::CameraMove(float deltaX, float deltaY, float width, float height)
{
	m_renderer->CameraMove(deltaX, deltaY, width, height);
}

void jelly_App::CameraZoom(float factor)
{
	m_renderer->CameraZoom(factor);
}

void jelly_App::StartSimulation()
{
	switch (m_simState)
	{
	case simulationState::Initial:
		m_simState = simulationState::Running;
		m_simThread->StartSimulation();
		break;
	
	case simulationState::Stopped:
		m_simState = simulationState::Running;
		m_simThread->ContinueSimulation();
		break;
	}
}

void jelly_App::StopSimulation()
{
	switch (m_simState)
	{
	case simulationState::Running:
		m_simState = simulationState::Stopped;
		m_simThread->StopSimulation();
		break;
	}
}

void jelly_App::ResetSimulation()
{
	switch (m_simState)
	{
	case simulationState::Stopped:
		m_simState = simulationState::Initial;
		m_simThread->EndSimulation();
		break;
	}
}

bool jelly_App::IsRunning()
{
	return m_simState == simulationState::Running;
}

bool jelly_App::IsStopped()
{
	return m_simState == simulationState::Stopped;
}

std::shared_ptr<simulationParameters> jelly_App::GetSimulationParameters()
{
	return m_simulationParams;
}