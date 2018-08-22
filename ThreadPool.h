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
            fprintf(stderr,"thread_count超过CPU核心数\n");
            _max_thread_count = max_thread_count;
        }else
            _max_thread_count = thread_count;
        _sem = new SEM::Semaphore("nick",0);
    }

    ~ThreadPool(){
        delete _sem;
        _sem = nullptr;
    }

    void submit(std::function<void ()> const &f){
        auto task = [=]()->bool{
            f();
            return true;
        };
        _funcs.push(task);
        _sem->signal();
    }

    void join(){
        for(int i=0;i<_max_thread_count;i++){
            auto task = [=]()->bool{
                return false;
            };
            _funcs.push(task);
            _sem->signal();
        }

        auto caller = [&](){
            while (true){
                _sem->wait();
                _mutex.lock();
                std::function<bool ()> f = _funcs.front();
                _funcs.pop();
                _mutex.unlock();
                if(!f())
                    break;
            }
        };
        for(int i=0;i<_max_thread_count;i++){
            _threads.emplace_back(std::thread(caller));
        }

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
    SEM::Semaphore *_sem;

    std::queue<std::function<bool ()> > _funcs;
    std::mutex _mutex;
};

#endif //THREADPOOL_H
