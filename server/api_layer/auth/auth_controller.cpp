#include "auth_controller.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <optional>

#include "dto/auth/confirm_register_request.h"
#include "dto/auth/general_response.h"
#include "dto/auth/login_request.h"
#include "dto/auth/register_init_request.h"
#include "dto/auth/register_init_response.h"
#include "serialization/auth/json.h"

namespace Api::Controllers
{

AuthController::AuthController(
    QHttpServer& server,
    Service::Auth::AuthService& authService
    )
    : m_server(server),
    m_authService(authService)
{
}

void AuthController::registerEndpoints()
{
    m_server.route(
        "/auth/register/init",
        QHttpServerRequest::Method::Post,
           [this](const QHttpServerRequest& request) {
               return this->startRegistration(request);
           }
        );

    m_server.route(
        "/auth/register/confirm",
        QHttpServerRequest::Method::Post,
           [this](const QHttpServerRequest& request) {
               return this->completeRegistration(request);
           }
        );

    m_server.route(
        "/auth/login",
        QHttpServerRequest::Method::Post,
           [this](const QHttpServerRequest& request) {
               return this->login(request);
           }
        );
}

QHttpServerResponse AuthController::startRegistration(
    const QHttpServerRequest& request
    )
{
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(
        request.body(),
        &parseError
        );

    if ((parseError.error != QJsonParseError::NoError) ||
        !jsonDoc.isObject())
    {
        return buildBadRequestErrorResponse("Invalid JSON format");
    }

    // JSON -> DTO
    auto requestDtoOpt = Serialization::Auth::
        fromJsonRegisterInitRequest(jsonDoc.object());
    if (!requestDtoOpt.has_value())
    {
        return buildBadRequestErrorResponse("Missing or invalid fields in request");
    }
    const auto& requestDto = requestDtoOpt.value();

    // call service
    auto serviceResult = m_authService.startRegistrationSession(
        requestDto.email
        );
    if (!serviceResult.isOk())
    {
        return buildErrorResponse(serviceResult.error());
    }

    // form response
    DTO::Auth::RegisterInitResponse responseDto;
    responseDto.verificationId = serviceResult.data().verificationId;
    responseDto.expiresAt = serviceResult.data().expiresAt;

    QJsonObject responseJson = Serialization::Auth::toJson(responseDto);
    return QHttpServerResponse(
        responseJson,
        QHttpServerResponder::StatusCode::Ok
        );
}

QHttpServerResponse AuthController::completeRegistration(
    const QHttpServerRequest& request
    )
{
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(
        request.body(), &parseError);

    if ((parseError.error != QJsonParseError::NoError) ||
        !jsonDoc.isObject())
    {
        return buildBadRequestErrorResponse("Invalid JSON format");
    }

    // JSON -> DTO
    auto requestDtoOpt = Serialization::Auth::
        fromJsonConfirmRegisterRequest(jsonDoc.object());
    if (!requestDtoOpt.has_value())
    {
        return buildBadRequestErrorResponse("Missing or invalid fields in request");
    }
    const auto& requestDto = requestDtoOpt.value();

    // call service
    auto serviceResult = m_authService.completeRegistration(
        requestDto.verificationId,
        requestDto.accessCode,
        requestDto.username,
        requestDto.password
        );

    if (!serviceResult.isOk())
    {
        return buildErrorResponse(serviceResult.error());
    }

    // form response
    DTO::Auth::GeneralResponse responseDto;
    responseDto.accessToken = serviceResult.data().accessToken;
    responseDto.expiresAt = serviceResult.data().expiresAt;

    QJsonObject responseJson = Serialization::Auth::toJson(responseDto);
    return QHttpServerResponse(
        responseJson,
        QHttpServerResponder::StatusCode::Created
        );
}

QHttpServerResponse AuthController::login(const QHttpServerRequest& request)
{
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(
        request.body(), &parseError);

    if ((parseError.error != QJsonParseError::NoError) ||
        !jsonDoc.isObject())
    {
        return buildBadRequestErrorResponse("Invalid JSON format");
    }

    // JSON -> DTO
    auto requestDtoOpt = Serialization::Auth::
        fromJsonLoginRequest(jsonDoc.object());
    if (!requestDtoOpt.has_value())
    {
        return buildBadRequestErrorResponse("Missing or invalid fields in request");
    }
    const auto& requestDto = requestDtoOpt.value();

    // call service
    auto serviceResult = m_authService.login(
        requestDto.login,
        requestDto.password
        );
    if (!serviceResult.isOk())
    {
        return buildErrorResponse(serviceResult.error());
    }

    // form response
    DTO::Auth::GeneralResponse responseDto;
    responseDto.accessToken = serviceResult.data().accessToken;
    responseDto.expiresAt = serviceResult.data().expiresAt;

    QJsonObject responseJson = Serialization::Auth::toJson(responseDto);
    return QHttpServerResponse(
        responseJson,
        QHttpServerResponder::StatusCode::Ok
        );
}

QHttpServerResponse AuthController::buildErrorResponse(
    ErrorCode::Auth::ServiceError error
    )
{
    using ServiceError = ErrorCode::Auth::ServiceError;
    using StatusCode = QHttpServerResponder::StatusCode;

    QString errorMsg;
    StatusCode statusCode;

    switch (error)
    {
    case ServiceError::InvalidCredentials:
        errorMsg = "Invalid username or password";
        statusCode = StatusCode::Unauthorized;
        break;
    case ServiceError::UserAlreadyExists:
        errorMsg = "User with this email or username already exists";
        statusCode = StatusCode::Conflict;
        break;
    case ServiceError::SessionExpired:
        errorMsg = "Registration session expired";
        statusCode =  StatusCode::BadRequest;
        break;
    case ServiceError::SessionDoesNotExists:
        errorMsg = "Registration session is invalid or does not exist";
        statusCode = StatusCode::BadRequest;
        break;
    case ServiceError::TooManyAttempts:
        errorMsg = "Too many failed attempts. Try again later.";
        statusCode = StatusCode::TooManyRequests;
        break;
    default:
        errorMsg = "An internal system error occurred";
        statusCode = StatusCode::InternalServerError;
        break;
    }

    return buildErrorResponse(errorMsg, statusCode);
}

QHttpServerResponse AuthController::buildBadRequestErrorResponse(
    const QString& errorMsg
    )
{
    using StatusCode = QHttpServerResponder::StatusCode;
    return buildErrorResponse(errorMsg, StatusCode::BadRequest);
}

QHttpServerResponse AuthController::buildErrorResponse(
    const QString& errorMsg,
    QHttpServerResponder::StatusCode statusCode
    )
{
    QJsonObject errorJson;
    errorJson["error"] = errorMsg;
    return QHttpServerResponse(errorJson, statusCode);
}

} // namespace Api::Controllers
