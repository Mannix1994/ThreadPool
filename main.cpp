#include <iostream>
#include "ThreadPool.h"
#include "timer.h"
#include <functional>
#include <typeinfo>

int main() {
    Timer timer;
    ThreadPool tp(12);
    timer.log();
    for(auto i=0;i<100;i++){
        auto task = [=](){
        };
        tp.submit(task);
    }
    tp.join();
    timer.log();
    return 0;
}