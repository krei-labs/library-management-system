#pragma once
#include <string>
#include <memory>

// Abstract base class -> demonstrates ABSTRACTION.
// Fields are protected, password is never exposed in plaintext -> ENCAPSULATION.
class User {
protected:
    int id;
    std::string name;
    std::string passwordSalt; // random per-user salt
    std::string passwordHash; // salted hash of the password; plaintext is never stored

public:
    User(int id, const std::string& name, const std::string& salt, const std::string& hash);
    virtual ~User() = default;

    int getId() const;
    std::string getName() const;
    std::string getSalt() const;
    std::string getPasswordHash() const;

    // Verifies a plaintext password attempt against the stored hash.
    bool checkPassword(const std::string& plainPassword) const;

    // Re-salts and re-hashes with a new password (used by "change password").
    void setPassword(const std::string& newPlainPassword);

    // Pure virtual -> combined with dynamic dispatch this gives POLYMORPHISM.
    virtual bool canManageCatalog() const = 0;
    virtual std::string role() const = 0;
};

// INHERITANCE: Member and Librarian both extend User.
class Member : public User {
public:
    Member(int id, const std::string& name, const std::string& salt, const std::string& hash);
    bool canManageCatalog() const override;
    std::string role() const override;
};

class Librarian : public User {
public:
    Librarian(int id, const std::string& name, const std::string& salt, const std::string& hash);
    bool canManageCatalog() const override;
    std::string role() const override;
};

// Convenience factory: creates a brand-new user straight from a plaintext
// password, generating the salt and hash for you. Used at registration
// time and for seeding demo accounts (loading from file uses the
// role/salt/hash constructor above directly, since those are already hashed).
std::unique_ptr<User> makeUserWithPassword(bool isLibrarian, int id,
                                            const std::string& name,
                                            const std::string& plainPassword);
