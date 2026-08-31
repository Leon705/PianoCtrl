#include "uicontroller.h"

UiController::UiController(Controller *controller, QObject *parent)
    : QObject(parent), controller_(controller)
{ }

void UiController::switchToAdjacentSampleLibrary(bool previous)
{
    const int soundLibraryId = this->controller_->appState().getLastSoundLibraryId();
    const int currentSampleId = this->controller_->appState().getLastSampleLibraryId();

    auto allSampleLibrariesResult = this->controller_->getAllSampleLibrariesInSoundLibrary(soundLibraryId);
    if (!allSampleLibrariesResult)
    {
        this->handleError(allSampleLibrariesResult.error());
        return;
    }

    const auto &samples = allSampleLibrariesResult.value();
    if (samples.empty()) return;

    auto indexResult = this->getIndexById(samples, currentSampleId);
    if (!indexResult)
    {
        this->handleError(indexResult.error());
        return;
    }

    const size_t newIndex = previous ?(indexResult.value() + samples.size() - 1) % samples.size()
                                : (indexResult.value() + 1) % samples.size();

    this->controller_->switchSampleLibrary(samples[newIndex].id);
}

QString UiController::errorToQString(const UiController::Error &error) noexcept
{
    if (std::holds_alternative<UiController::UiControllerError>(error))
    {
        auto uiControllerError = std::get<UiController::UiControllerError>(error);

        QString baseMessage;
        switch (uiControllerError.code)
        {
            case UiController::ErrorCode::ErrorUnexpected:
                baseMessage = QStringLiteral("Unexpected error");
                break;
        }

        if (uiControllerError.message.isEmpty()) {
            return baseMessage;
        }

        return baseMessage + QLatin1StringView(" (") + uiControllerError.message + QLatin1StringView(")");
    }
    else if (std::holds_alternative<Controller::SystemError>(error))
    {
        return Controller::errorToQString(std::get<Controller::SystemError>(error));
    }

    return QString();
}

template<typename T>
    requires std::same_as<T, SoundLibrary> || std::same_as<T, SampleLibrary>
std::expected<size_t, UiController::UiControllerError> UiController::getIndexById(const std::vector<T> &vec, int id)
{
    for (size_t i = 0; i < vec.size(); ++i)
    {
        if (vec[i].id == id) return i;
    }

    return std::unexpected(UiController::UiControllerError{
        UiController::ErrorCode::ErrorUnexpected,
        QStringLiteral("index not found")
    });
}

void UiController::handleError(const Error &error)
{
    QString errorStr = UiController::errorToQString(error);
    qCritical() << errorStr;
    emit this->errorOccured(errorStr);
}
