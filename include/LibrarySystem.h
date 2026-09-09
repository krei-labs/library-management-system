#pragma once
#include "Catalog.h"
#include "ReservationQueue.h"
#include "ActionStack.h"
#include "User.h"
#include <map>
#include <memory>
#include <vector>
#include <string>

// LibrarySystem is the "front controller" that composes every other
// component (COMPOSITION) and drives the console menu.
class LibrarySystem {
private:
    Catalog catalog;
    ActionStack history;
    std::map<int, ReservationQueue> waitlists;   // bookId -> waiting members
    std::vector<std::unique_ptr<User>> users;
    User* currentUser = nullptr;                 // non-owning: whoever is logged in

    const std::string booksFile  = "books.dat";
    const std::string usersFile  = "users.dat";
    const std::string logFile    = "activity.log";

    void seedDefaultBooksIfEmpty();
    void seedDefaultUsersIfEmpty();
    void log(const std::string& message);

    // Permission / login guards used by the menu handlers.
    bool requireLogin();
    bool requireLibrarian();

public:
    LibrarySystem();
    ~LibrarySystem(); // auto-saves catalog + users on shutdown

    void run();

    void handleShowCatalog();
    void handleAddBook();
    void handleRemoveBook();
    void handleBorrowBook();
    void handleReturnBook();
    void handleSearchBook();
    void handleSortCatalog();
    void handleUndoLastAction();
    void handleShowHistory();
    void handleShowUsers();
    void handleLogin();
    void handleRegisterMember();
    void handleChangePassword();
};
