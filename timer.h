#ifndef W_TIMER_H
#define W_TIMER_H

#include <iostream>
#include <string>
#include <chrono>

class Timer {
public:

    explicit Timer() {
        _name = "Time elapsed:";
        restart();
    }

    explicit Timer(const std::string &name) {
        _name = name + ":";
        restart();
    }

    /**
    * restart timer
    */
    inline void restart() {
        _start_time = std::chrono::steady_clock::now();
    }

    /**
    * stop timing
    * @return elapsed time, the unit is ms
    */
    inline double elapsed(bool restart = false) {
        _end_time = std::chrono::steady_clock::now();
        std::chrono::duration<double> diff = _end_time-_start_time;
        if(restart)
            this->restart();
        return diff.count()*1000;
    }

    /**
     * print elapsed time with the unit of ms, then restart timer
     * @param tip prefix of time
     */
    void rlog(const std::string &tip){
        log(true,tip,true,false);
    }

    /**
    * print elapsed time
    * @param reset if true, restart timer
    * @param unit_ms uf true, the unit of time is ms.or false,the unit of time is s
    * @param tip prefix of time
    * @param kill is true, after print time, exit program
    */
    void log(bool reset = false, const std::string &tip = "",
             bool unit_ms = true, bool kill = false
    ) {
        if (unit_ms) {
            if (tip.length() > 0)
                std::cout << tip+":" << elapsed() << "ms" << std::endl;
            else
                std::cout << _name << elapsed() << "ms" << std::endl;
        } else {
            if (tip.length() > 0)
                std::cout << tip+":" << elapsed() / 1000.0 << "s" << std::endl;
            else
                std::cout << _name << elapsed() / 1000.0 << "s" << std::endl;
        }

        if (reset)
            this->restart();

        if (kill)
            exit(5);
    }


private:
    std::chrono::steady_clock::time_point _start_time;
    std::chrono::steady_clock::time_point _end_time;
    std::string _name;
}; // timer

#endif //W_TIMER_H
