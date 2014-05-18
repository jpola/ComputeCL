#ifndef _TIMER_H_
#define _TIMER_H_

#include<CL/cl.hpp>
#include <boost/compute/command_queue.hpp>

#include <cassert>
#include <vector>
#include <iostream>

#include <chrono>
#include <ctime>

using namespace boost;

namespace computecl
{

//TODO: command_queue.enqueue_barrier(wait_list) does not work.
// I'm not able to build event based CL timer;

class HTimer
{
public:
    HTimer()
    {
        _start = std::chrono::high_resolution_clock::now();
    }

    void start()
    {
        _start = std::chrono::high_resolution_clock::now();
    }

    double stop()
    {
        _stop = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>
                (_stop - _start).count();
    }

private:
        std::chrono::time_point<std::chrono::high_resolution_clock> _start;
        std::chrono::time_point<std::chrono::high_resolution_clock> _stop;

};

} //computecl
#endif //TIMER
