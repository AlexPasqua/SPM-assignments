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

// The queue code itself. It is defined as a template such that we can defined queues with different item types:
template <typename T>
class myqueue
{
private:
  std::mutex              d_mutex;
  std::condition_variable d_condition;
  std::deque<T>           d_queue;
public:

  myqueue(std::string s) { std::cout << "Created " << s << " queue " << std::endl;  }
  myqueue() {}
  
  void push(T const& value) {
    {
      std::unique_lock<std::mutex> lock(d_mutex);
      d_queue.push_front(value);
    }
    this->d_condition.notify_one();
  }
  
  T pop() {
    std::unique_lock<std::mutex> lock(d_mutex);
    d_condition.wait(lock, [=]{ return !d_queue.empty(); });  // wait if the queue is currently empty
    T rc(std::move(this->d_queue.back()));       // used to get the actual message rather than a copy
    d_queue.pop_back();                   // remove the item just read from the back end of the queue
    return rc;
  }
};

// definition of end of stream
#define EOS -1

// Drain stage definition
using namespace std::chrono_literals;
void drain(myqueue<int> &q, std::chrono::milliseconds msecs) {
  std::cout << "Drain started" << std::endl;
  auto e = q.pop();
  
  while(e != EOS) {
    std::this_thread::sleep_for(msecs);
    std::cout << "received " << e << std::endl;
    e = q.pop();
  }
    
  return;
}

// Source stage definition
void source(myqueue<int> &q, int n, std::chrono::milliseconds msecs) {
    for(int i=0; i<n; i++){
        std::this_thread::sleep_for(msecs);
        q.push(i);
        std::cout << "Drain emitted " << i << std::endl; 
    }
    q.push(EOS);
    std::cout << "sent EOS" << std::endl;
    return;
}

// Generic stage definition (computes int -> int function)
void genericstage(myqueue<int> &inq, myqueue<int> &outq, std::chrono::milliseconds msecs) {
  auto e = inq.pop();
  
  while(e != EOS) {
    std::this_thread::sleep_for(msecs);
    auto res = e+1;
    outq.push(res);
    e = inq.pop();
  }
  outq.push(EOS);
  return;
}


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
  myqueue<int> q1, q2, q3;
  int taskNo = 10;
  std::chrono::milliseconds msecs_src = 100ms;
  std::chrono::milliseconds msecs_s1 = 200ms;
  std::chrono::milliseconds msecs_s2 = 150ms;
  std::chrono::milliseconds msecs_drn = 50ms;

  {
    utimer my_timer("my timer");
    std::thread src(source, std::ref(q1), taskNo, msecs_src);
    std::thread stg1(genericstage, std::ref(q1), std::ref(q2), msecs_s1);
    std::thread stg2(genericstage, std::ref(q2), std::ref(q3), msecs_s2);
    std::thread drn(drain, std::ref(q3), msecs_drn);
    src.join();
    stg1.join();
    stg2.join();
    drn.join();
  }
  
  cout << "Theoretical time: " << \
    completionTime(taskNo, msecs_src, msecs_s1, msecs_s2, msecs_drn).count() << '\n';

  return 0;
}