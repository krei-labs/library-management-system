#include "ReservationQueue.h"
#include <stdexcept>
#include <iostream>

ReservationQueue::ReservationQueue(int initialCapacity)
    : data(new std::string[initialCapacity]), capacity(initialCapacity),
      frontIdx(0), count(0) {}

ReservationQueue::~ReservationQueue() {
    delete[] data;
}

void ReservationQueue::resize() {
    int newCapacity = capacity * 2;
    std::string* newData = new std::string[newCapacity];
    for (int i = 0; i < count; ++i) {
        newData[i] = data[(frontIdx + i) % capacity];
    }
    delete[] data;
    data = newData;
    capacity = newCapacity;
    frontIdx = 0;
}

void ReservationQueue::enqueue(const std::string& memberName) {
    if (count == capacity) resize();
    int backIdx = (frontIdx + count) % capacity;
    data[backIdx] = memberName;
    ++count;
}

std::string ReservationQueue::dequeue() {
    if (isEmpty()) throw std::runtime_error("ReservationQueue is empty");
    std::string value = data[frontIdx];
    frontIdx = (frontIdx + 1) % capacity;
    --count;
    return value;
}

bool ReservationQueue::isEmpty() const { return count == 0; }
int ReservationQueue::size() const { return count; }

void ReservationQueue::printAll() const {
    if (isEmpty()) {
        std::cout << "  (no one waiting)\n";
        return;
    }
    for (int i = 0; i < count; ++i) {
        std::cout << "  " << (i + 1) << ". " << data[(frontIdx + i) % capacity] << "\n";
    }
}
