//
// Created by creator on 18-8-19.
//

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <thread>
#include <vector>
#include <chrono>
#include <functional>
#include <queue>
#include <atomic>
#include "Semaphore.h"

class ThreadPool{
public:
    explicit ThreadPool(unsigned thread_count){
        unsigned max_thread_count = std::thread::hardware_concurrency();
        if(thread_count>max_thread_count){
			fprintf(stderr, "thread_count is bigger than the number of CPU cores %u\n", max_thread_count);
            _max_thread_count = max_thread_count;
        }else
            _max_thread_count = thread_count;
        _sem_more_task = new SEM::Semaphore("nick",0);
		_sem_join = new SEM::Semaphore("nick", 0);
        _continue_run = true;
        killed = false;

        auto caller = [=](){
			while (true){
				while (true){
					_sem_more_task->wait();
					if (!_continue_run){
						return;
					}
					_mutex.lock();
					std::function<bool ()> f = _funcs.front();
					_funcs.pop();
					_mutex.unlock();
					bool tag = f();
					if (!tag)
						break;
				}
				//printf("_sem_join->signal();\n");
				_sem_join->signal();
			}
        };
        for(unsigned i=0;i<_max_thread_count;i++){
            _threads.emplace_back(std::thread(caller));
        }
    }

    ~ThreadPool(){
        kill();
        delete _sem_more_task;
        _sem_more_task = nullptr;
		delete _sem_join;
		_sem_join = nullptr;
    }

    void submit(std::function<void ()> const &f){
		auto task = [f]()->bool{
			f();
			return true;
		};
        _mutex.lock();
        _funcs.push(task);
        _mutex.unlock();
        _sem_more_task->signal();
    }

    void join(){
		auto task = []()->bool{
			return false;
		};
		for (unsigned i = 0; i<_max_thread_count; i++){
			_mutex.lock();
			_funcs.push(task);
			_mutex.unlock();
			_sem_more_task->signal();
		}
		for (unsigned i = 0; i<_max_thread_count; i++){
			_sem_join->wait();
			//printf("_sem_join->wait()\n");
		}
    }

    void kill(){
        if(killed){
            return;
        }
        killed = true;
        _continue_run = false;
        for(unsigned i =0;i<_max_thread_count;i++){
            _sem_more_task->signal();
        }
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        for(auto& t :_threads)
        {
            if(t.joinable()){
                t.join();
            }
        }
        _threads.clear();
    }

private:
    std::vector<std::thread> _threads;
    unsigned _max_thread_count;
    SEM::Semaphore *_sem_more_task;
	SEM::Semaphore *_sem_join;

    std::queue<std::function<bool ()> > _funcs;
    std::mutex _mutex;
    std::atomic_bool _continue_run;
    bool killed;
};

#endif //THREADPOOL_H
