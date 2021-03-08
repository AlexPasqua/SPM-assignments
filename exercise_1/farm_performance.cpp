#include <iostream>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <vector>
#include <chrono>
#include <cstddef>
#include <math.h>
#include <string>
#include <functional>
#include <thread>

#include "aux_src/myqueue.cpp"
#include "aux_src/utimer.cpp"
#include "farm_of_pipelines.cpp"

// -------------------------------------------------------------------------------------
// Exercise

// Use combinations of generic stage to set up a farm of 2 stage pipelines: Vary
// the amount of delay given to the stages (source, generic and drain)
// Measure the time spent (from start to end) and figure out if the
// formula given for completion time is exact.

auto completionTime(int m, int nw, 
                     std::chrono::milliseconds temit, std::chrono::milliseconds tinc1, 
                     std::chrono::milliseconds tinc2, std::chrono::milliseconds tdrain) {
    auto ts = max ( max(temit,tdrain), (tinc1+tinc2)/nw);
    auto sum = temit + tinc1 + tinc2 + tdrain;
    auto tc = sum + (m-1) * ts;
    return tc;
}

using namespace std;
int main() {
    int taskNo = 10;
    int nw = 4;
    std::chrono::milliseconds temit = 100ms;
    std::chrono::milliseconds tinc1 = 200ms;
    std::chrono::milliseconds tinc2 = 150ms;
    std::chrono::milliseconds tdrain = 50ms;

    // farm of pipelines
    cout << "Farm of pipelines" << endl;
    {
        utimer my_timer("my timer");
        farm_of_pipelines(taskNo, 4, temit, tinc1, tinc2, tdrain);
    }
    cout << "Theoretical time: " << \
        completionTime(taskNo, nw, temit, tinc1, tinc2, tdrain).count() << " mesc" << endl;
    
    return 0;
}