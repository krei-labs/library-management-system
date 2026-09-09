#pragma once
#include <string>

// One recorded transaction, used to support "undo last action".
struct Action {
    std::string type;       // "BORROW" or "RETURN"
    int bookId;
    std::string memberName;
    std::string description;
};

// LIFO stack implemented from scratch with a dynamic array.
// Every borrow/return operation is pushed here so the last one can be undone.
class ActionStack {
private:
    Action* data;
    int capacity;
    int topIndex;

    void resize();

public:
    explicit ActionStack(int initialCapacity = 8);
    ~ActionStack();

    ActionStack(const ActionStack&) = delete;
    ActionStack& operator=(const ActionStack&) = delete;

    void push(const Action& action);
    Action pop();
    bool isEmpty() const;
    int size() const;
    void printHistory() const;
};
