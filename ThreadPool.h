/**
 * This is a thread pool class, based on C++11
 * In this class, background threads will be created and call task
 * in a queue continuously until function kill() is called.
 * I will write some Chinese comments in this file, if you don't
 * understand,that's ok, you can look the code directly.
 */

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <thread>
#include <vector>
#include <chrono>
#include <functional>
#include <queue>
#include <atomic>
#include "Semaphore.h"

class ThreadPool{
public:
    /**
     * create thread_count threads and initialize semaphores
     * 新建thread_count个线程和初始化信号量
     * @param thread_count 新建的线程数量
     */
    explicit ThreadPool(unsigned thread_count):_continue_run(true),killed(false)
    {
        // get the max concurrent thread
        unsigned max_thread_count = std::thread::hardware_concurrency();
        if(thread_count>max_thread_count){
			fprintf(stderr, "thread_count is bigger than the number of CPU cores: %u\n", max_thread_count);
            _max_thread_count = max_thread_count;
        }else
            _max_thread_count = thread_count;

        // initialize semaphores
        _sem_more_task = new SEM::Semaphore("a name",0);    // task semaphore
		_sem_sync = new SEM::Semaphore("a name", 0);        // sync semaphore

        for (unsigned i = 0; i<_max_thread_count; i++){
           _sem_for_every_thread.emplace_back(new SEM::Semaphore("i",0));   //one semaphore for one thread
        }

        auto caller = [=](unsigned m_id){
            while (true){
                // wait task
                _sem_more_task->wait();
                // if _continue_run is false, this thread will exit
                if (!_continue_run){
                    return;
                }
                // get a task from task queue
                _mutex_for_task.lock();
                std::function<bool ()> task = _tasks.front();
                _tasks.pop();
                _mutex_for_task.unlock();
                // run task
                bool is_empty_task = task();
                // if task is a empty task
                if (is_empty_task){
                    _sem_sync->signal();
                    // block this thread
                    _sem_for_every_thread[m_id]->wait();
                }
            }
        };
        // create _max_thread_count threads
        for(unsigned i=0;i<_max_thread_count;i++){
            auto proxy = [=](){
                caller(i);
            };
            _threads.emplace_back(std::thread(proxy));
        }
    }

    /**
     * submit a task
     * 提交一个任务
     * @param task a lambda expression without return or
     * a std::function<void ()> object.
     */
    void submit(std::function<void ()> const &task){
		auto proxy = [task]()-> bool {
			task();
			return false;// task pushed by submit() is not a empty task
		};
        _mutex_for_task.lock();
        _tasks.push(proxy);
        _mutex_for_task.unlock();
        _sem_more_task->signal();
    }

    /**
     * wait until all submitted task finished.
     * 等待所有提交的任务完成.
     */
    void join(){
        auto task = []()-> bool {
            return true;// task pushed by join() is a empty task
        };
        // push _max_thread_count empty task to task queue, when
        // a thread execute a empty task, it will block itself.
        _mutex_for_task.lock();
        for (unsigned i = 0; i<_max_thread_count; i++){

			_tasks.emplace(task);
			_sem_more_task->signal();
		}
        _mutex_for_task.unlock();

        // wait until all the thread execute a empty task and
        // call _sem_sync->signal().
        for (unsigned i = 0; i<_max_thread_count; i++){
			_sem_sync->wait();
		}

        // weak all the blocked threads
        for (unsigned i = 0; i<_max_thread_count; i++){
            _sem_for_every_thread[i]->signal();
        }
    }

    /**
     * kill all the threads
     */
    void kill(){
        // if kill() is called, return
        if(killed){
            return;
        }
        killed = true;
        // make _continue_run to false, then call _sem_more_task->signal()
        // all threads will exit.
        _continue_run = false;
        for(unsigned i =0;i<_max_thread_count;i++){
            _sem_more_task->signal();
        }

        // wait all threads exit
        for(auto& t :_threads)
        {
            if(t.joinable()){
                t.join();
            }
        }
        _threads.clear();
    }

    ~ThreadPool(){
        // kill all threads
        kill();
        // release semaphores
        for(auto& sem : _sem_for_every_thread){
            delete sem;
            sem = nullptr;
        }
        delete _sem_more_task;
        _sem_more_task = nullptr;
        delete _sem_sync;
        _sem_sync = nullptr;
    }

private:
    unsigned _max_thread_count;
    std::vector<std::thread> _threads;
    std::vector<SEM::Semaphore *> _sem_for_every_thread;
    SEM::Semaphore *_sem_more_task;
	SEM::Semaphore *_sem_sync;

    std::queue<std::function<bool ()> > _tasks;
    std::mutex _mutex_for_task;
    std::atomic_bool _continue_run;
    bool killed;
};

#endif //THREAD_POOL_H
