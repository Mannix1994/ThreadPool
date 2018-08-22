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
        _continue_run = true;
        _running_thread = 0;
        killed = false;

        auto caller = [=](){
            while (true){
                _sem->wait();
                _running_thread++;
                if(!_continue_run){
                    break;
                }
                _mutex.lock();
                std::function<void ()> f = _funcs.front();
                _funcs.pop();
                _mutex.unlock();
                f();
                _running_thread--;
            }
        };
        for(int i=0;i<_max_thread_count;i++){
            _threads.emplace_back(std::thread(caller));
        }
    }

    ~ThreadPool(){
        kill();
        delete _sem;
        _sem = nullptr;
    }

    void submit(std::function<void ()> const &f){
        _mutex.lock();
        _funcs.push(f);
        _mutex.unlock();
        _sem->signal();
    }

    void join(){
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        while (_running_thread>0){
            std::this_thread::sleep_for(std::chrono::microseconds(50));
        }
    }

    void kill(){
        if(killed){
            return;
        }
        killed = true;
        _continue_run = false;
        for(int i =0;i<_max_thread_count;i++){
            _sem->signal();
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
    SEM::Semaphore *_sem;

    std::queue<std::function<void ()> > _funcs;
    std::mutex _mutex;
    std::atomic_bool _continue_run;
    std::atomic_uint _running_thread;
    bool killed;
};

#endif //THREADPOOL_H
