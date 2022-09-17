#include "ThreadPool.h"
#include <array>    // std::array
#include <thread>   // std::thread
#include <chrono>   // std::chrono

array<thread,8> thread_pool;

ThreadPool::ThreadPool():m_bStop(false)
{
	for (size_t i = 0; i < 8; i++)
	{
		thread_pool[i] = thread([this]{
			while (true)
			{
				function<void()> task;
				{
					unique_lock<mutex> lock(m_taskMutex);

					m_condition.wait(lock, [this] { return m_bStop || !m_tasks.empty(); });

					if (m_bStop && m_tasks.empty())
						return;

					task = move(m_tasks.front());
					m_tasks.pop_front();
				}
				task();
			}
		});
	}
}

ThreadPool::~ThreadPool()
{
	{
		unique_lock<mutex> lock(m_taskMutex);
		m_bStop = true;
	}
	m_condition.notify_all();
	for (thread &worker : thread_pool)
		worker.join();
}

