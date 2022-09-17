#pragma once
#include <iostream> // std::cout
#include <deque>	// std::deque
#include <future>   // std::future
#include <mutex>    // std::mutex
#include <type_traits>
#include <condition_variable> 

using namespace std;

class ThreadPool
{
public:

	ThreadPool();
	~ThreadPool();

	template <class F, class... Args>
	auto addTask(F&& f, Args&&... args)->future<typename result_of<F(Args...)>::type>;

private:

	deque<function<void()>> m_tasks;

	mutex					m_taskMutex;

	condition_variable		m_condition;

	bool					m_bStop;
};

template <class F, class... Args>
auto ThreadPool::addTask(F&& f, Args&&... args)->future<typename result_of<F(Args...)>::type>
{
	using return_type = typename result_of<F(Args...)>::type;  //保存返回值类型

	auto task = make_shared<packaged_task<return_type()>>(bind(forward<F>(f),forward<Args>(args)...)); //包装函数指针

	future<return_type> res = task->get_future(); //获取函数对象
	{
		unique_lock<mutex> lock(m_taskMutex); //自动上下锁
		
		if (m_bStop)
			throw runtime_error("addTask stop");

		m_tasks.emplace_back([task] {(*task)(); }); //添加任务

		m_condition.notify_one(); //通知
	}

	return res;
}

