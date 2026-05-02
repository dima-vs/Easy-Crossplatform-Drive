#ifndef SYSTEM_TIME_PROVIDER_H
#define SYSTEM_TIME_PROVIDER_H

#include <QDateTime>
#include "datetime/time_provider_interface.h"

namespace Service::Time
{

class SystemTimeProvider : public ITimeProvider
{
public:
    QDateTime currentDateTimeUtc() const override
    {
        return QDateTime::currentDateTimeUtc();
    }
};

}

#endif // SYSTEM_TIME_PROVIDER_H
