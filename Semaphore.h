/**
 * this is a semaphore, based on C++11.
 * I will write some Chinese comments in this file, if you don't understand,
 * that's ok, you can look the code directly.
 */
#ifndef SEMAPHORE_H
#define SEMAPHORE_H
#include <mutex>
#include <condition_variable>
#include <string>
#include <cassert>
#include <chrono>
#include <iostream>

namespace SEM {
    class Semaphore {
    public:
        explicit Semaphore(std::string name, int value = 0) : _name(std::move(name)), _count(value) {}

        Semaphore(const Semaphore &semaphore) = delete;
        Semaphore &operator=(const Semaphore &semaphore) = delete;

        /**
         * P operation.
         * if --_count is bigger than 0, it will return immediately.
         * if --_count is smaller than 0 or equal with 0, it will block
         * the thread which call this function until a signal() or
         * signalAll() is called.
         * _count大于0(有资源)时,函数会立即返回,否则会阻塞调用此函数的线程.
         */
        inline void wait() {
            std::unique_lock<std::mutex> lock(_mutex);
            //std::cout<<_name+":"<<_count<<std::endl;
            if (--_count < 0) { // 资源不够
                _condition.wait(lock); // 阻塞
            }
        }

        /**
         * P operation.
         * if --_count is bigger than 0, it will return immediately.
         * if --_count is smaller than 0 or equal with 0, it will block
         * the thread which call this function until a signal(),signalAll()
         * is called or after ms microseconds.
         * After waiting ms microseconds, this function will return.
         * _count大于0(有资源)时,函数会立即返回,否则会阻塞调用此函数的线程.
         * 但如果等待时间超过seconds指定的值，会唤醒所有阻塞线程.
         * @param ms 等待时间(ms)
         */
        inline void wait(int ms) {
            assert(ms > 0);
            std::unique_lock<std::mutex> lock(_mutex);
            //std::cout<<_name+":"<<_count<<std::endl;
            if (--_count < 0) {                            // 资源不够
                std::cv_status status
                        = _condition.wait_for(lock, std::chrono::milliseconds(ms));
                if (status == std::cv_status::timeout)   // 超时
                {
                    std::cout << _name + ":timeout" << std::endl;
                    _count = 0;
                    _condition.notify_all();//唤醒所有阻塞线程
                }
            }
        }

        /**
         * V Operation.
         * weak one thread which blocked on _condition
         * 如果有阻塞的线程,则随机唤醒一个线程，相当于信号量机制里面的V操作.否则
         * 立即返回.
         */
        inline void signal() {
            std::lock_guard<std::mutex> lock(_mutex);
            if (++_count <= 0) {
                _condition.notify_one();
            }
        }

        /**
         * V Operation.
         * weak all threads which blocked on _condition
         * 如果有线程阻塞,唤醒阻塞的所有线程;否则立即返回.
         */
        inline void signalAll() {
            std::lock_guard<std::mutex> lock(_mutex);
            while (++_count <= 0) {
                _condition.notify_one();
            }
            _count = 0;
        }

        /**
         * name of semaphore
         * 返回这个信号量的名字
         * @return 名字
         */
        inline std::string name(){
            return _name;
        }

    private:
        int _count;                             //等待线程数量
        std::mutex _mutex;                      //互斥锁
        std::condition_variable _condition;     //条件变量
        std::string _name;                      //信号量名字
    };
};
#endif