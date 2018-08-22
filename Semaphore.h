/**
 * this is a semaphore, bases on C++11
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
         * P operation
         * _count大于0(有资源)时,函数会立即返回,否则会阻塞调用此函数的线程.
         */
        void wait() {
            std::unique_lock<std::mutex> lock(_mutex);
            //std::cout<<_name+":"<<_count<<std::endl;
            if (--_count < 0) { // 资源不够
                _condition.wait(lock); // 阻塞
            }
        }

        /**
         * 相当于信号量机制里面的P操作.
         * _count大于0(有资源)时,函数会立即返回,否则会阻塞调用此函数的线程.
         * 但如果等待时间超过seconds指定的值，会唤醒所有阻塞线程.
         * @param ms 等待时间(ms)
         */
        void wait(int ms) {
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
         * 如果有阻塞的线程,则随机唤醒一个线程，相当于信号量机制里面的V操作.否则
         * 立即返回.
         */
        void signal() {
            std::lock_guard<std::mutex> lock(_mutex);
            if (++_count <= 0) {
                _condition.notify_one();
            }
        }

        /**
         * 如果有线程阻塞,唤醒阻塞的所有线程;否则立即返回.
         */
        void signalAll() {
            std::lock_guard<std::mutex> lock(_mutex);
            while (++_count <= 0) {
                _condition.notify_one();
            }
            _count = 0;
        }

        /**
         * 返回这个信号量的名字
         * @return 名字
         */
        std::string name(){
            return _name;
        }

//        /**
//         * 重载<<,输出信号量的信息
//         * @param out 输出流
//         * @param sem 信号量
//         * @return 输出流
//         */
//        friend std::ostream &operator<<(std::ostream &out,Semaphore &sem);

    private:
        int _count;                             //等待线程数量
        std::mutex _mutex;                      //互斥锁
        std::condition_variable _condition;     //条件变量
        std::string _name;                      //信号量名字
    };


//    std::ostream &operator<<(std::ostream &out, Semaphore &sem) {
//        out<<"Semaphore:"<<sem._name<<",Waiting threads count:"<<sem._count<<std::endl;
//        return out;
//    }
};
#endif