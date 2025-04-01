/*
 * Error.hpp
 *
 *  Created on: Mar 30, 2025
 *      Author: kobru
 */

#ifndef INC_ERROR_HPP_
#define INC_ERROR_HPP_
#include <string.h>

#define MAX_MESSAGE_LEN 10
#define MAX_ERRORS 5

class Error {
private:
    char header[MAX_MESSAGE_LEN];
    char text[MAX_MESSAGE_LEN];

public:
    Error() {
        header[0] = '\0';
        text[0] = '\0';
    }

    void init(const char* header_, const char* msg) {
        strncpy(header, header_, MAX_MESSAGE_LEN - 1);
        header[MAX_MESSAGE_LEN - 1] = '\0'; // null-terminate

        strncpy(text, msg, MAX_MESSAGE_LEN - 1);
        text[MAX_MESSAGE_LEN - 1] = '\0'; // null-terminate
    }

    const char* getHeader() const { return header; }
    const char* getText() const { return text; }
};

class ErrorModel {
private:
    Error errors[MAX_ERRORS];
    uint8_t errorCount;

    // Private constructor for singleton
    ErrorModel() : errorCount(0) {}

public:
    static ErrorModel& getInstance() {
        static ErrorModel instance;  // C++11: thread-safe and lazy init
        return instance;
    }

    // Delete copy/move constructors to enforce singleton
    ErrorModel(const ErrorModel&) = delete;
    ErrorModel& operator=(const ErrorModel&) = delete;

    bool addError(const char* header, const char* msg) {
        if (errorCount >= MAX_ERRORS) {
            return false;  // log full
        }
        errors[errorCount].init(header, msg);
        errorCount++;
        return true;
    }

    bool removeErrorAt(uint8_t index) {
        if (index >= errorCount) {
            return false;
        }

        // Shift subsequent errors left
        for (uint8_t i = index; i < errorCount - 1; ++i) {
            errors[i] = errors[i + 1];
        }

        // Optional: zero out last entry
        errors[errorCount - 1] = Error();  // make sure Error has default constructor
        errorCount--;

        return true;
    }

    const Error* getError(uint8_t index) const {
        if (index >= errorCount) return nullptr;
        return &errors[index];
    }

    uint8_t getErrorCount() const {
        return errorCount;
    }

    void clearErrors() {
        errorCount = 0;
    }
};

#endif /* INC_ERROR_HPP_ */
