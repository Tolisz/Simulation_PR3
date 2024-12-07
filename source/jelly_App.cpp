#include "jelly_App.hpp"

#include <iostream>

jelly_App::jelly_App()
{
	m_simParams = std::make_shared<simulationParameters>();
	m_bCube = std::make_shared<bezierCube>(m_simParams->a);
	
	auto m_bCubeDrawer = std::make_unique<bezierCubeDrawer>(m_bCube);
	m_renderer = std::make_unique<jelly_Renderer>(std::move(m_bCubeDrawer));
	
	m_simThread = std::make_unique<jelly_simThread>(m_bCube, m_simParams);
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
		m_bCube->ResetCube(m_simParams->a);
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

void jelly_App::ChoseMovableObject(float xpos, float ypos)
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
	glm::vec3 to_world_far = world_far - cameraPos;

	std::vector<glm::vec3> points = m_bCube->GetPoints();

	float d_min = std::numeric_limits<float>::max();
	int i_min = -1;

	for (int i = 0; i < points.size(); ++i)
	{
		glm::vec3 to_point = points[i] - cameraPos;

		// Dont check points behind the camera
		if (glm::dot(to_world_far, to_point) < 0)
			continue;

		float d = distanceFromPointToLine(points[i], cameraPos, world_far);
		float l = glm::length(to_point);
		if (d < 0.005f * l) {
			if (d < d_min) {
				d_min = d;
				i_min = i;
			}
		}
	}

	if (m_seenPointIndex != -1)
		SetPointAttribute(m_seenPointIndex, 1, false);

	m_seenPointIndex = i_min;
	if (m_seenPointIndex != -1)
		SetPointAttribute(m_seenPointIndex, 1, true);
	
}

void jelly_App::ChooseObject()
{
	m_bCube->SetChosenPointIndex(m_seenPointIndex);
}

void jelly_App::UnchooseObject(bool resetSeenPoint)
{
	m_bCube->SetChosenPointIndex(-1);

	if (resetSeenPoint && m_seenPointIndex != -1)
		SetPointAttribute(m_seenPointIndex, 1, false);
}

void jelly_App::MoveChosenObject(float xpos, float ypos)
{
	glm::vec2 renderSize = m_renderer->GetRenderAreaSize();
	glm::vec4 NDC_far = glm::vec4(	 
		(glm::vec2(xpos, ypos) / renderSize * 2.0f) - 1.0f,
		1.0f,
		1.0f
	);
	NDC_far.y = -NDC_far.y;

	glm::mat4 invP = glm::inverse(m_renderer->GetProjectionMatrix());
	glm::mat4 invV = glm::inverse(m_renderer->GetViewMatrix());

	glm::vec4 view_far = invP * NDC_far;
	view_far /= view_far.w;

	glm::vec3 world_far = glm::vec3(invV * view_far); 
	glm::vec3 cameraPos = m_renderer->GetCameraPos();

	if (m_bCube->GetChosenPointIndex() != -1)
	{
		glm::vec3 P0 = m_bCube->GetChoosenPointPos();
		glm::vec3 N  = m_renderer->Camera().GetVecFront();
		glm::vec3 L = glm::normalize(world_far - cameraPos);

		auto newPos = planeLineIntersection(P0, N, cameraPos, L);
		if (newPos.has_value())
		{
			m_bCube->SetChoosenPointPos(newPos.value());
		}
	}
}

void jelly_App::SetPointAttribute(int pointIndex, int attributeIndex, bool value)
{
	m_renderer->SetPointAttribute(pointIndex, attributeIndex, value);
}

void jelly_App::SetCubeEdgeLength(float newLength)
{
	m_bCube->ResetCube(newLength);
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

std::optional<glm::vec3> jelly_App::planeLineIntersection(
		const glm::vec3& P0, 
		const glm::vec3& N, 
		const glm::vec3& L0, 
		const glm::vec3& L)
{
	float NL = glm::dot(N, L);

	if (glm::abs(NL) < 1e-8f) { 
        return std::nullopt; 
    }	

	float t = glm::dot(P0 - L0, N) / NL;
	return L0 + t * L;
}

std::shared_ptr<simulationParameters> jelly_App::GetSimulationParameters()
{
	return m_simParams;
}

std::shared_ptr<drawParameters> jelly_App::GetDrawParameters()
{
	return m_renderer->GetDrawParameters();
}