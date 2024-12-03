#include "jelly_App.hpp"

#include <iostream>

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

void jelly_App::CameraMove(float deltaX, float deltaY)
{
	m_renderer->CameraMove(deltaX, deltaY);
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

void jelly_App::ChooseObject(float xpos, float ypos)
{
	glm::vec2 renderSize = m_renderer->GetRenderAreaSize();
	glm::vec4 NDC_far = glm::vec4(	 
		(glm::vec2(xpos, ypos) / renderSize * 2.0f) - 1.0f,
		1.0f,
		1.0f
	);
	NDC_far.y = -NDC_far.y;

	glm::mat4 invProj = glm::inverse(m_renderer->GetProjectionMatrix());
	glm::mat4 invView = glm::inverse(m_renderer->GetViewMatrix());

	glm::vec4 view_far = invProj * NDC_far;
	view_far /= view_far.w;

	glm::vec3 world_far = glm::vec3(invView * view_far); 
	glm::vec3 cameraPos = m_renderer->GetCameraPos();

	std::vector<glm::vec3> points = m_bCube->GetPoints();

	float d_min = std::numeric_limits<float>::max();
	int i_min = -1;

	for (int i = 0; i < points.size(); ++i)
	{
		if (glm::dot(world_far - cameraPos, points[i] - cameraPos) < 0)
			continue;

		float d = distanceFromPointToLine(points[i], cameraPos, world_far);
		
		if (d < 0.05f) { // MAKE IT BETTER
			if (d < d_min) {
				d_min = d;
				i_min = i;
			}
		}
	}
	
	m_bCube->SetChosenPoint(i_min);
}

float jelly_App::distanceFromPointToLine(const glm::vec3& P, const glm::vec3& A, const glm::vec3& B)
{
    glm::vec3 AB = B - A;
    glm::vec3 AP = P - A;

    float t = glm::dot(AP, AB) / glm::dot(AB, AB);
    t = glm::clamp(t, 0.0f, 1.0f);

    glm::vec3 closestPoint = A + t * AB;
    return glm::length(P - closestPoint);
}

std::shared_ptr<simulationParameters> jelly_App::GetSimulationParameters()
{
	return m_simulationParams;
}