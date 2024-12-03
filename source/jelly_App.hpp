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
	void ChooseObject(float xpos, float ypos);
	void MoveChosenObject(float xpos, float ypos);

	// Parameters
	std::shared_ptr<simulationParameters> GetSimulationParameters();
	
private:

	float distanceFromPointToLine(const glm::vec3& P, const glm::vec3& P1, const glm::vec3& P2);
	std::optional<glm::vec3> planeLineIntersection(
		const glm::vec3& P0, 
		const glm::vec3& N, 
		const glm::vec3& L0, 
		const glm::vec3& L);

private:

	std::unique_ptr<jelly_Renderer> m_renderer;
	std::unique_ptr<jelly_simThread> m_simThread;

	std::shared_ptr<simulationParameters> m_simulationParams;
	std::shared_ptr<bezierCube> m_bCube;

	simulationState m_simState = simulationState::Initial;
};