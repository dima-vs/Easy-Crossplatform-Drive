#include <gtest/gtest.h>
#include <QJsonObject>
#include <QDateTime>
#include <QtTypes>

#include "serialization/auth/json.h"

TEST(AuthJsonTest, ConfirmRegisterRequest_RoundTrip)
{
    // 1. Init
    DTO::Auth::ConfirmRegisterRequest original;
    original.verificationId = "test-uuid-1234";
    original.accessCode = 123456;
    original.username = "test_user";
    original.password = "secret123";

    // 2. Serialize
    QJsonObject json = Serialization::Auth::toJson(original);

    // 3. Deserialize
    auto restoredOpt = Serialization::Auth::
        fromJsonConfirmRegisterRequest(json);

    // 4. Check
    ASSERT_TRUE(restoredOpt.has_value());
    const auto& restored = restoredOpt.value();

    EXPECT_EQ(restored.verificationId, original.verificationId);
    EXPECT_EQ(restored.accessCode, original.accessCode);
    EXPECT_EQ(restored.username, original.username);
    EXPECT_EQ(restored.password, original.password);
}

TEST(AuthJsonTest, GeneralResponse_RoundTrip)
{
    // 1. Init
    DTO::Auth::GeneralResponse original;
    original.accessToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...";
    original.expiresAt = QDateTime::currentDateTime().toUTC();

    // 2. Serialize
    QJsonObject json = Serialization::Auth::toJson(original);

    // 3. Deserialize
    auto restoredOpt = Serialization::Auth::
        fromJsonGeneralResponse(json);

    // 4. Check
    ASSERT_TRUE(restoredOpt.has_value());
    const auto& restored = restoredOpt.value();

    EXPECT_EQ(restored.accessToken, original.accessToken);

    qint64 restoredSec = restored.expiresAt.toSecsSinceEpoch();
    qint64 originalSec = original.expiresAt.toSecsSinceEpoch();
    EXPECT_EQ(restoredSec, originalSec);
}

TEST(AuthJsonTest, LoginRequest_RoundTrip)
{
    // 1. Init
    DTO::Auth::LoginRequest original;
    original.login = "user1@example.com";
    original.password = "strong_secret_psw";

    // 2. Serialize
    QJsonObject json = Serialization::Auth::toJson(original);

    // 3.Deserialize
    auto restoredOpt = Serialization::Auth::fromJsonLoginRequest(json);

    // 4. Check
    ASSERT_TRUE(restoredOpt.has_value());
    const auto& restored = restoredOpt.value();

    EXPECT_EQ(restored.login, original.login);
    EXPECT_EQ(restored.password, original.password);
}

TEST(AuthJsonTest, RegisterInitRequest_RoundTrip)
{
    // 1. Init
    DTO::Auth::RegisterInitRequest original;
    original.email = "no_reply@gmail.com";

    // 2. Serialize
    QJsonObject json = Serialization::Auth::toJson(original);

    // 3. Deserialize
    auto restoredOpt = Serialization::Auth::
        fromJsonRegisterInitRequest(json);

    // 4. Check
    ASSERT_TRUE(restoredOpt.has_value());
    const auto& restored = restoredOpt.value();

    EXPECT_EQ(restored.email, original.email);
}

TEST(AuthJsonTest, RegisterInitResponse_RoundTrip)
{
    // 1. Init
    DTO::Auth::RegisterInitResponse original;
    original.verificationId = "uuid-9999-8888";
    original.expiresAt = QDateTime::currentDateTime()
                             .addSecs(3600).toUTC();

    // 2. Serialize
    QJsonObject json = Serialization::Auth::toJson(original);

    // 3. Deserialize
    auto restoredOpt = Serialization::Auth::
        fromJsonRegisterInitResponse(json);

    // 4. Check
    ASSERT_TRUE(restoredOpt.has_value());
    const auto& restored = restoredOpt.value();

    EXPECT_EQ(restored.verificationId, original.verificationId);

    qint64 restoredSec = restored.expiresAt.toSecsSinceEpoch();
    qint64 originalSec = original.expiresAt.toSecsSinceEpoch();
    EXPECT_EQ(restoredSec, originalSec);
}

TEST(AuthJsonTest, ConfirmRegisterRequest_MissingFields_ReturnsNullopt)
{
    // forgot to set password
    QJsonObject badJson;
    badJson["verificationId"] = "dfwe4-hr34j-3r34g-saefw";
    badJson["accessCode"] = 123456;
    badJson["username"] = "some_user";

    // deserialize
    auto restoredOpt = Serialization::Auth::
        fromJsonConfirmRegisterRequest(badJson);

    EXPECT_FALSE(restoredOpt.has_value());
}


TEST(AuthJsonTest, EmptyJson_ReturnsNullopt)
{
    QJsonObject emptyJson;

    // deserialize ConfirmRegisterRequest
    auto restoredOpt1 = Serialization::Auth
        ::fromJsonConfirmRegisterRequest(emptyJson);

    EXPECT_FALSE(restoredOpt1.has_value());

    // deserialize GeneralResponse
    auto restoredOpt2 = Serialization::Auth
        ::fromJsonGeneralResponse(emptyJson);
    EXPECT_FALSE(restoredOpt2.has_value());

    // deserialize LoginRequest
    auto restoredOpt3 = Serialization::Auth
        ::fromJsonLoginRequest(emptyJson);
    EXPECT_FALSE(restoredOpt3.has_value());

    // deserialize RegisterInitRequest
    auto restoredOpt4 = Serialization::Auth
        ::fromJsonRegisterInitRequest(emptyJson);
    EXPECT_FALSE(restoredOpt4.has_value());

    // deserialize InitResponse
    auto restoredOpt5 = Serialization::Auth
        ::fromJsonRegisterInitResponse(emptyJson);
    EXPECT_FALSE(restoredOpt5.has_value());

}
