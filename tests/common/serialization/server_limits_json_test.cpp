#include <gtest/gtest.h>
#include <QJsonObject>
#include <optional>
#include <QtTypes>

#include "serialization/server_limits/json.h"

TEST(ServerLimitsJsonTest, TransferResponse_RoundTrip)
{
    // 1. Init
    DTO::ServerLimits::TransferResponse original;

    original.upload.maxChunkSize = 5 * 1024 * 1024;
    original.upload.maxFileSize = 1024 * 1024 * 1024;

    original.download.maxChunkSize = 10 * 1024 * 1024;
    original.download.maxFileSize = 5 * 1024 * 1024 * 1024;

    original.storage.maxTotalPerUser = 15 * 1024 * 1024 * 1024;

    // 2. Serialize
    QJsonObject json = Serialization::ServerLimits::toJson(original);

    // 3. Deserialize
    auto restoredOpt = Serialization::ServerLimits
        ::fromJsonTransferResponse(json);

    // 4. Check
    ASSERT_TRUE(restoredOpt.has_value());
    const auto& restored = restoredOpt.value();

    // check Upload
    EXPECT_EQ(restored.upload.maxChunkSize, original.upload.maxChunkSize);
    EXPECT_EQ(restored.upload.maxFileSize, original.upload.maxFileSize);

    // check Download
    EXPECT_EQ(restored.download.maxChunkSize, original.download.maxChunkSize);
    EXPECT_EQ(restored.download.maxFileSize, original.download.maxFileSize);

    // check Storage
    EXPECT_EQ(restored.storage.maxTotalPerUser, original.storage.maxTotalPerUser);
}

TEST(ServerLimitsJsonTest, EmptyJson_ReturnsNullopt)
{
    // 1. Init
    QJsonObject emptyJson;

    // 2. Deserialize
    auto restoredOpt = Serialization::ServerLimits
        ::fromJsonTransferResponse(emptyJson);

    // 3. Check
    EXPECT_FALSE(restoredOpt.has_value());
}
