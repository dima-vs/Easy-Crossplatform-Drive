#ifndef JSON_KEYS_LIMITS_H
#define JSON_KEYS_LIMITS_H

namespace JsonKeys::ServerLimits
{

static constexpr auto Upload = "upload";
static constexpr auto Download = "download";
static constexpr auto Storage = "storage";

static constexpr auto MaxChunkSize = "maxChunkSize";
static constexpr auto MaxFileSize = "maxFileSize";
static constexpr auto MaxTotalPerUser = "maxTotalPerUser";

}

#endif // JSON_KEYS_LIMITS_H
