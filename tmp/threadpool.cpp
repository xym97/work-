#include"threadpool.h"

void* threadpool::task_func( void* arg)
{
  threadpool* pool = static_cast<threadpool*>(arg);
	while (pool->ifruning)
  {
		Task task = pool->take();
    if (task.empty())
		{
			printf("no task\n");
			break;
	  }
    assert(!task.empty());
		task.run_func(task.arg);
	}
  return 0;
}

threadpool::threadpool(int nums)
{
	ifruning = true;
  create_pool(nums);
}

int threadpool::create_pool(int nums)
{
	for (auto i = 0 ; i < nums; ++i)
	{
		threads.emplace_back(std::thread(threadpool::task_func ,this));
	}
  return 0;
}

threadpool::~threadpool()
{
  stop();
}

void threadpool::stop()
{
	if (!ifruning)
	{
		return;
	}
	ifruning = false;
	this->_cond.notify_all();
	for (size_t i = 0; i <  threads.size(); ++i)
	{
		if (threads[i].joinable()) threads[i].join();
	}
}

size_t threadpool::size()
{
  std::lock_guard<std::mutex> loc_g(_mut);
	return tasks.size();
}


Task  threadpool::take()
{
  Task task ;
	{
		std::unique_lock<std::mutex> unilock(_mut);  //任务队列是临界资源，必须加锁
		while (tasks.empty() && ifruning)
		{
			this->_cond.wait(unilock);        //释放锁并继续等待
		}
		if (ifruning == false)                
		{
			unilock.unlock();                 //释放锁直接返回
			return task;
		}
		assert(!tasks.empty());
		task = tasks.front();
		tasks.pop_front();
	}
	return task;
}

int threadpool::addtask( Task& task)
{
  int size = 0;
  {
    std::lock_guard<std::mutex> loc_g(_mut);
	  tasks.push_back(task);
    size = tasks.size();
  }
  _cond.notify_one();
	return size;
}

//binvoid threadpool::thread_pool_deal_with_data(Task task ,  void* arg)
//{
//  this->addtask(task);
  //if(0 == size)
  //{
  //  std::cout << "add error"  <<std::endl;
  //}
//  this->task_func(arg);
//}



