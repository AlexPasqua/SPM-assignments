#ifndef PIPELINE
#define PIPELINE

#include "aux_src/myqueue.cpp"

using namespace std::chrono_literals;
using namespace std::chrono;

// Drain stage definition
void drain_pipeline(myqueue<int> &q, std::chrono::milliseconds msecs) {
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
void source_pipeline(myqueue<int> &q, int n, std::chrono::milliseconds msecs) {
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

// Pipeline definition
void pipeline4stages(int taskNo, milliseconds t0, milliseconds t1, milliseconds t2, milliseconds t3) {
    // queues
    myqueue<int> q1, q2, q3;

    // stages of the pipeline
    std::thread s1(source_pipeline, std::ref(q1), taskNo, t0);
    std::thread s2(genericstage, std::ref(q1), std::ref(q2), t1);
    std::thread s3(genericstage, std::ref(q2), std::ref(q3), t2);
    std::thread s4(drain_pipeline, std::ref(q3), t3);

    s1.join(); s2.join(); s3.join(); s4.join();
}

#endif