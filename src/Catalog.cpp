#include "Catalog.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

Catalog::Catalog() : head(nullptr), count(0), nextId(1) {}

Catalog::~Catalog() {
    BookNode* curr = head;
    while (curr) {
        BookNode* next = curr->next;
        delete curr;
        curr = next;
    }
}

int Catalog::generateNextId() {
    return nextId++;
}

void Catalog::addBook(const Book& book) {
    BookNode* newNode = new BookNode(book);
    if (!head) {
        head = newNode;
    } else {
        BookNode* curr = head;
        while (curr->next) curr = curr->next;
        curr->next = newNode;
    }
    ++count;
    if (book.id >= nextId) nextId = book.id + 1;
}

bool Catalog::removeBook(int id) {
    BookNode* curr = head;
    BookNode* prev = nullptr;
    while (curr && curr->data.id != id) {
        prev = curr;
        curr = curr->next;
    }
    if (!curr) return false;
    if (!prev) head = curr->next;
    else prev->next = curr->next;
    delete curr;
    --count;
    return true;
}

BookNode* Catalog::findById(int id) const {
    BookNode* curr = head;
    while (curr) {
        if (curr->data.id == id) return curr;
        curr = curr->next;
    }
    return nullptr;
}

std::vector<Book> Catalog::toVector() const {
    std::vector<Book> result;
    result.reserve(count);
    BookNode* curr = head;
    while (curr) {
        result.push_back(curr->data);
        curr = curr->next;
    }
    return result;
}

void Catalog::rebuildFromVector(const std::vector<Book>& books) {
    // Free existing nodes first
    BookNode* curr = head;
    while (curr) {
        BookNode* next = curr->next;
        delete curr;
        curr = next;
    }
    head = nullptr;
    count = 0;

    for (const auto& b : books) {
        addBook(b);
    }
}

void Catalog::sortByTitle() {
    std::vector<Book> books = toVector();
    int n = static_cast<int>(books.size());
    for (int i = 0; i < n - 1; ++i) {
        bool swapped = false;
        for (int j = 0; j < n - i - 1; ++j) {
            if (books[j].title > books[j + 1].title) {
                std::swap(books[j], books[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
    rebuildFromVector(books);
}

void Catalog::sortById() {
    std::vector<Book> books = toVector();
    int n = static_cast<int>(books.size());
    for (int i = 0; i < n - 1; ++i) {
        bool swapped = false;
        for (int j = 0; j < n - i - 1; ++j) {
            if (books[j].id > books[j + 1].id) {
                std::swap(books[j], books[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
    rebuildFromVector(books);
}

void Catalog::merge(std::vector<Book>& books, int left, int mid, int right) {
    std::vector<Book> leftHalf(books.begin() + left, books.begin() + mid + 1);
    std::vector<Book> rightHalf(books.begin() + mid + 1, books.begin() + right + 1);

    size_t i = 0, j = 0;
    int k = left;
    while (i < leftHalf.size() && j < rightHalf.size()) {
        if (leftHalf[i].title <= rightHalf[j].title) {
            books[k++] = leftHalf[i++];
        } else {
            books[k++] = rightHalf[j++];
        }
    }
    while (i < leftHalf.size()) books[k++] = leftHalf[i++];
    while (j < rightHalf.size()) books[k++] = rightHalf[j++];
}

void Catalog::mergeSortHelper(std::vector<Book>& books, int left, int right) {
    if (left >= right) return; // base case: 0 or 1 element is already sorted
    int mid = left + (right - left) / 2;
    mergeSortHelper(books, left, mid);
    mergeSortHelper(books, mid + 1, right);
    merge(books, left, mid, right);
}

void Catalog::mergeSortByTitle() {
    std::vector<Book> books = toVector();
    if (books.size() > 1) {
        mergeSortHelper(books, 0, static_cast<int>(books.size()) - 1);
    }
    rebuildFromVector(books);
}

int Catalog::binarySearchById(int id) const {
    // Assumes the catalog is currently sorted by id (call sortById() first).
    std::vector<Book> books = toVector();
    int low = 0, high = static_cast<int>(books.size()) - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (books[mid].id == id) return mid;
        if (books[mid].id < id) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

void Catalog::printAll() const {
    if (isEmpty()) {
        std::cout << "(catalog is empty)\n";
        return;
    }
    std::cout << std::left
              << std::setw(5)  << "ID"
              << std::setw(28) << "Title"
              << std::setw(20) << "Author"
              << std::setw(6)  << "Year"
              << std::setw(10) << "Status" << "\n";
    std::cout << std::string(69, '-') << "\n";

    BookNode* curr = head;
    while (curr) {
        const Book& b = curr->data;
        std::cout << std::left
                  << std::setw(5)  << b.id
                  << std::setw(28) << b.title
                  << std::setw(20) << b.author
                  << std::setw(6)  << b.year
                  << std::setw(10) << (b.isBorrowed ? ("Borrowed(" + b.borrowedBy + ")") : "Available")
                  << "\n";
        curr = curr->next;
    }
}

int Catalog::size() const { return count; }
bool Catalog::isEmpty() const { return head == nullptr; }
