#include "LibrarySystem.h"
#include "FileManager.h"
#include "PasswordUtil.h"
#include <iostream>
#include <limits>
#include <algorithm>

namespace {

void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Reads an integer safely, re-prompting on invalid (non-numeric) input
// instead of leaving std::cin in a broken state or crashing. On success,
// it also discards the trailing newline so a getline() call right after
// this doesn't read an empty leftover line.
int readInt(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            clearInputBuffer();
            return value;
        }
        std::cout << "That doesn't look like a number. Please try again.\n";
        clearInputBuffer();
    }
}

// Plain line read. Does NOT clear the buffer first, so consecutive
// readLine() calls (e.g. title then author) each get their own line
// rather than one call eating the next call's input.
std::string readLine(const std::string& prompt) {
    std::string line;
    std::cout << prompt;
    std::getline(std::cin, line);
    return line;
}

} // namespace

LibrarySystem::LibrarySystem() {
    std::vector<Book> loadedBooks = FileManager::loadBooks(booksFile);
    if (!loadedBooks.empty()) {
        catalog.rebuildFromVector(loadedBooks);
        std::cout << "Loaded " << loadedBooks.size() << " book(s) from " << booksFile << "\n";
    } else {
        seedDefaultBooksIfEmpty();
        std::cout << "No saved catalog found. Seeded with sample books.\n";
    }

    users = FileManager::loadUsers(usersFile);
    if (!users.empty()) {
        std::cout << "Loaded " << users.size() << " user(s) from " << usersFile << "\n";
    } else {
        seedDefaultUsersIfEmpty();
        std::cout << "No saved users found. Seeded with sample users.\n";
    }
}

LibrarySystem::~LibrarySystem() {
    FileManager::saveBooks(booksFile, catalog.toVector());
    FileManager::saveUsers(usersFile, users);
    std::cout << "\nCatalog and users saved. Goodbye!\n";
}

void LibrarySystem::seedDefaultBooksIfEmpty() {
    catalog.addBook({catalog.generateNextId(), "The Pragmatic Programmer", "Hunt & Thomas", 1999, false, ""});
    catalog.addBook({catalog.generateNextId(), "Clean Code", "Robert C. Martin", 2008, false, ""});
    catalog.addBook({catalog.generateNextId(), "Introduction to Algorithms", "Cormen et al.", 2009, false, ""});
    catalog.addBook({catalog.generateNextId(), "Effective C++", "Scott Meyers", 2005, false, ""});
    catalog.addBook({catalog.generateNextId(), "Design Patterns", "Gang of Four", 1994, false, ""});
}

void LibrarySystem::seedDefaultUsersIfEmpty() {
    users.push_back(makeUserWithPassword(true,  1, "Ms. Rivera", "admin123"));
    users.push_back(makeUserWithPassword(false, 2, "Alice", "alice123"));
    users.push_back(makeUserWithPassword(false, 3, "Bob", "bob123"));

    std::cout << "\nDemo accounts created (please change these passwords after logging in):\n";
    std::cout << "  ID 1  Ms. Rivera (Librarian)  password: admin123\n";
    std::cout << "  ID 2  Alice (Member)          password: alice123\n";
    std::cout << "  ID 3  Bob (Member)            password: bob123\n";
}

void LibrarySystem::log(const std::string& message) {
    FileManager::appendLog(logFile, message);
}

bool LibrarySystem::requireLogin() {
    if (!currentUser) {
        std::cout << "You need to log in first (menu option 11).\n";
        return false;
    }
    return true;
}

bool LibrarySystem::requireLibrarian() {
    if (!requireLogin()) return false;
    if (!currentUser->canManageCatalog()) {
        std::cout << "Permission denied: only a Librarian can do that. "
                  << "(" << currentUser->getName() << " is logged in as "
                  << currentUser->role() << ".)\n";
        return false;
    }
    return true;
}

void LibrarySystem::run() {
    int choice = -1;
    while (choice != 0) {
        std::cout << "\n================ Library Management System ================\n";
        std::cout << "Logged in as: "
                  << (currentUser ? (currentUser->getName() + " (" + currentUser->role() + ")") : "nobody")
                  << "\n";
        std::cout << "-------------------------------------------------------------\n";
        std::cout << " 1. Show catalog\n";
        std::cout << " 2. Add book              (Librarian only)\n";
        std::cout << " 3. Remove book           (Librarian only)\n";
        std::cout << " 4. Borrow book           (must be logged in)\n";
        std::cout << " 5. Return book           (must be logged in)\n";
        std::cout << " 6. Search book by ID     (linear + binary search)\n";
        std::cout << " 7. Sort catalog          (bubble sort / merge sort)\n";
        std::cout << " 8. Undo last action      (stack)\n";
        std::cout << " 9. Show action history   (stack)\n";
        std::cout << "10. Show registered users\n";
        std::cout << "11. Login / switch user\n";
        std::cout << "12. Register new member\n";
        std::cout << "13. Change my password\n";
        std::cout << " 0. Exit\n";
        std::cout << "=============================================================\n";

        choice = readInt("Choose an option: ");

        switch (choice) {
            case 1:  handleShowCatalog(); break;
            case 2:  handleAddBook(); break;
            case 3:  handleRemoveBook(); break;
            case 4:  handleBorrowBook(); break;
            case 5:  handleReturnBook(); break;
            case 6:  handleSearchBook(); break;
            case 7:  handleSortCatalog(); break;
            case 8:  handleUndoLastAction(); break;
            case 9:  handleShowHistory(); break;
            case 10: handleShowUsers(); break;
            case 11: handleLogin(); break;
            case 12: handleRegisterMember(); break;
            case 13: handleChangePassword(); break;
            case 0:  std::cout << "Exiting...\n"; break;
            default: std::cout << "Invalid option, try again.\n";
        }
    }
}

