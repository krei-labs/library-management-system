<h1 align="center">Library Management System (LMS)</h1>

<p align="center">
  A single-binary C++ console application that models a real library workflow — accounts, roles, borrowing, waitlists, undo history, and persistence — built to demonstrate core data structures and algorithms working together rather than in isolation.
</p>

<p align="center">
  <img alt="C++" src="https://img.shields.io/badge/C%2B%2B-17-blue?logo=cplusplus&logoColor=white">
  <img alt="Build" src="https://img.shields.io/badge/Build-Make-informational?logo=gnu&logoColor=white">
  <img alt="Platform" src="https://img.shields.io/badge/Platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey">
  <img alt="Compiler" src="https://img.shields.io/badge/Compiler-g%2B%2B%20%7C%20clang%2B%2B%20%7C%20Embarcadero-yellowgreen">
  <img alt="License" src="https://img.shields.io/badge/License-Personal%20Project-lightgrey">
</p>

---

LMS is a personal learning project built to show **object-oriented design, hand-built data structures (linked list, stack, queue), sorting/searching algorithms, file persistence, and password-based authentication** working together inside one coherent application — a Librarian and Member console system, rather than a folder of disconnected demos.

> **Note:** This is a terminal application with no bundled GUI, so there are no screenshots — the [Example Session](#-example-session) section below shows a real run instead.

---

## 📑 Table of Contents

- [Features](#-features)
- [Example Session](#-example-session)
- [System Architecture](#%EF%B8%8F-system-architecture)
- [Technology Stack](#%EF%B8%8F-technology-stack)
- [Repository Structure](#-repository-structure)
- [Local Setup](#-local-setup)
- [Building with Embarcadero C++Builder](#-building-with-embarcadero-c-builder)
- [Concepts Demonstrated](#-concepts-demonstrated)
- [Security & Password Handling](#-security--password-handling)
- [Data & Persistence](#-data--persistence)
- [Academic / Learning Context](#-academic--learning-context)
- [My Role](#-my-role)
- [Future Improvements](#-future-improvements)
- [License](#-license)
- [Author](#-author)

---

## ✨ Features

- 🔐 **Password-based Authentication** — Salted, hashed passwords (never stored in plaintext), masked console input, and a 3-attempt login lockout.
- 👥 **Role-Based Access Control** — `Librarian` and `Member` accounts via polymorphic `User` subclasses; only Librarians can add/remove catalog books, enforced at runtime, not just hidden in the menu.
- 📚 **Catalog Management** — Add, remove, and browse books, backed by a hand-built singly linked list rather than `std::list`.
- 🔁 **Borrow / Return Workflow** — Borrowing a taken book automatically joins a waitlist; returning a book automatically hands it to the next person in line.
- ⏳ **Waitlist Queues** — A circular-array FIFO queue per book, implemented from scratch.
- ↩️ **Undo Last Action** — Every borrow/return is pushed onto a hand-built stack, so the most recent action can be reversed on demand.
- 🔎 **Search & Sort** — Linear search and binary search side by side, plus bubble sort (O(n²)) and merge sort (O(n log n)) available from the same menu for direct comparison.
- 📝 **Audit Log** — Every action is appended (not overwritten) to a timestamped activity log, independent of the main save file.
- 💾 **Local File Persistence** — Catalog and user accounts are saved on exit and reloaded on next launch — no database server required.
- 🖥️ **Cross-Compiler Portability** — Builds cleanly under `g++`/`clang++` and Embarcadero C++Builder's modern Clang-enhanced compiler; deliberately avoids `<filesystem>` and other inconsistently-supported headers.

---

## 💻 Example Session

```text
Starting Library Management System...
Loaded 5 book(s) from books.dat
Loaded 3 user(s) from users.dat

================ Library Management System ================
Logged in as: nobody
-------------------------------------------------------------
 1. Show catalog
 2. Add book              (Librarian only)
 3. Remove book           (Librarian only)
 4. Borrow book           (must be logged in)
 5. Return book           (must be logged in)
 6. Search book by ID     (linear + binary search)
 7. Sort catalog          (bubble sort / merge sort)
 8. Undo last action      (stack)
 9. Show action history   (stack)
10. Show registered users
11. Login / switch user
12. Register new member
13. Change my password
 0. Exit
=============================================================
Choose an option: 4
Enter book ID to borrow: 1
"The Pragmatic Programmer" is currently borrowed by Alice. Bob has been
added to the waitlist (position 1).
```

---

## 🏗️ System Architecture

```text
main.cpp
   │  creates
   ▼
LibrarySystem  ───────────────────────────────┐
   │  owns                                    │  owns
   ▼                                           ▼
Catalog (linked list)                    ActionStack (undo history)
   │  contains
   ▼
BookNode → Book (struct)

LibrarySystem also owns:
  • map<bookId, ReservationQueue>   — one waitlist queue per borrowed book
  • vector<unique_ptr<User>>        — polymorphic Librarian / Member accounts

FileManager  ⇄  books.dat / users.dat / activity.log
   loads on startup → Catalog::rebuildFromVector() / user list
   saves on shutdown → Catalog::toVector()          / user list
   appends every action → activity.log (audit trail)

PasswordUtil ⇄ User
   generates salt, hashes password, verifies login attempts,
   handles masked console input
```

**Data flow**

1. `main.cpp` constructs a `LibrarySystem`, which loads the catalog and user accounts from disk (or seeds demo data on first run).
2. The console menu loop routes each choice to a handler method (`handleBorrowBook`, `handleAddBook`, etc.).
3. Handlers operate on the `Catalog` (linked list), `ActionStack` (undo), and per-book `ReservationQueue` (waitlist), gated by the logged-in `User`'s role.
4. On exit, `FileManager` writes the catalog and user list back to disk; every action was already appended to `activity.log` as it happened.

---

## 🛠️ Technology Stack

<div align="center">

| Category | Technologies |
|:---:|:---:|
| **Language** | C++17 |
| **Build** | GNU Make, `g++` / `clang++` / Embarcadero `bcc32c` / `bcc64` |
| **Data Structures** | Hand-built singly linked list, stack, circular-array queue |
| **Algorithms** | Bubble sort, merge sort, linear search, binary search |
| **Persistence** | Plain `<fstream>` (pipe-delimited flat files) — no database server |
| **Security** | Salted + hashed passwords (custom, dependency-free), masked terminal input |
| **Standard Library** | `<memory>` (smart pointers), `<map>`, `<vector>`, `<random>` |

</div>

---

## 📂 Repository Structure

```text
library-management-system/
│
├── include/                    Header files (class declarations)
│   ├── Book.h                   Plain struct: the data unit of the catalog
│   ├── User.h                   Abstract User + Librarian/Member subclasses
│   ├── Catalog.h                 Linked-list catalog + sorting/searching
│   ├── ReservationQueue.h        Circular-array FIFO waitlist queue
│   ├── ActionStack.h              Array-based LIFO undo stack
│   ├── PasswordUtil.h             Salting, hashing, masked input
│   └── LibrarySystem.h            Front controller / menu logic
│
├── src/                         Implementation files
│   ├── User.cpp
│   ├── Catalog.cpp
│   ├── ReservationQueue.cpp
│   ├── ActionStack.cpp
│   ├── PasswordUtil.cpp
│   ├── FileManager.cpp
│   ├── LibrarySystem.cpp
│   └── main.cpp
│
├── Makefile
└── README.md

# Created at runtime in the working directory:
books.dat        catalog data
users.dat        registered users (salted password hashes only)
activity.log     append-only audit trail
```

---

## 🚀 Local Setup

### 1. Get the source

Unzip the project (or clone it, if you've pushed it to a repository):

```bash
cd library-management-system
```

### 2. Build it

Requires a C++17 compiler.

```bash
make          # builds ./library_system
make run      # builds and runs it in one step
make clean    # removes build artifacts
```

Or manually, without the Makefile:

```bash
g++ -std=c++17 -Wall -Wextra -Iinclude -o library_system src/*.cpp
```

### 3. Run it

```bash
./library_system
```

On first launch, the app seeds a sample catalog and three demo accounts, printing their credentials once:

```text
Demo accounts created (please change these passwords after logging in):
  ID 1  Ms. Rivera (Librarian)  password: admin123
  ID 2  Alice (Member)          password: alice123
  ID 3  Bob (Member)            password: bob123
```

> ⚠️ **Change these passwords** (menu option 13) if you intend to keep using this instance beyond a quick test — they're printed in plaintext to the terminal for onboarding convenience only, never stored that way.

---

## 🔧 Building with Embarcadero C++Builder

This project also builds under **Embarcadero C++Builder**, provided you use the **modern, Clang-enhanced compiler** (`bcc32c` / `bcc64`) rather than the legacy Classic compiler (`bcc32`), which predates C++11 and can't handle this codebase.

To keep it portable there, the project deliberately:
- Avoids `<filesystem>` (inconsistent support across Clang-enhanced versions) — all file I/O uses plain `<fstream>`.
- Picks the right masked-password backend automatically: `<conio.h>`'s `_getch()` on Windows (a Borland legacy header C++Builder has always shipped) or `<termios.h>` on POSIX, via `#ifdef _WIN32`.

**Command-line build** (from a RAD Studio command prompt):

```bat
bcc64 -std=c++17 -Iinclude -e library_system.exe ^
  src\ActionStack.cpp src\Catalog.cpp src\FileManager.cpp ^
  src\LibrarySystem.cpp src\PasswordUtil.cpp src\ReservationQueue.cpp ^
  src\User.cpp src\main.cpp
```

Or from the IDE: **File → New → Console Application** (not VCL/FMX), add every file under `include/` and `src/`, set the C++ language standard to C++17, and add `include` to the compiler's include path.

> This was developed and tested against `g++` in a Linux environment, since C++Builder's IDE is Windows-only. The code sticks to portable, standard-only C++17 so it should build cleanly on Embarcadero's modern compiler — worth a first test build on your end in case a version-specific quirk comes up.

---

## 🧩 Concepts Demonstrated

| Concept | Where it lives | How |
|---|---|---|
| Basic programming | `LibrarySystem::run()` | Menu loop, `switch`, safe input parsing |
| OOP – Encapsulation | `User.h` | Protected fields, public accessors, password never exposed |
| OOP – Abstraction | `User.h` | `User` is an abstract class (pure virtual methods) |
| OOP – Inheritance | `User.h` / `User.cpp` | `Member` and `Librarian` extend `User` |
| OOP – Polymorphism | `requireLibrarian()` | `canManageCatalog()` resolved at runtime — actually gates behavior |
| Linked List | `Catalog.h/.cpp` | The catalog is a hand-built singly linked list (`BookNode`) |
| Sorting Algorithms | `Catalog::sortByTitle/sortById/mergeSortByTitle` | Bubble sort O(n²) vs. merge sort O(n log n), compared directly |
| Searching Algorithms | `Catalog::findById` / `binarySearchById` | Linear search vs. binary search |
| Stack | `ActionStack.h/.cpp` | Powers "undo last action" |
| Queue | `ReservationQueue.h/.cpp` | Circular-array FIFO per-book waitlist |
| File Handling | `FileManager.h/.cpp` | Catalog/users saved & reloaded; actions appended to an audit log |
| Security | `PasswordUtil.h/.cpp` | Salted + hashed passwords, masked input, login attempt limiting |

---

## 🔐 Security & Password Handling

- Passwords are **never stored in plaintext** — each account has a random per-user salt and a hashed password in `users.dat`.
- **Honesty note:** the hash is a salted, multi-round FNV-1a construction — enough to keep passwords off disk in plaintext for a learning project, but it is *not* a cryptographically vetted algorithm like bcrypt/argon2 (those need an external crypto library, which would conflict with this project's goal of staying dependency-free and portable to Embarcadero). See the disclaimer at the top of `PasswordUtil.h`.
- Login is limited to 3 attempts before returning to the menu.
- Password input is masked on an interactive terminal, with an automatic plaintext fallback when stdin isn't a real TTY (e.g. piped/scripted input), so automation and manual use both work.
- Only the currently-authenticated user can change their own password, and must re-enter their current password first.

---

## 💾 Data & Persistence

All state lives in three plain text files created in the working directory — no database server, no cloud dependency:

| File | Contents | Format |
|---|---|---|
| `books.dat` | Catalog | `id\|title\|author\|year\|isBorrowed\|borrowedBy` |
| `users.dat` | Accounts | `role\|id\|name\|salt\|passwordHash` |
| `activity.log` | Audit trail | `[timestamp] message`, append-only |

Delete any of these files (or all three) to reset that part of the system back to its seeded defaults on the next run.

---

## 🎓 Academic / Learning Context

This project was built as a self-directed exercise in combining core computer-science fundamentals — object-oriented design, manual data structure implementation, algorithmic complexity, file-based persistence, and basic application security — into a single working system, rather than as separate textbook exercises.

Topics covered:
- Object-oriented design (encapsulation, inheritance, polymorphism, abstraction)
- Manual data structure implementation (linked list, stack, queue)
- Algorithmic complexity comparison (O(n²) vs. O(n log n) sorting)
- File-based persistence and serialization
- Applied authentication (salting, hashing, login flow design)
- Cross-compiler portability (GCC/Clang vs. Embarcadero C++Builder)

---

## 👨‍💻 My Role

**Sole Developer**

- Designed the overall class architecture and composition
- Implemented every data structure (linked list, stack, queue) from scratch
- Implemented sorting/searching algorithms and the persistence layer
- Designed and implemented the authentication and password-hashing system
- Verified cross-compiler portability considerations for Embarcadero C++Builder
- Tested and debugged the full menu-driven workflow end to end

---

## 📈 Future Improvements

- Replace the custom hash with a vetted password-hashing library where a dependency is acceptable
- Persist waitlist queues across restarts (currently in-memory only)
- Add unit tests for `Catalog`, `ActionStack`, and `ReservationQueue`
- Support multiple simultaneous undo levels instead of a single last action
- Move from flat files to a lightweight embedded database (e.g. SQLite)
- Add a configuration file for default seed data and file paths

---

## 📄 License

This is a personal learning project. If you intend to reuse, modify, or redistribute it, please contact the author first.

---

## 👤 Author

**Christian G. Maranan**
Computer Engineering Student — Major in Machine Learning
at Tanauan City College

- **GitHub:** [@krei-labs](https://github.com/krei-labs)
- **Instagram:** [@krei_in](https://instagram.com/krei_in)
- **Email:** [christianmaranan0303@gmail.com](mailto:christianmaranan0303@gmail.com)

---

<p align="center"><strong>Build. Learn. Experiment.</strong> — kréi / Krei Labs</p>
