#ifndef UICONTROLLER_H
#define UICONTROLLER_H

#include "controller.h"

#include <QObject>


class UiController : public QObject
{
    Q_OBJECT

public:
    enum class ErrorCode: uint8_t {
        ErrorUnexpected,
    };
    using UiControllerError = ::Error<ErrorCode>;
    using Error = std::variant<UiController::UiControllerError, Controller::SystemError>;

    explicit UiController(Controller *controller, QObject *parent = nullptr);

    Q_INVOKABLE void switchToAdjacentSampleLibrary(bool previous = false);

    static QString errorToQString(const UiController::Error &error) noexcept;
private:
    Controller *controller_;

    template<typename T>
        requires std::same_as<T, SoundLibrary> || std::same_as<T, SampleLibrary>
    std::expected<size_t, UiController::UiControllerError> getIndexById(const std::vector<T> &vec, int id);
    void handleError(const UiController::Error &error);

signals:
    void errorOccured(const QString &errorMessage);
};

#endif // UICONTROLLER_H
