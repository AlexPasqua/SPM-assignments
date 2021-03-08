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

#include "myqueue.cpp"
#include "pipeline4stages.cpp"


// -------------------------------------------------------------------------------------
// Exercise

// Use combinations of generic stage to set up a two stage pipeline: * Vary
// the amount of delay given to the stages (source, generic and drain) *
// Measure the time spent (from start to end) and * figure out if the
// formula given for completion time is exact.

class utimer {
  std::chrono::system_clock::time_point start;
  std::chrono::system_clock::time_point stop;
  std::string message; 
  using usecs = std::chrono::microseconds;
  using msecs = std::chrono::milliseconds;

private:
  long * us_elapsed;
  
public:

  utimer(const std::string m) : message(m), us_elapsed((long *)NULL) {
    start = std::chrono::system_clock::now();
  }
    
  utimer(const std::string m, long * us) : message(m), us_elapsed(us) {
    start = std::chrono::system_clock::now();
  }

  ~utimer() {
    stop = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = stop - start;
    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    
    std::cout << message << " computed in " << musec << " usec " << std::endl;
    if(us_elapsed != NULL)
      (*us_elapsed) = musec;
  }
};

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
    completionTime(taskNo, t0, t1, t2, t3).count() << " mesc\n";

  return 0;
}