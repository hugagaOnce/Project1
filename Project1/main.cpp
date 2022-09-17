#include <iostream>
#include "ThreadPool.h"

int main()
{
	// create thread pool with 4 worker threads
	ThreadPool pool;

	for (size_t i = 0; i < 100; i++)
	{
		// enqueue and store future
		auto result = pool.addTask([](int answer, int eqrs) { return answer + eqrs; }, 42, 87);

		// get result from future, print 42
		std::cout << result.get() << std::endl;
	}
}