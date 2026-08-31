#include "iserializable.h"

QString ISerializable::errorToQString(const ISerializable::Error &error) noexcept
{
    QString baseMessage;
    switch (error.code)
    {
    case ISerializable::ErrorCode::ErrorInvalidFormat:
        baseMessage = QStringLiteral("Bad JSON format");
        break;
    }

    if (error.message.isEmpty()) {
        return baseMessage;
    }

    return baseMessage + QLatin1StringView(" (") + error.message + QLatin1StringView(")");
}