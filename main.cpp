#include <iostream>
#include "ThreadPool.h"
#include "timer.h"
#include <functional>
#include <typeinfo>

int main() {
    ThreadPool tp(10);
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

    for(int i=2;i<10;i++){
        auto ff = [=](){
            std::this_thread::sleep_for(std::chrono::milliseconds(i*200));
            printf("id:%d\n",i);
        };
        tp.submit(ff);
    }
    tp.join();
	std::this_thread::sleep_for(std::chrono::milliseconds(5 * 200));
    printf("tp.join();\n");

    ////////////////////////////////////////////////////////
    auto task = [](){
        std::this_thread::sleep_for(std::chrono::seconds(2));
    };
    tp.submit(task);
    tp.join();
    printf("tp.join();\n");

    ////////////////////////////////////////////////////////
    int *a = new int[20];
    auto task0 = [=](){
        for (int i=0;i<10;i++){
            a[i] = i * 10;
        }
    };
    tp.submit(task0);

    auto task1 = [=](){
        for (int i=10;i<20;i++){
            a[i] = i * (-1);
        }
    };
    tp.submit(task1);
    tp.join();
    printf("tp.join();\n");

    //! must read object a after call tp.join()
    for (int i=0;i<20;i++){
        std::cout<<a[i]<<" ";
    }
    std::cout<<std::endl;
    delete[] a;
    a = nullptr;

    std::cout << "Hello, World!" << std::endl;

    tp.kill();
    return 0;
}