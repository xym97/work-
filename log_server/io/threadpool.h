#include<deque>
#include<vector>
#include<thread>
#include<assert.h>
#include<mutex>
#include<atomic>
#include<condition_variable>
#include<type_traits>
#include<functional>


typedef void* (*THREAD_FUNC)(void*);

struct Task
{
  Task():run_func(nullptr),arg(0){}
 
  Task(THREAD_FUNC fun, void* _arg)
    :run_func(fun)
    ,arg(_arg)
  {}
  bool empty()
  {
    return run_func == nullptr && arg == 0;
  }
  THREAD_FUNC  run_func;
  void* arg;
};

class threadpool
{
public:
	//typedef std::function<void*(void*)> Task;
	void  thread_pool_deal_with_data(Task task , void* arg);
  threadpool(int threadnum);
	~threadpool();
	
public:
	Task	 take();
	int		 addtask( Task& task);
	static void*  task_func(void* arg);
	void     stop();
	size_t size();
	
private:
	threadpool& operator = (const threadpool& th) = delete;
	threadpool(const threadpool& th) = delete ;
  int create_pool(int nums);

  std::atomic<bool> ifruning;           //线程是否正在执行
  std::vector<std::thread> threads;     //线程池中的线程
	std::deque<Task>  tasks;              //任务队列中的任务
	std::mutex _mut;                      //互斥锁
  std::condition_variable _cond;        //条件变量
};





















