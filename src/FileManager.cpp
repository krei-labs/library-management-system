#include "FileManager.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include <iostream>

bool FileManager::saveBooks(const std::string& filename, const std::vector<Book>& books) {
    std::ofstream out(filename.c_str());
    if (!out) {
        std::cerr << "Error: could not open " << filename << " for writing.\n";
        return false;
    }

    for (const auto& b : books) {
        out << b.id << "|" << b.title << "|" << b.author << "|" << b.year
            << "|" << (b.isBorrowed ? 1 : 0) << "|" << b.borrowedBy << "\n";
    }
    return true;
}

std::vector<Book> FileManager::loadBooks(const std::string& filename) {
    std::vector<Book> books;
    std::ifstream in(filename.c_str());
    if (!in) return books; // file doesn't exist yet -> empty catalog, not an error

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string field;
        Book b{};

        std::getline(ss, field, '|'); b.id = std::atoi(field.c_str());
        std::getline(ss, field, '|'); b.title = field;
        std::getline(ss, field, '|'); b.author = field;
        std::getline(ss, field, '|'); b.year = std::atoi(field.c_str());
        std::getline(ss, field, '|'); b.isBorrowed = (field == "1");
        std::getline(ss, field, '|'); b.borrowedBy = field;

        books.push_back(b);
    }
    return books;
}

bool FileManager::saveUsers(const std::string& filename, const std::vector<std::unique_ptr<User>>& users) {
    std::ofstream out(filename.c_str());
    if (!out) {
        std::cerr << "Error: could not open " << filename << " for writing.\n";
        return false;
    }

    for (const auto& u : users) {
        out << u->role() << "|" << u->getId() << "|" << u->getName()
            << "|" << u->getSalt() << "|" << u->getPasswordHash() << "\n";
    }
    return true;
}

std::vector<std::unique_ptr<User>> FileManager::loadUsers(const std::string& filename) {
    std::vector<std::unique_ptr<User>> users;
    std::ifstream in(filename.c_str());
    if (!in) return users;

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string role, idStr, name, salt, hash;

        std::getline(ss, role, '|');
        std::getline(ss, idStr, '|');
        std::getline(ss, name, '|');
        std::getline(ss, salt, '|');
        std::getline(ss, hash, '|');

        int id = std::atoi(idStr.c_str());

        // Simple factory: reconstruct the correct polymorphic subtype
        // based on the role string stored in the file.
        if (role == "Librarian") {
            users.push_back(std::make_unique<Librarian>(id, name, salt, hash));
        } else {
            users.push_back(std::make_unique<Member>(id, name, salt, hash));
        }
    }
    return users;
}

void FileManager::appendLog(const std::string& filename, const std::string& message) {
    std::ofstream out(filename.c_str(), std::ios::app); // append mode: never overwrites history
    if (!out) return; // logging is best-effort; don't crash the app over it

    std::time_t now = std::time(nullptr);
    std::string ts = std::ctime(&now); // e.g. "Wed Sep 09 12:34:56 2026\n"
    if (!ts.empty() && ts.back() == '\n') ts.pop_back();

    out << "[" << ts << "] " << message << "\n";
}
