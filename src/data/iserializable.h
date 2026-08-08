#ifndef ISERIALIZABLE_H
#define ISERIALIZABLE_H

#include <QJsonObject>
#include <expected>
#include "src/coreerror.h"

class ISerializable {
public:
    enum class ErrorCode: uint8_t {
        ErrorInvalidFormat,
    };
    using Error = ::Error<ISerializable::ErrorCode>;

    virtual ~ISerializable() = default;

    virtual QJsonObject toJson() const = 0;
    virtual std::expected<void, ISerializable::Error> fromJson(const QJsonObject &json) = 0;

    static QString errorToQString(const ISerializable::Error& error) noexcept;
};

#endif // ISERIALIZABLE_H
