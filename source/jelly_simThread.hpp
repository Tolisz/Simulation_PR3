#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include "bezierCube.hpp"
#include "simulationParameters.hpp"

class jelly_simThread
{
public:

	jelly_simThread(
		std::shared_ptr<bezierCube> cube,
		std::shared_ptr<simulationParameters> simParams);
	~jelly_simThread();

	jelly_simThread(const jelly_simThread&) = delete;
	jelly_simThread(jelly_simThread&&) = delete;
	jelly_simThread operator=(const jelly_simThread&) = delete;
	jelly_simThread operator=(jelly_simThread&&) = delete;

	// Simulation Actions
	void StartSimulation();
	void StopSimulation();
	void ContinueSimulation();
	void EndSimulation();

private:

	void Start();
	void Main();
	void SimulationStep();
	void ComputeCubeForces(const std::vector<glm::vec3>& P);
	void ComputeControlFrameFoces(const std::vector<glm::vec3>& P);

private:

	// thread menagement 
	std::thread m_thread;
	bool b_threadRunning = false;
	bool b_simulationStopped = true;

	std::mutex 			m_stopSimulation;
	std::atomic_bool 	b_endSimulation;

	// ===================
	std::shared_ptr<bezierCube> m_bCube;
	std::shared_ptr<simulationParameters> m_simParams;

	std::vector<glm::vec3> m_V;
	std::vector<glm::vec3> m_F;
	std::vector<glm::vec3> m_newP;
	std::unordered_map<int, std::unordered_map<int, float>> m_prevDL;
	std::array<float, 8> m_prevDL_frameSprings;

	float m_dt;
};