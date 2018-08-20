#include <iostream>
#include "ThreadPool/ThreadPool.h"
#include "ThreadPool/timer_std.h"
#include <functional>

int main() {
    ThreadPool tp(10);
    Timer timer;
    int  b = 0;
    auto f = [=]()->void{
        std::this_thread::sleep_for(std::chrono::milliseconds(b*200));
        printf("id:%d\n",b);
    };
    tp.submit(f);
    b++;
    auto f1 = [=]()->void{
        std::this_thread::sleep_for(std::chrono::milliseconds(b*200));
        printf("id:%d\n",b);
    };
    tp.submit(f1);
    b++;
    auto f2 = [=]()->void{
        std::this_thread::sleep_for(std::chrono::milliseconds(b*200));
        printf("id:%d\n",b);
    };
    tp.submit(f2);
    b++;
    auto f3 = [=]()->void{
        std::this_thread::sleep_for(std::chrono::milliseconds(b*200));
        printf("id:%d\n",b);
    };
    tp.submit(f3);
    b++;
    auto f4 = [=]()->void{
        std::this_thread::sleep_for(std::chrono::milliseconds(b*200));
        printf("id:%d\n",b);
    };
    tp.submit(f4);
    timer.log();

    for(int i=11;i<20;i++){
        auto ff = [=](){
            int ii =i;
            std::this_thread::sleep_for(std::chrono::milliseconds(ii*200));
            printf("id:%d\n",ii);
        };
        tp.submit(ff);
    }
    timer.log();
    tp.join();
    timer.log();
    std::cout << "Hello, World!" << std::endl;
    return 0;
}