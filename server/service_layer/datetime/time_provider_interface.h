#ifndef TIME_PROVIDER_INTERFACE_H
#define TIME_PROVIDER_INTERFACE_H

#include <QDateTime>

namespace Service::Time
{

class ITimeProvider
{
public:
    virtual ~ITimeProvider() = default;
    virtual QDateTime currentDateTimeUtc() const = 0;
};

}

#endif // TIME_PROVIDER_INTERFACE_H
