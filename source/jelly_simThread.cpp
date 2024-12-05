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

glm::vec3 jelly_simThread::Derivative_V(const glm::vec3& V, const float& m, const float& k, const glm::vec3& f)
{
	return (1.0f / m) * (- k * V + f);
}

glm::vec3 jelly_simThread::Derivative_X(const glm::vec3& V)
{
	return V;
}

void jelly_simThread::SimulationStep()
{
	std::vector<glm::vec3> P = m_bCube->GetPoints();
	m_F.assign(64, glm::vec3(0.0f));

	// Compute forces
	auto restLengths = m_bCube->GetRestLengths();	
	for(auto I : restLengths)
	{
		int i = I.first;
		for (auto J : I.second) 
		{
			int j = J.first;
			float rl = J.second;
			float cl = glm::length(P[j] - P[i]);
			float dl = cl - rl;
			glm::vec3 f = glm::normalize(P[j] - P[i]) * m_simParams->c1 * dl;

			if (dl > 0) 
			{
				m_F[i] += f;
				m_F[j] += -f;
			}
			else // dl <= 0
			{
				m_F[i] += -f;
				m_F[j] += f;
			}
		}
	}

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

		// Runge–Kutta for v;
		const glm::vec3& 	v = m_V[i];
		const float& 		m = m_simParams->m[i];
		const glm::vec3& 	f = m_F[i];
		const float& 		k = m_simParams->k;

		glm::vec3 k1_V = Derivative_V(v, m, k, f);
		glm::vec3 k2_V = Derivative_V(v + k1_V * (m_dt / 2.0f), m, k, f);
		glm::vec3 k3_V = Derivative_V(v + k2_V * (m_dt / 2.0f), m, k, f);
		glm::vec3 k4_V = Derivative_V(v + k3_V * m_dt, m, k, f);
		glm::vec3 dV = (k1_V + 2.0f * k2_V + 2.0f * k3_V + k4_V) * (m_dt / 6.0f);
		m_V[i] += dV; 

		// Runge–Kutta for x
		glm::vec3 k1_X = Derivative_X(v);
		glm::vec3 k2_X = Derivative_X(v + k1_X * (m_dt / 2.0f));
		glm::vec3 k3_X = Derivative_X(v + k2_X * (m_dt / 2.0f));
		glm::vec3 k4_X = Derivative_X(v + k3_X * m_dt);
		glm::vec3 dX = (k1_X + 2.0f * k2_X + 2.0f * k3_X + k4_X) * (m_dt / 6.0f);
		m_newP[i] = P[i] + dX;
	}

	m_bCube->SetPoints(m_newP);
}