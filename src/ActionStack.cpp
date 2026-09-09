#include "ActionStack.h"
#include <stdexcept>
#include <iostream>

ActionStack::ActionStack(int initialCapacity)
    : data(new Action[initialCapacity]), capacity(initialCapacity), topIndex(-1) {}

ActionStack::~ActionStack() {
    delete[] data;
}

void ActionStack::resize() {
    int newCapacity = capacity * 2;
    Action* newData = new Action[newCapacity];
    for (int i = 0; i <= topIndex; ++i) newData[i] = data[i];
    delete[] data;
    data = newData;
    capacity = newCapacity;
}

void ActionStack::push(const Action& action) {
    if (topIndex + 1 == capacity) resize();
    data[++topIndex] = action;
}

Action ActionStack::pop() {
    if (isEmpty()) throw std::runtime_error("ActionStack is empty: nothing to undo");
    return data[topIndex--];
}

bool ActionStack::isEmpty() const { return topIndex == -1; }
int ActionStack::size() const { return topIndex + 1; }

void ActionStack::printHistory() const {
    if (isEmpty()) {
        std::cout << "  (no actions recorded yet)\n";
        return;
    }
    for (int i = topIndex; i >= 0; --i) {
        std::cout << "  [" << (topIndex - i + 1) << "] " << data[i].description << "\n";
    }
}
