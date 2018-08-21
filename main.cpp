#include <iostream>
#include "ThreadPool.h"
#include "timer.h"
#include <functional>
#include <typeinfo>

int main() {
    ThreadPool tp(10);
    Timer timer;
    int  b = 0;
    auto f = [=](){
        std::this_thread::sleep_for(std::chrono::milliseconds(b*200));
        printf("id:%d\n",b);
    };
    tp.submit(f);
    b++;
    auto f1 = [=](){
        std::this_thread::sleep_for(std::chrono::milliseconds(b*200));
        printf("id:%d\n",b);
    };
    tp.submit(f1);

    for(int i=11;i<20;i++){
        auto ff = [=](){
            std::this_thread::sleep_for(std::chrono::milliseconds(i*200));
            printf("id:%d\n",i);
        };
        tp.submit(ff);
    }
    timer.log();
    tp.join();
    timer.log();
    std::cout << "Hello, World!" << std::endl;
    return 0;
}