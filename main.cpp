#include <thread>
#include <vector>

#include "tsqueue.h"

using namespace std;

int main() {
    tsqueue<int> q;

    vector<thread> workers;
    const int numOfWorkers = 10;
    const int numOfOperations = 10000;

    for (int i = 0; i < numOfWorkers; i++) {
        workers.push_back(thread([&]() {
            for (int j = 0; j < numOfOperations; j++) {
                q.push(200);
            }
        }));
    }

    for (int i = 0; i < numOfWorkers; i++) {
        workers.push_back(thread([&]() {
            for (int j = 0; j < numOfOperations; j++) {
                q.tryPopFor(chrono::seconds(1));
            }
        }));
    }

    for_each(workers.begin(), workers.end(), mem_fn(&thread::join));

    return 0;
}
