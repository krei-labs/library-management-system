#pragma once
#include "Book.h"
#include <vector>

// Node of a singly linked list -> demonstrates LINKED LIST data structure.
struct BookNode {
    Book data;
    BookNode* next;
    explicit BookNode(const Book& b) : data(b), next(nullptr) {}
};

// The Catalog stores every book in the library as a hand-built linked
// list (not std::list), and also demonstrates classic ALGORITHMS
// (bubble sort, linear search, binary search) by working on a vector
// snapshot of the same data.
class Catalog {
private:
    BookNode* head;
    int count;
    int nextId;

public:
    Catalog();
    ~Catalog();

    // Non-copyable: this class manages raw heap memory manually.
    Catalog(const Catalog&) = delete;
    Catalog& operator=(const Catalog&) = delete;

    int generateNextId();

    void addBook(const Book& book);          // O(n) append to tail
    bool removeBook(int id);                  // O(n) unlink by id
    BookNode* findById(int id) const;         // LINEAR SEARCH, O(n)

    std::vector<Book> toVector() const;       // snapshot for sorting/saving
    void rebuildFromVector(const std::vector<Book>& books); // rebuild list

    void sortByTitle();                       // BUBBLE SORT by title, O(n^2)
    void sortById();                          // BUBBLE SORT by id, O(n^2)
    void mergeSortByTitle();                  // MERGE SORT by title, O(n log n)
    int binarySearchById(int id) const;       // BINARY SEARCH, requires sorted-by-id

    void printAll() const;
    int size() const;
    bool isEmpty() const;

private:
    static void mergeSortHelper(std::vector<Book>& books, int left, int right);
    static void merge(std::vector<Book>& books, int left, int mid, int right);
};
