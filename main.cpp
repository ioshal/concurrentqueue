#include <iostream>
#include <thread>
#include <vector>
#include <functional>
#include <chrono>

#include "tsqueue.h"

using namespace std;

int main() {
    tsqueue<int> q({1, 2, 3, 4, 5});

    vector<thread> workers;
    const int numOfWorkers = 5;
    const int numOfOperations = 100000;

    for (int i = 0; i < numOfWorkers; i++) {
        workers.push_back(thread([&]() {
            for (int j = 0; j < numOfOperations; j++) {
                q.push(j);
            }

            q.push(1337);
        }));
    }

    for (int i = 0; i < numOfWorkers; i++) {
        workers.push_back(thread([&]() {
            q.tryPop();

            for (int j = 0; j < numOfOperations; j++) {
                q.tryPopFor(chrono::seconds(2));
            }
        }));
    }

    for_each(workers.begin(), workers.end(), mem_fn(&thread::join));

    return 0;
}
