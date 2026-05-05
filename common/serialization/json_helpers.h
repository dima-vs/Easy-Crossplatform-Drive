#ifndef JSON_HELPERS_H
#define JSON_HELPERS_H

#include <QJsonObject>
#include <QString>
#include <QDateTime>
#include <QDebug>
#include <optional>
#include <QVariant>
#include <QtTypes>

namespace Serialization::JsonHelper
{

inline bool requireString(
    const QJsonObject& json,
    const char* key,
    QString& outValue
    )
{
    if (!json.contains(key) || !json[key].isString())
    {
        qWarning().nospace() << "invalid or missing '" << key <<
            "' field in JSON";
        return false;
    }
    outValue = json[key].toString();
    return true;
}

inline bool requireInt(
    const QJsonObject& json,
    const char* key,
    int& outValue
    )
{
    if (!json.contains(key) || !json[key].isDouble())
    {
        qWarning().nospace() << "invalid or missing '" << key <<
            "' field in JSON";
        return false;
    }
    outValue = json[key].toInt();
    return true;
}

inline bool requireInt64(
    const QJsonObject& json,
    const char* key,
    qint64& outValue
    )
{
    if (!json.contains(key) || !json[key].isDouble())
    {
        qWarning().nospace() << "invalid or missing '" << key <<
            "' field in JSON";
        return false;
    }
    outValue = json[key].toInteger();
    return true;
}

inline bool requireBool(
    const QJsonObject& json,
    const char* key,
    bool& outValue
    )
{
    if (!json.contains(key) || !json[key].isBool())
    {
        qWarning().nospace() << "invalid or missing '" << key <<
            "' field in JSON";
        return false;
    }
    outValue = json[key].toBool();
    return true;
}

inline bool requireDateTime(
    const QJsonObject& json,
    const char* key,
    QDateTime& outValue
    )
{
    QString dateStr;
    if (!requireString(json, key, dateStr))
        return false;

    outValue = QDateTime::fromString(dateStr, Qt::ISODate);
    if (!outValue.isValid())
    {
        qWarning().nospace() << "invalid '" << key <<
            "' date format (expected ISO 8601)";
        return false;
    }
    return true;
}

inline bool requireNullableInt(
    const QJsonObject& json,
    const char* key,
    std::optional<int>& outValue
    )
{
    if (!json.contains(key))
    {
        qWarning().nospace() << "missing '" << key << "' field in JSON";
        return false;
    }
    if (json[key].isNull())
    {
        outValue = std::nullopt;
        return true;
    }
    if (json[key].isDouble())
    {
        outValue = json[key].toInt();
        return true;
    }
    qWarning().nospace() << "invalid '" << key <<
        "' field type in JSON";
    return false;
}

inline QJsonValue getNullableInt(const std::optional<int>& opt)
{
    return opt.has_value() ? QJsonValue(opt.value()) :
               QJsonValue(QJsonValue::Null);
}

inline bool extractOptionalString(
    const QJsonObject& json,
    const char* key,
    std::optional<QString>& outValue
    )
{
    if (!json.contains(key))
    {
        outValue = std::nullopt;
        return true;
    }
    if (json[key].isString())
    {
        outValue = json[key].toString();
        return true;
    }
    qWarning().nospace() << "invalid '" << key <<
        "' field type in JSON (expected string)";

    return false;
}

inline bool extractOptionalParentId(
    const QJsonObject& json,
    const char* key,
    std::optional<QVariant>& outValue
    )
{
    if (!json.contains(key))
    {
        outValue = std::nullopt;
        return true;
    }
    if (json[key].isNull())
    {
        outValue = QVariant();
        return true;
    }
    if (json[key].isDouble())
    {
        outValue = json[key].toInt();
        return true;
    }

    qWarning().nospace() << "invalid '" << key <<
        "' field type in JSON for PATCH";

    return false;
}

inline bool requireObject(
    const QJsonObject& json,
    const char* key,
    QJsonObject& outValue
    )
{
    if (!json.contains(key) || !json[key].isObject())
    {
        qWarning().nospace() << "invalid or missing '" << key <<
            "' object in JSON";
        return false;
    }
    outValue = json[key].toObject();
    return true;
}

}

#endif // JSON_HELPERS_H
