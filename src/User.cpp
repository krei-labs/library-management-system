#include "User.h"
#include "PasswordUtil.h"

User::User(int id, const std::string& name, const std::string& salt, const std::string& hash)
    : id(id), name(name), passwordSalt(salt), passwordHash(hash) {}

int User::getId() const { return id; }
std::string User::getName() const { return name; }
std::string User::getSalt() const { return passwordSalt; }
std::string User::getPasswordHash() const { return passwordHash; }

bool User::checkPassword(const std::string& plainPassword) const {
    return PasswordUtil::hashPassword(plainPassword, passwordSalt) == passwordHash;
}

void User::setPassword(const std::string& newPlainPassword) {
    passwordSalt = PasswordUtil::generateSalt();
    passwordHash = PasswordUtil::hashPassword(newPlainPassword, passwordSalt);
}

Member::Member(int id, const std::string& name, const std::string& salt, const std::string& hash)
    : User(id, name, salt, hash) {}
bool Member::canManageCatalog() const { return false; }
std::string Member::role() const { return "Member"; }

Librarian::Librarian(int id, const std::string& name, const std::string& salt, const std::string& hash)
    : User(id, name, salt, hash) {}
bool Librarian::canManageCatalog() const { return true; }
std::string Librarian::role() const { return "Librarian"; }

std::unique_ptr<User> makeUserWithPassword(bool isLibrarian, int id,
                                            const std::string& name,
                                            const std::string& plainPassword) {
    std::string salt = PasswordUtil::generateSalt();
    std::string hash = PasswordUtil::hashPassword(plainPassword, salt);

    if (isLibrarian) {
        return std::make_unique<Librarian>(id, name, salt, hash);
    }
    return std::make_unique<Member>(id, name, salt, hash);
}
