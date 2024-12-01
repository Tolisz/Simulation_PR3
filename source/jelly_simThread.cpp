#include "jelly_simThread.hpp"

#include <iostream>

jelly_simThread::jelly_simThread()
{

}

jelly_simThread::~jelly_simThread()
{
	EndSimulation();
}

void jelly_simThread::StartSimulation()
{
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

void jelly_simThread::Main()
{
	while (true)
	{
		m_stopSimulation.lock();
		if (b_endSimulation)  {
			m_stopSimulation.unlock();
			break; 
		}
		m_stopSimulation.unlock();

		std::cout << "TEST" << std::endl;
	};

}