#include "PasswordUtil.h"
#include <random>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include <cstdio>
#include <iostream>

#ifdef _WIN32
    #include <conio.h>
    #include <io.h>
    #define LMS_ISATTY _isatty
    #define LMS_FILENO _fileno
#else
    #include <termios.h>
    #include <unistd.h>
    #define LMS_ISATTY isatty
    #define LMS_FILENO fileno
#endif

namespace PasswordUtil {

std::string generateSalt() {
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dist;

    uint64_t saltValue = dist(gen);
    std::ostringstream oss;
    oss << std::hex << std::setw(16) << std::setfill('0') << saltValue;
    return oss.str();
}

std::string hashPassword(const std::string& password, const std::string& salt) {
    std::string combined = salt + ":" + password;
    uint64_t hash = 14695981039346656037ULL; // FNV-1a 64-bit offset basis
    const uint64_t prime = 1099511628211ULL;

    // Key stretching: repeat the hash many times, feeding each round's
    // output back into the next round's input, so this is meaningfully
    // slower to brute-force than a single hash pass.
    for (int round = 0; round < 10000; ++round) {
        uint64_t h = hash;
        for (unsigned char c : combined) {
            h ^= c;
            h *= prime;
        }
        hash = h;
        combined = salt + std::to_string(hash);
    }

    std::ostringstream oss;
    oss << std::hex << std::setw(16) << std::setfill('0') << hash;
    return oss.str();
}

std::string readMaskedPassword(const std::string& prompt) {
    std::cout << prompt;
    std::cout.flush();

    // Non-interactive input (piped/redirected) can't be masked meaningfully
    // and termios/conio calls may misbehave on a non-tty stream, so fall
    // back to a plain read in that case.
    if (!LMS_ISATTY(LMS_FILENO(stdin))) {
        std::string password;
        std::getline(std::cin, password);
        return password;
    }

    std::string password;

#ifdef _WIN32
    int ch;
    while ((ch = _getch()) != '\r' && ch != '\n' && ch != EOF) {
        if (ch == '\b') { // backspace
            if (!password.empty()) {
                password.pop_back();
                std::cout << "\b \b";
                std::cout.flush();
            }
        } else {
            password.push_back(static_cast<char>(ch));
            std::cout << '*';
            std::cout.flush();
        }
    }
    std::cout << "\n";
#else
    termios oldSettings{};
    tcgetattr(STDIN_FILENO, &oldSettings);
    termios newSettings = oldSettings;
    newSettings.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);

    char ch;
    while (std::cin.get(ch) && ch != '\n') {
        if (ch == 127 || ch == '\b') { // DEL or backspace
            if (!password.empty()) {
                password.pop_back();
                std::cout << "\b \b";
                std::cout.flush();
            }
        } else {
            password.push_back(ch);
            std::cout << '*';
            std::cout.flush();
        }
    }
    std::cout << "\n";

    tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
#endif

    return password;
}

} // namespace PasswordUtil
