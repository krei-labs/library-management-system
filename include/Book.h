#pragma once
#include <string>

// Plain data structure (struct) representing one library book.
// Kept as a simple aggregate so it can be stored in arrays, vectors,
// linked-list nodes, and serialized to/from a file with equal ease.
struct Book {
    int id;
    std::string title;
    std::string author;
    int year;
    bool isBorrowed;
    std::string borrowedBy; // member name; empty string if not borrowed
};
