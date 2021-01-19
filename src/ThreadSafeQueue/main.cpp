#include <iostream>
#include <thread>
#include <memory>

#include "ThreadSafeQueue.h"

using namespace std;

class Example
{
public:
	typedef std::shared_ptr<Example> ptr;
	Example()
	{

	}
	virtual~Example()
	{

	}

public:
	std::vector<int> m_Data;
};


ThreadSafeQueue<std::shared_ptr<Example>> GlobalQueue;

void produce_thread() 
{
	while (true)
	{
		std::shared_ptr<Example> pExample = std::make_shared<Example>();
		GlobalQueue.Push(pExample);
	}
}


void consume_thread()
{
	while (true)
	{
		std::shared_ptr<Example> pExample = nullptr;
		if (GlobalQueue.WaitAndPop(pExample))
		{

		}
	}
}


int main()
{
	std::thread t1(produce_thread);
	std::thread t2(consume_thread);

	t1.join();
	t2.join();

	return 0;
}