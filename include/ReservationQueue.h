#pragma once
#include <string>

// FIFO queue implemented from scratch with a circular dynamic array.
// Each borrowed book gets its own ReservationQueue of waiting members.
class ReservationQueue {
private:
    std::string* data;
    int capacity;
    int frontIdx;
    int count;

    void resize();

public:
    explicit ReservationQueue(int initialCapacity = 4);
    ~ReservationQueue();

    // Non-copyable: manages raw heap memory manually.
    ReservationQueue(const ReservationQueue&) = delete;
    ReservationQueue& operator=(const ReservationQueue&) = delete;

    void enqueue(const std::string& memberName);
    std::string dequeue();
    bool isEmpty() const;
    int size() const;
    void printAll() const;
};
