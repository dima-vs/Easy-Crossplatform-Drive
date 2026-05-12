#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <QString>

namespace Config::App
{
    struct AppConfig
    {
        QString protocol = "http";
        QString host = "127.0.0.1"; 
        int port = 8080;
    };
}

#endif // APP_CONFIG_H