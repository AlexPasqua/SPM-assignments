#ifndef FARMOFPIPES
#define FARMOFPIPES

#include "aux_src/myqueue.cpp"

using namespace std;
using namespace std::chrono;

void source_farm(vector<myqueue<int>> &q, int n, int nw, milliseconds t) {
    // push items (just a number) to queues
    short turn = 0;
    for (int i = 0; i < n; i++) {
        std::this_thread::sleep_for(t);
        q[turn].push(i);
        turn = (turn + 1) % nw;
        cout << "Source emitted " << i << endl;
    }

    // push EOS to queues
    for (int i = 0; i < nw; i++)
        q[i].push(EOS);
    
    cout << "Sent EOS" << endl;
    return;
}

void genericfarm(myqueue<int> &inq, myqueue<int> &outq, milliseconds t1, milliseconds t2) {
    auto e = inq.pop();
    while (e != EOS) {
        std::this_thread::sleep_for(t1);
        std::this_thread::sleep_for(t2);
        auto res = e + 2;
        outq.push(res);
        e = inq.pop();
    }
    outq.push(EOS);
    return;
}

void drain_farm(myqueue<int> &q, int nw, milliseconds t) {
    int count_EOSs = 0;
    do {
        std::this_thread::sleep_for(t);
        auto e = q.pop();
        count_EOSs += (e == EOS) ? 1 : 0;
        cout << "Received " << e << endl;
    } while (count_EOSs < nw);
    return;
}

void farm_of_pipelines(int n, int nw, milliseconds t0, milliseconds t1, milliseconds t2, milliseconds t3) {
    vector<myqueue<int>> queues(nw);
    myqueue<int> outq;
    
    std::thread src(source_farm, std::ref(queues), n, nw, t0);
    std::vector<std::thread*> tids(nw);
    for (int i = 0; i < nw; i++) {
        tids[i] = new std::thread(genericfarm, std::ref(queues[i]), std::ref(outq), t1, t2);
    }
    std::thread drn(drain_farm, std::ref(outq), nw, t3);

    src.join();
    for (int i = 0; i < nw; i++)
        tids[i]->join();
    drn.join();
}

#endif