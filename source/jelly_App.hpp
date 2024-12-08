#pragma once 

#include <memory>
#include <optional>
#include "jelly_Renderer.hpp"
#include "simulationParameters.hpp"
#include "bezierCube.hpp"
#include "jelly_simThread.hpp"

class jelly_App
{
	enum class simulationState
	{
		Initial,
		Running,
		Stopped
	};

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

	// Simulation Action
	void StartSimulation();
	void StopSimulation();
	void ResetSimulation();

	bool IsRunning();
	bool IsStopped();

	// Scene manipulation
	void ChoseMovableObject(float xpos, float ypos);
	void ChooseObject();
	void UnchooseObject(bool resetSeenPoint);
	void MoveChosenObject(float xpos, float ypos);

	// Cube
	void SetPointAttribute(int pointIndex, int attributeIndex, bool value);
	void SetCubeEdgeLength(float newLength);

	// Parameters
	std::shared_ptr<simulationParameters> GetSimulationParameters();
	std::shared_ptr<drawParameters> GetDrawParameters();
	
private:

	void ChoseSeenPoint(const glm::vec3& A, const glm::vec3& B);
	void ChoseControlFrame(const glm::vec3& A, const glm::vec3& B);

	float DistanceFromPointToLine(const glm::vec3& P, const glm::vec3& P1, const glm::vec3& P2);
	std::optional<glm::vec3> PlaneLineIntersection(
		const glm::vec3& P0, 
		const glm::vec3& N, 
		const glm::vec3& L0, 
		const glm::vec3& L);

	bool RayTriangleIntersection(
		const glm::vec3& A, 
		const glm::vec3& B, 
		const glm::vec3& V0, 
		const glm::vec3& V1, 
		const glm::vec3& V2);

private:

	std::unique_ptr<jelly_Renderer> m_renderer;
	std::unique_ptr<jelly_simThread> m_simThread;

	std::shared_ptr<simulationParameters> m_simParams;
	std::shared_ptr<bezierCube> m_bCube;

	simulationState m_simState = simulationState::Initial;

	int m_seenPointIndex = -1;
	bool b_ControlFrameSeen = false;
};