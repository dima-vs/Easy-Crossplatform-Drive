#ifndef VIEWCONFIG_H
#define VIEWCONFIG_H
#include <qobject.h>
namespace CommonTypes
{
    struct ViewConfig
    {
        bool expandTreeOnLoad;
        bool rememberLastLogin;

        QString lastLogin;
        //add new config entries
    };
}
Q_DECLARE_METATYPE(CommonTypes::ViewConfig);
#endif // VIEWCONFIG_H
