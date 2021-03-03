#include <iostream>
#include <thread>
#include <vector>
#include <functional>

#include "tsqueue.h"

using namespace std;

int main() {
    tsqueue<int> q;
    vector<thread> workers;
    const int numOfWorkers = 10;

    for (int i = 0; i < numOfWorkers; i++) {
        workers.push_back(thread([&]() {
            for (int j = 0; j < 10; j++) {
                q.push(j);
            }

            q.pop();
            q.push(1337);
        }));
    }

    thread th1([&](){
        q.pop();
        q.pop();
        q.pop();
    });

    for_each(workers.begin(), workers.end(), mem_fn(&thread::join));

    th1.join();

    cout << q.size();

//    while (!q.empty()) {
//        int el;
//
//        q.pop(el);
//
//        cout << "[ " << el << " ] ";
//    }

    return 0;
}
