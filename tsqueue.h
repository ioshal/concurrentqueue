//
// Created by Igor on 18.02.2021.
//

#ifndef THREADSAFEQUEUE_TSQUEUE_H
#define THREADSAFEQUEUE_TSQUEUE_H

#include <queue>
#include <mutex>
#include <iostream>
#include <chrono>
#include <condition_variable>

enum class OpStatus {
    OK,
    EMPTY
};

template<class Element>
class tsqueue final {
public:
    tsqueue() = default;
    tsqueue(std::initializer_list<Element>);

    void push(const Element &);
    void push(Element &&);

    void pop(Element&);
    void pop();

    OpStatus tryPop();

    void tryPopFor(std::chrono::seconds);

    [[nodiscard]] size_t size();
    [[nodiscard]] bool empty();
private:
    std::mutex _mutex;
    std::condition_variable _queueEmptyCV;
    std::queue<Element> _queue;
};

template<class Element>
void tsqueue<Element>::push(const Element &el) {
    std::unique_lock<std::mutex> lock(_mutex);

    const bool wasEmpty = _queue.empty();
    _queue.push(el);

    lock.unlock();

    if (wasEmpty) {
        _queueEmptyCV.notify_one();
    }
}

template<class Element>
void tsqueue<Element>::push(Element &&el) {
    std::unique_lock<std::mutex> lock(_mutex);

    const bool wasEmpty = _queue.empty();
    _queue.push(std::move(el));

    lock.unlock();

    if (wasEmpty) {
        _queueEmptyCV.notify_one();
    }
}

template<class Element>
size_t tsqueue<Element>::size() {
    std::scoped_lock<std::mutex> lock(_mutex);

    return _queue.size();
}

template<class Element>
void tsqueue<Element>::pop(Element& popped) {
    std::unique_lock<std::mutex> lock(_mutex);

    _queueEmptyCV.wait(lock, [this](){ return !_queue.empty(); });

    popped = _queue.front();

    _queue.pop();
}


template<class Element>
void tsqueue<Element>::pop() {
    std::unique_lock<std::mutex> lock(_mutex);

    _queueEmptyCV.wait(lock, [this](){ return !_queue.empty(); });

    _queue.pop();
}

template<class Element>
void tsqueue<Element>::tryPopFor(std::chrono::seconds seconds) {
    std::unique_lock<std::mutex> lock(_mutex);

    if (!_queueEmptyCV.wait_for(lock, seconds, [this](){ return !_queue.empty(); })) {
        return;
    }

    _queue.pop();
}

template<class Element>
OpStatus tsqueue<Element>::tryPop() {
    std::unique_lock<std::mutex> lock(_mutex);

    if (_queue.empty()) {
        return OpStatus::EMPTY;
    }
    _queue.pop();

    return OpStatus::OK;
}

template<class Element>
bool tsqueue<Element>::empty() {
    std::scoped_lock<std::mutex> lock(_mutex);

    return _queue.empty();
}

template<class Element>
tsqueue<Element>::tsqueue(std::initializer_list<Element> elements) {
    for (const auto& el : elements) {
        _queue.push(el);
    }
}


#endif //THREADSAFEQUEUE_TSQUEUE_H
