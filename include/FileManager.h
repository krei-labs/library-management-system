#pragma once
#include "Book.h"
#include "User.h"
#include <vector>
#include <string>
#include <memory>

// Handles persisting data to disk and reloading it on startup.
// Deliberately avoids <filesystem> (inconsistent support across older/
// alternate compilers such as Embarcadero's classic bcc32) and instead
// reads/writes plain files in the working directory using only
// <fstream>, which every standard-conforming C++ compiler supports.
class FileManager {
public:
    // Catalog persistence: pipe-delimited "id|title|author|year|isBorrowed|borrowedBy"
    static bool saveBooks(const std::string& filename, const std::vector<Book>& books);
    static std::vector<Book> loadBooks(const std::string& filename);

    // User persistence: pipe-delimited "role|id|name|salt|passwordHash"
    // (never plaintext — see PasswordUtil.h for the hashing approach used)
    static bool saveUsers(const std::string& filename, const std::vector<std::unique_ptr<User>>& users);
    static std::vector<std::unique_ptr<User>> loadUsers(const std::string& filename);

    // Append-only audit trail: each call adds one timestamped line.
    static void appendLog(const std::string& filename, const std::string& message);
};
