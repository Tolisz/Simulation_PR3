#include "jelly_simThread.hpp"

#include <iostream>
#include <glm/geometric.hpp>

jelly_simThread::jelly_simThread(
		std::shared_ptr<bezierCube> cube,
		std::shared_ptr<simulationParameters> simParams) :
	m_bCube{cube},
	m_simParams{simParams}
{

}

jelly_simThread::~jelly_simThread()
{
	EndSimulation();
}

void jelly_simThread::StartSimulation()
{
	Start();

	b_endSimulation = false;
	m_thread = std::thread(&jelly_simThread::Main, this);
	b_threadRunning = true;
	b_simulationStopped = false;
}

void jelly_simThread::StopSimulation()
{
	if (b_threadRunning)
	{
		m_stopSimulation.lock();
		b_simulationStopped = true;
	}
}

void jelly_simThread::ContinueSimulation()
{
	if (b_threadRunning)
	{
		m_stopSimulation.unlock();
		b_simulationStopped = false;
	}
}

void jelly_simThread::EndSimulation()
{
	if (b_threadRunning)
	{
		b_endSimulation = true;

		if (b_simulationStopped)
		{
			m_stopSimulation.unlock();
			b_simulationStopped = false;
		}

		m_thread.join();
		b_threadRunning = false;
	}
}

void jelly_simThread::Start()
{
	m_V.assign(64, glm::vec3(0.0f));
	m_F.assign(64, glm::vec3(0.0f));
	m_newP.assign(64, glm::vec3(0.0f));

	// Compute Initial Values of dl change
	std::vector<glm::vec3> P = m_bCube->GetCubePoints();
	m_prevDL = m_bCube->GetCubeSprings();
	for(const auto& I : m_prevDL)
	{
		int i = I.first;
		for (const auto& J : I.second) 
		{
			int j = J.first;
			
			float rl = J.second;
			float cl = glm::length(P[j] - P[i]);
			float dl = cl - rl;

			m_prevDL[i][j] = dl;
		}
	}	

	m_dt = m_simParams->dt;
}

void jelly_simThread::Main()
{
	auto START = std::chrono::high_resolution_clock::now();
	int N = 0;
	
	std::chrono::duration<double> DELTA(static_cast<double>(m_dt));

	while (true)
	{
		if (m_stopSimulation.try_lock())
		{
			if (b_endSimulation)  {
				m_stopSimulation.unlock();
				break; 
			}
			m_stopSimulation.unlock();
		}
		else 
		{
			auto lockStart = std::chrono::high_resolution_clock::now();
			
			m_stopSimulation.lock();
			if (b_endSimulation)  {
				m_stopSimulation.unlock();
				break; 
			}
			m_stopSimulation.unlock();

			auto lockEnd = std::chrono::high_resolution_clock::now();
			START += (lockEnd - lockStart);
		}

		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> t = now - START;
		int N_theoretical = static_cast<int>(t.count() / static_cast<double>(m_dt));
		
		auto end = N_theoretical > N ? now : now + DELTA * (N - N_theoretical);

		// =====================
			SimulationStep();
		// =====================

		// :(
		while (std::chrono::high_resolution_clock::now() < end) { }
		N++;
	};

}

void jelly_simThread::SimulationStep()
{
	std::vector<glm::vec3> P = m_bCube->GetCubePoints();
	m_F.assign(64, glm::vec3(0.0f));

	// Compute forces of the cube's springs
	ComputeCubeForces(P);

	// Compute forces for springs to control frame
	ComputeControlFrameFoces(P);

	// Compute new positions
	int chosenInd = m_bCube->GetChosenPointIndex();
	for (int i = 0; i < P.size(); ++i)
	{
		if (i == chosenInd)
		{
			m_newP[i] = P[i];
			m_V[i] = glm::vec3(0.0f);
			continue;
		}

		m_V[i] += m_dt * ( (1.0f / m_simParams->m[i]) * (m_F[i]) ); 
		m_newP[i] = P[i] + m_V[i] * m_dt;
	}

	m_bCube->SetCubePoints(m_newP);
}

void jelly_simThread::ComputeCubeForces(const std::vector<glm::vec3>& P)
{
	auto restLengths = m_bCube->GetCubeSprings();
	for(const auto& I : restLengths)
	{
		int i = I.first;
		for (const auto& J : I.second) 
		{
			int j = J.first;

			// compute dl
			float rl = J.second;
			float cl = glm::length(P[j] - P[i]);
			float dl = cl - rl;
			
			// compute dl derivative
			float dl_drv = (dl - m_prevDL[i][j]) / m_dt; 
			m_prevDL[i][j] = dl;

			glm::vec3 f = glm::normalize(P[j] - P[i]) * (
				- m_simParams->k * dl_drv - m_simParams->c1 *  dl);

			m_F[i] += -f;
			m_F[j] += f;
		}
	}
}

void jelly_simThread::ComputeControlFrameFoces(const std::vector<glm::vec3>& P)
{
	if (!m_simParams->bControlFrame) return;


}