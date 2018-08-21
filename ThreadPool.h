//
// Created by creator on 18-8-19.
//

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <thread>
#include <vector>
#include <chrono>
#include <functional>
#include "Semaphore.h"

class ThreadPool{
public:
    explicit ThreadPool(unsigned thread_count){
        unsigned max_thread_count = std::thread::hardware_concurrency();
        if(thread_count>max_thread_count){
            fprintf(stderr,"thread_count超过CPU核心数\n");
            _max_count = max_thread_count;
        }
        _max_count = thread_count;
        _sem = new SEM::Semaphore("nick",_max_count);
        _max = new SEM::Semaphore("nick",_max_count);
    }

    ~ThreadPool(){
        delete _sem;
        _sem = nullptr;
        delete _max;
        _max = nullptr;
    }

//    template <typename F>
//    void submit(F const &f){
//        auto fun = [=]()->void{
//            _sem->wait();
//            f();
//            _sem->signal();
//        };
//        _threads.emplace_back(std::thread(fun));
//    }

    void submit(std::function<void ()> const &f){
        _max->wait();
        auto fun = [=]()->void{
            _sem->wait();
            f();
            _sem->signal();
            _max->signal();
        };
        _threads.emplace_back(std::thread(fun));
    }

    void join(){
        for (auto& t : _threads){
            if (t.joinable()){
                t.join();
            }
        }
        _threads.clear();
    }


private:
    std::vector<std::thread> _threads;
    unsigned _max_count;
    SEM::Semaphore *_sem;
    SEM::Semaphore *_max;
};

#endif //THREADPOOL_H
