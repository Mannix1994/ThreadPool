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
#include "timer.h"

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
        _funcs.push(f);
        _sem->signal();
    }

    void join(){
//        Timer timer;
        auto caller = [&](){
            while (true){
//                _mutex.lock();
                bool empty = _funcs.empty();
//                _mutex.unlock();
                if(empty)
                    break;
                _sem->wait();
                _mutex.lock();
                std::function<void ()> f = _funcs.front();
                _funcs.pop();
                _mutex.unlock();
                f();
            }
        };
        for(int i=0;i<_max_thread_count;i++){
            _threads.emplace_back(std::thread(caller));
        }
//        timer.log();

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
};

#endif //THREADPOOL_H
