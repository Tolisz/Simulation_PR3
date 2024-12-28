#include "jelly_App.hpp"

#include <iostream>
#include <numeric>
#include <algorithm>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

jelly_App::jelly_App()
{
	m_simParams = std::make_shared<simulationParameters>();
	m_bCube = std::make_shared<bezierCube>(m_simParams->a);
	m_cFrame = std::make_shared<collisionFrame>(11.0f);

	auto bCubeDrawer = std::make_unique<bezierCubeDrawer>(m_bCube);
	auto cFrameDrawer = std::make_unique<collisionFrameDrawer>(m_cFrame);	
	m_renderer = std::make_unique<jelly_Renderer>(std::move(bCubeDrawer), std::move(cFrameDrawer));
	
	m_simThread = std::make_unique<jelly_simThread>(m_bCube, m_cFrame, m_simParams);
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
		m_bCube->Reset(m_simParams->a);
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
	
	if (m_simParams->bControlFrame)
	{
		ChoseControlFrame(cameraPos, world_far);
	}
	else 
	{
		ChoseSeenPoint(cameraPos, world_far);
	}
}

void jelly_App::ChoseSeenPoint(const glm::vec3& A, const glm::vec3& B)
{
	std::vector<glm::vec3> points = m_bCube->GetCubePoints();
	glm::vec3 AB = B - A;
	float d_min = std::numeric_limits<float>::max();
	int i_min = -1;

	for (int i = 0; i < points.size(); ++i)
	{
		glm::vec3 to_point = points[i] - A;

		// Dont check points behind the camera
		if (glm::dot(AB, to_point) < 0)
			continue;

		float d = DistanceFromPointToLine(points[i], A, B);
		float l = glm::length(to_point);
		if (d < 0.05f * l) {
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

void jelly_App::ChoseControlFrame(const glm::vec3& A, const glm::vec3& B)
{
	auto points = m_bCube->GetFramePoints();
	b_CFSeen = false;

	std::array<std::array<int, 4>, 6> faces = {{
        {0, 1, 2, 3}, // FRONT
        {4, 5, 6, 7}, // BACK

        {0, 4, 1, 5}, // TOP
        {2, 6, 3, 7}, // BOTTOM
        
		{2, 0, 6, 4}, // LEFT
        {3, 1, 7, 5}  // RIGHT
    }};

	for (const auto& face : faces)
	{
		{
			auto T = RayTriangleIntersection(A, B, points[face[0]], points[face[1]], points[face[2]]);
			if (T.first)
			{
				b_CFSeen = true;
				m_CFIntersection = A + T.second * (B - A);
				break;
			}
		}

		{
			auto T = RayTriangleIntersection(A, B, points[face[1]], points[face[2]], points[face[3]]);
			if (T.first)
			{
				b_CFSeen = true;
				m_CFIntersection = A + T.second * (B - A);
				break;
			}
		}
	}

	m_renderer->SetControlFrameChoosen(b_CFSeen);
}

void jelly_App::ChooseObject()
{
	if (!m_simParams->bControlFrame)
	{
		m_bCube->SetChosenPointIndex(m_seenPointIndex);
	}
}

void jelly_App::UnchooseObject(bool resetSeenPoint)
{
	if (m_simParams->bControlFrame)
	{
		m_renderer->SetControlFrameChoosen(false);
		b_CFSeen = false;	
	}
	else 
	{
		m_bCube->SetChosenPointIndex(-1);

		if (resetSeenPoint && m_seenPointIndex != -1)
		{
			SetPointAttribute(m_seenPointIndex, 1, false);
			m_seenPointIndex = -1;
		}
	}
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

	if (m_simParams->bControlFrame)
	{
		if (!b_CFSeen)
			return;

		glm::vec3 P0 = m_CFIntersection;
		glm::vec3 N  = m_renderer->Camera().GetVecFront();
		glm::vec3 L = glm::normalize(world_far - cameraPos);
		
		auto points = m_bCube->GetFramePoints();
		auto to_points = points;
		for (int i = 0; i < points.size(); ++i)
		{
			to_points[i] = points[i] - P0;
		}

		auto newPos = PlaneLineIntersection(P0, N, cameraPos, L);
		if (newPos.has_value())
		{
			glm::vec3 nP = newPos.value();
			for (int i = 0; i < points.size(); ++i)
			{
				points[i] = nP + to_points[i];
			}
			m_CFIntersection = nP;
		}

		m_bCube->SetFramePoints(points);
	}
	else 
	{
		if (m_bCube->GetChosenPointIndex() == -1)
			return;

		glm::vec3 P0 = m_bCube->GetChoosenPointPos();
		glm::vec3 N  = m_renderer->Camera().GetVecFront();
		glm::vec3 L = glm::normalize(world_far - cameraPos);

		auto newPos = PlaneLineIntersection(P0, N, cameraPos, L);
		if (newPos.has_value())
		{
			m_bCube->SetChoosenPointPos(newPos.value());
		}
	}
}

void jelly_App::StartControlFrameRotation(float xpos, float ypos)
{
	m_CFRotationStart = {xpos, ypos};
	m_CFRotationPoints = m_bCube->GetFramePoints();
	m_CFRotatedPoints = m_CFRotationPoints; 
	m_CFCenter = std::accumulate(m_CFRotationPoints.begin(), m_CFRotationPoints.end(), glm::vec3(0.0f));
	m_CFCenter /= m_CFRotationPoints.size();

	glm::mat4 Proj = m_renderer->GetProjectionMatrix();
	glm::mat4 View = m_renderer->GetViewMatrix();

	glm::vec4 ProjPoint = Proj * View * glm::vec4(m_CFCenter, 1.0f);
	ProjPoint /= ProjPoint.w;

	glm::uvec2 screenSize = m_renderer->GetRenderAreaSize();
	m_CFScreenCenter.x = screenSize.x * ((ProjPoint.x + 1.0f) / 2.0f);
	m_CFScreenCenter.y = screenSize.y * ((-ProjPoint.y + 1.0f) / 2.0f);
}

void jelly_App::EndControlFrameRotation()
{
	m_bCube->SetFramePoints(m_CFRotatedPoints);
}

void jelly_App::ControlFrameRotation_Front(float xpos, float ypos)
{
	// Compute rotation angle
	float angle1 = glm::atan2(m_CFRotationStart.y - m_CFScreenCenter.y, m_CFRotationStart.x - m_CFScreenCenter.x);
	float angle2 = glm::atan2(ypos - m_CFScreenCenter.y, xpos - m_CFScreenCenter.x);
	
	float angle = angle2 - angle1;
	if (angle < 0) { angle += 2 * glm::pi<float>(); }

	// Rotate points
	glm::quat rotation = glm::angleAxis(-angle, m_renderer->Camera().GetVecFront());
	m_CFRotatedPoints = m_CFRotationPoints;
	std::for_each(m_CFRotatedPoints.begin(), m_CFRotatedPoints.end(), 
		[&](auto& elem){ 
			elem = ((elem - m_CFCenter) * rotation) + m_CFCenter; 
			});

	m_bCube->SetFramePoints(m_CFRotatedPoints);
}

void jelly_App::ControlFrameRotation_RightUp(float xpos, float ypos)
{
	float angle_UP = 0.01f * (xpos - m_CFRotationStart.x);
	float angle_RIGHT = 0.01f * (ypos - m_CFRotationStart.y);

	glm::quat rot_UP = glm::angleAxis(-angle_UP, m_renderer->Camera().GetVecUp());
	glm::quat rot_RIGHT = glm::angleAxis(-angle_RIGHT, m_renderer->Camera().GetVecRight());

	m_CFRotatedPoints = m_CFRotationPoints;
	std::for_each(m_CFRotatedPoints.begin(), m_CFRotatedPoints.end(), 
		[&](auto& elem){ 
			elem = ((elem - m_CFCenter) * rot_UP) + m_CFCenter;
			elem = ((elem - m_CFCenter) * rot_RIGHT) + m_CFCenter;
			});

	m_bCube->SetFramePoints(m_CFRotatedPoints);
}

void jelly_App::SetPointAttribute(int pointIndex, int attributeIndex, bool value)
{
	m_renderer->SetPointAttribute(pointIndex, attributeIndex, value);
}

void jelly_App::SetCubeEdgeLength(float newLength)
{
	m_bCube->Reset(newLength);
}

float jelly_App::DistanceFromPointToLine(const glm::vec3& P, const glm::vec3& A, const glm::vec3& B)
{
    glm::vec3 AB = B - A;
    glm::vec3 AP = P - A;

    float t = glm::dot(AP, AB) / glm::dot(AB, AB);
    t = glm::clamp(t, 0.0f, 1.0f);

    glm::vec3 closestPoint = A + t * AB;
    return glm::length(P - closestPoint);
}

std::optional<glm::vec3> jelly_App::PlaneLineIntersection(
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

std::pair<bool, float> jelly_App::RayTriangleIntersection(
	const glm::vec3& A, 
	const glm::vec3& B, 
	const glm::vec3& V0, 
	const glm::vec3& V1, 
	const glm::vec3& V2)
{
    glm::vec3 dir = B - A;
    glm::vec3 edge1 = V1 - V0;
    glm::vec3 edge2 = V2 - V0;
    glm::vec3 h = glm::cross(dir, edge2);
    float a = glm::dot(edge1, h);

    if (glm::abs(a) < 1e-5f) return { false, 0.0f };

    float f = 1.0f / a;
    glm::vec3 s = A - V0;
    float u = f * glm::dot(s, h);
    if (u < 0.0f || u > 1.0f) return { false, 0.0f };

    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(dir, q);
    if (v < 0.0f || u + v > 1.0f) return { false, 0.0f };

    float t = f * glm::dot(edge2, q);
    return { (t >= 0.0f && t <= 1.0f), t};	
}

std::shared_ptr<simulationParameters> jelly_App::GetSimulationParameters()
{
	return m_simParams;
}

std::shared_ptr<drawParameters> jelly_App::GetDrawParameters()
{
	return m_renderer->GetDrawParameters();
}

void jelly_App::LoadModelFromFile(std::string path)
{
	
}