void LibrarySystem::handleShowCatalog() {
    std::cout << "\n--- Catalog (" << catalog.size() << " books) ---\n";
    catalog.printAll();
}

void LibrarySystem::handleAddBook() {
    if (!requireLibrarian()) return;

    std::string title = readLine("Book title: ");
    std::string author = readLine("Author: ");
    int year = readInt("Year published: ");

    Book b{catalog.generateNextId(), title, author, year, false, ""};
    catalog.addBook(b);

    std::string desc = currentUser->getName() + " added \"" + title + "\" (ID " + std::to_string(b.id) + ")";
    std::cout << desc << ".\n";
    log(desc);
}

void LibrarySystem::handleRemoveBook() {
    if (!requireLibrarian()) return;

    int id = readInt("Enter book ID to remove: ");
    BookNode* node = catalog.findById(id);
    std::string title = node ? node->data.title : "";

    if (catalog.removeBook(id)) {
        std::string desc = currentUser->getName() + " removed \"" + title + "\" (ID " + std::to_string(id) + ")";
        std::cout << desc << ".\n";
        log(desc);
    } else {
        std::cout << "No book found with ID " << id << ".\n";
    }
}

void LibrarySystem::handleBorrowBook() {
    if (!requireLogin()) return;

    int id = readInt("Enter book ID to borrow: ");
    BookNode* node = catalog.findById(id);
    if (!node) {
        std::cout << "No book found with ID " << id << ".\n";
        return;
    }

    const std::string memberName = currentUser->getName();

    if (node->data.isBorrowed) {
        waitlists[id].enqueue(memberName);
        std::cout << "\"" << node->data.title << "\" is currently borrowed by "
                  << node->data.borrowedBy << ". " << memberName
                  << " has been added to the waitlist (position "
                  << waitlists[id].size() << ").\n";
        log(memberName + " joined the waitlist for \"" + node->data.title + "\" (ID " + std::to_string(id) + ")");
        return;
    }

    node->data.isBorrowed = true;
    node->data.borrowedBy = memberName;

    std::string desc = memberName + " borrowed \"" + node->data.title + "\" (ID " + std::to_string(id) + ")";
    history.push({"BORROW", id, memberName, desc});
    std::cout << desc << ".\n";
    log(desc);
}

void LibrarySystem::handleReturnBook() {
    if (!requireLogin()) return;

    int id = readInt("Enter book ID to return: ");
    BookNode* node = catalog.findById(id);
    if (!node) {
        std::cout << "No book found with ID " << id << ".\n";
        return;
    }
    if (!node->data.isBorrowed) {
        std::cout << "That book isn't currently borrowed.\n";
        return;
    }
    if (node->data.borrowedBy != currentUser->getName() && !currentUser->canManageCatalog()) {
        std::cout << "That book was borrowed by " << node->data.borrowedBy
                  << ", not you. A Librarian can return it on their behalf.\n";
        return;
    }

    std::string returningMember = node->data.borrowedBy;
    node->data.isBorrowed = false;
    node->data.borrowedBy = "";

    std::string desc = returningMember + " returned \"" + node->data.title + "\" (ID " + std::to_string(id) + ")";
    history.push({"RETURN", id, returningMember, desc});
    std::cout << desc << ".\n";
    log(desc);

    auto it = waitlists.find(id);
    if (it != waitlists.end() && !it->second.isEmpty()) {
        std::string nextMember = it->second.dequeue();
        node->data.isBorrowed = true;
        node->data.borrowedBy = nextMember;

        std::string handoffDesc = nextMember + " borrowed \"" + node->data.title +
                                   "\" (ID " + std::to_string(id) + ") from waitlist";
        history.push({"BORROW", id, nextMember, handoffDesc});
        std::cout << "-> Book automatically handed to next in line: " << nextMember << "\n";
        log(handoffDesc);
    }
}

void LibrarySystem::handleSearchBook() {
    int id = readInt("Enter book ID to search for: ");

    // LINEAR SEARCH: works on the live linked list regardless of order.
    BookNode* found = catalog.findById(id);
    if (found) {
        std::cout << "[Linear search] Found: \"" << found->data.title
                  << "\" by " << found->data.author << " (" << found->data.year << ")\n";
    } else {
        std::cout << "[Linear search] No book with ID " << id << ".\n";
    }

    // BINARY SEARCH: requires sorted-by-id data first.
    catalog.sortById();
    int idx = catalog.binarySearchById(id);
    if (idx != -1) {
        std::cout << "[Binary search] Found at sorted position " << idx << ".\n";
    } else {
        std::cout << "[Binary search] Not found.\n";
    }
}

