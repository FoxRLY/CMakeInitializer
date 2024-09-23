#pragma once
#include <exception>

class LogicException : public std::exception {
private:
    const char *message;
public:
    LogicException(const char *message) : message(message) {}

    const char *what() const noexcept override {
        return message;
    }
};

class ExitSignal: public std::exception {
private:
    const char *what() const noexcept override {
        return "exit";
    }
};
