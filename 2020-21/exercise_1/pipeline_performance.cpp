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

#include "../aux_src/myqueue.cpp"
#include "../aux_src/utimer.cpp"
#include "pipeline4stages.cpp"

// -------------------------------------------------------------------------------------
// Exercise

// Use combinations of generic stage to set up a 4 stage pipeline: Vary
// the amount of delay given to the stages (source, generic and drain)
// Measure the time spent (from start to end) and figure out if the
// formula given for completion time is exact.

// theoretical value
auto completionTime(int m, std::chrono::milliseconds t0, std::chrono::milliseconds t1,
                    std::chrono::milliseconds t2, std::chrono::milliseconds t3) {
    auto ts = std::max(std::max(t0, t1) ,std::max(t2, t3));
    auto sumt = t0 + t1 + t2 + t3;
    auto tc = sumt + (m-1) * ts; 
    return tc;
}

using namespace std;
int main(){
  // 4 stage pipeline
  cout << "4 stages pipeline" << endl;
  int taskNo = 10;
  std::chrono::milliseconds t0 = 100ms;
  std::chrono::milliseconds t1 = 200ms;
  std::chrono::milliseconds t2 = 150ms;
  std::chrono::milliseconds t3 = 50ms;
  {
    utimer my_timer("my timer");
    pipeline4stages(taskNo, t0, t1, t2, t3);
  }
  cout << "Theoretical time: " << \
    completionTime(taskNo, t0, t1, t2, t3).count() << " mesc" << endl;

  return 0;
}