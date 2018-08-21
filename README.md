# ThreadPool

this is a C++11 ThreadPool Project.


## How 2 use
include header ThreadPool.h
```
#include "ThreadPool.h"
```
Define a ThreadPool object tp
```
ThreadPool tp(12); //parameter 12 is the kernel number of your cpu
```
add a task to tp.  
```
auto task0 = [=](){//must no parameters
    //do something
}
tp.submit(task0);
```
add tasks
```
for(int i=11;i<20;i++){
    auto task = [=](){ // in for loop, must pass by value and no parameters
        // do something
    };
    tp.submit(task);
}
```
start and wait for finish
```
tp.join();
```
if you want modify a object, you should catch a pointer 
of the object
```
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

//! must read object a after call tp.join()
for (int i=0;i<20;i++){
    std::cout<<a[i]<<" ";
}
std::cout<<std::endl;
delete[] a;
a = nullptr;
```