#ifndef COREERROR_H
#define COREERROR_H

#include <QString>

template <typename ErrorEnum>
struct Error {
    ErrorEnum code;
    QString message;

    constexpr Error(ErrorEnum c) : code(c) {}

    Error(ErrorEnum c, QString msg)
        : code(c), message(std::move(msg)) {}
};

#endif // COREERROR_H
