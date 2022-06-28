#include <stdio.h>
#include <iostream>
#include <thread>

using namespace std;

chrono::system_clock::time_point stop1;
chrono::system_clock::time_point start2;
chrono::system_clock::time_point stop2;

int f(){
    stop1 = chrono::system_clock::now();
    int k = 0;
    for (int i = 0; i < 100000; i++) {k += i;}
    start2 = chrono::system_clock::now();
    return k;
}


int main(){
    const uint16_t n_trials = 100;
    for (uint8_t i = 0; i < n_trials; i++) {
        chrono::system_clock::time_point start1 = chrono::system_clock::now();
        thread t1(f);
        t1.join();
        stop2 = chrono::system_clock::now();
        chrono::system_clock::duration elapsed = stop2 - start2 + stop1 - start1;
        cout << elapsed.count() << endl;
    }
}