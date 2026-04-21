#ifndef MOCK_TIME_PROVIDER_H
#define MOCK_TIME_PROVIDER_H

#include <gmock/gmock.h>
#include <QDateTime>
#include "datetime/time_provider_interface.h"

namespace Tests::Mocks
{

class MockTimeProvider : public Service::Time::ITimeProvider
{
public:
    QDateTime m_manualTime;

    MOCK_METHOD(QDateTime, currentDateTimeUtc, (), (const, override));
};

}

#endif // MOCK_TIME_PROVIDER_H
