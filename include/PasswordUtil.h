#pragma once
#include <string>

// Small, dependency-free password utilities.
//
// IMPORTANT / HONEST DISCLAIMER: the hash here is a salted, multi-round
// FNV-1a construction — good enough to keep passwords from sitting in
// plaintext on disk for a learning project, but it is NOT a
// cryptographically vetted algorithm like bcrypt/scrypt/argon2. Those
// require external libraries (e.g. OpenSSL/libsodium) that aren't
// guaranteed to be available in every build environment this project
// targets (including Embarcadero C++Builder without extra setup). If you
// adapt this for anything beyond a learning project, swap hashPassword()
// for a real password-hashing library.
namespace PasswordUtil {

    // Generates a random 16-hex-character salt using <random>.
    std::string generateSalt();

    // Combines password + salt and runs it through many rounds of hashing
    // (basic key stretching) so identical passwords never produce identical
    // hashes across users, and brute-forcing is at least slowed down.
    std::string hashPassword(const std::string& password, const std::string& salt);

    // Reads a password from the console without echoing it back as plain
    // text (shows '*' per character instead). Falls back to plain
    // std::getline when stdin isn't an interactive terminal (e.g. input
    // piped from a file), so automated/non-interactive runs still work.
    std::string readMaskedPassword(const std::string& prompt);

} // namespace PasswordUtil
