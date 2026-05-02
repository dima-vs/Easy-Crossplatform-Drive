#ifndef AUTH_CONTROLLER_H
#define AUTH_CONTROLLER_H

#include <QHttpServer>
#include <QHttpServerRequest>
#include <QHttpServerResponse>
#include <QHttpServerResponder>
#include <QString>
#include "auth/auth_controller.h"
#include "auth/service.h"

namespace Api::Controllers
{

class AuthController
{
    QHttpServer& m_server;
    Service::Auth::AuthService& m_authService;
public:
    AuthController(
        QHttpServer& server,
        Service::Auth::AuthService& authService
        );

    void registerEndpoints();

    // POST /auth/register/init
    QHttpServerResponse startRegistration(const QHttpServerRequest& request);

    // POST /auth/register/confirm
    QHttpServerResponse completeRegistration(const QHttpServerRequest& request);

    // POST /auth/login
    QHttpServerResponse login(const QHttpServerRequest& request);

private:
    QHttpServerResponse buildErrorResponse(
        ErrorCode::Auth::ServiceError error
    );

    QHttpServerResponse buildBadRequestErrorResponse(
        const QString& errorMsg
        );

    QHttpServerResponse buildErrorResponse(
        const QString& errorMsg,
        QHttpServerResponder::StatusCode statusCode
    );
};

} // namespace Api::Controllers

#endif // AUTH_CONTROLLER_H
