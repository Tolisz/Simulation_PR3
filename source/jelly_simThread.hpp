#pragma once

#include <thread>
#include <mutex>
#include <atomic>

class jelly_simThread
{
public:

	jelly_simThread();
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

	void Main();

private:

	// thread menagement 
	std::thread m_thread;
	bool b_threadRunning = false;
	bool b_simulationStopped = true;

	std::mutex 			m_stopSimulation;
	std::atomic_bool 	b_endSimulation;

};