void LibrarySystem::handleSortCatalog() {
    std::cout << "Sort by:\n";
    std::cout << "  1) Title - Bubble sort    O(n^2)\n";
    std::cout << "  2) ID    - Bubble sort    O(n^2)\n";
    std::cout << "  3) Title - Merge sort     O(n log n)\n";
    int choice = readInt("Choice: ");

    switch (choice) {
        case 1: catalog.sortByTitle();     std::cout << "Sorted by title (bubble sort).\n"; break;
        case 2: catalog.sortById();        std::cout << "Sorted by ID (bubble sort).\n"; break;
        case 3: catalog.mergeSortByTitle(); std::cout << "Sorted by title (merge sort).\n"; break;
        default: std::cout << "Invalid choice.\n"; return;
    }
    catalog.printAll();
}

void LibrarySystem::handleUndoLastAction() {
    if (history.isEmpty()) {
        std::cout << "Nothing to undo.\n";
        return;
    }

    Action last = history.pop();
    BookNode* node = catalog.findById(last.bookId);
    if (!node) {
        std::cout << "Could not undo: book no longer exists.\n";
        return;
    }

    if (last.type == "BORROW") {
        node->data.isBorrowed = false;
        node->data.borrowedBy = "";
    } else if (last.type == "RETURN") {
        node->data.isBorrowed = true;
        node->data.borrowedBy = last.memberName;
    }

    std::cout << "Undid: " << last.description << "\n";
    log("UNDO: " + last.description);
}

void LibrarySystem::handleShowHistory() {
    std::cout << "\n--- Action History (most recent first) ---\n";
    history.printHistory();
}

void LibrarySystem::handleShowUsers() {
    std::cout << "\n--- Registered Users ---\n";
    for (const auto& u : users) {
        std::cout << "  #" << u->getId() << " " << u->getName()
                  << " (" << u->role() << ")"
                  << " - can manage catalog: " << (u->canManageCatalog() ? "yes" : "no") << "\n";
    }
}

void LibrarySystem::handleLogin() {
    handleShowUsers();
    int id = readInt("Enter your user ID to log in: ");

    User* target = nullptr;
    for (const auto& u : users) {
        if (u->getId() == id) { target = u.get(); break; }
    }
    if (!target) {
        std::cout << "No user found with ID " << id << ".\n";
        return;
    }

    const int maxAttempts = 3;
    for (int attempt = 1; attempt <= maxAttempts; ++attempt) {
        std::string password = PasswordUtil::readMaskedPassword("Password: ");
        if (target->checkPassword(password)) {
            currentUser = target;
            std::cout << "Logged in as " << currentUser->getName()
                      << " (" << currentUser->role() << ").\n";
            log(currentUser->getName() + " logged in");
            return;
        }
        std::cout << "Incorrect password (" << attempt << "/" << maxAttempts << ").\n";
    }

    std::cout << "Too many failed attempts. Login cancelled.\n";
    log("Failed login attempts for user ID " + std::to_string(id));
}

void LibrarySystem::handleRegisterMember() {
    std::string name = readLine("New member's name: ");

    std::string password;
    while (true) {
        password = PasswordUtil::readMaskedPassword("Choose a password: ");
        if (password.empty()) {
            std::cout << "Password can't be empty. Try again.\n";
            continue;
        }
        std::string confirm = PasswordUtil::readMaskedPassword("Confirm password: ");
        if (password != confirm) {
            std::cout << "Passwords didn't match. Try again.\n";
            continue;
        }
        break;
    }

    int maxId = 0;
    for (const auto& u : users) maxId = std::max(maxId, u->getId());

    auto newMember = makeUserWithPassword(false, maxId + 1, name, password);
    int newId = newMember->getId();
    users.push_back(std::move(newMember));

    std::cout << "Registered " << name << " as a Member with ID " << newId << ".\n";
    log("Registered new member: " + name + " (ID " + std::to_string(newId) + ")");
}

void LibrarySystem::handleChangePassword() {
    if (!requireLogin()) return;

    std::string current = PasswordUtil::readMaskedPassword("Current password: ");
    if (!currentUser->checkPassword(current)) {
        std::cout << "Current password is incorrect.\n";
        return;
    }

    std::string newPassword;
    while (true) {
        newPassword = PasswordUtil::readMaskedPassword("New password: ");
        if (newPassword.empty()) {
            std::cout << "Password can't be empty. Try again.\n";
            continue;
        }
        std::string confirm = PasswordUtil::readMaskedPassword("Confirm new password: ");
        if (newPassword != confirm) {
            std::cout << "Passwords didn't match. Try again.\n";
            continue;
        }
        break;
    }

    currentUser->setPassword(newPassword);
    FileManager::saveUsers(usersFile, users); // persist immediately, don't wait for exit
    std::cout << "Password updated.\n";
    log(currentUser->getName() + " changed their password");
}
