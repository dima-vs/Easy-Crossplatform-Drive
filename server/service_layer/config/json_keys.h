#ifndef CONFIG_JSON_KEYS_H
#define CONFIG_JSON_KEYS_H

namespace Service::Config::JsonKeys
{

struct Root
{
    static constexpr auto App = "app";
    static constexpr auto Auth = "auth";
    static constexpr auto File = "fileTransfer";
    static constexpr auto Email = "email";
    static constexpr auto SystemSecurity = "systemSecurity";
};

struct App
{
    static constexpr auto Port = "port";
    static constexpr auto Protocol = "protocol";
    static constexpr auto Host = "host";
};

struct SystemSecurity
{
    static constexpr auto PasswordHashing = "passwordHashing";
    static constexpr auto Argon2 = "argon2";

    struct Argon2Keys
    {
        static constexpr auto OpsLimit = "opsLimit";
        static constexpr auto MemLimit = "memLimit";
    };
};

struct Auth
{
    static constexpr auto Token = "token";
    static constexpr auto Uuid = "uuid";
    static constexpr auto Security = "security";

    struct TokenKeys
    {
        static constexpr auto TokenIdEntropyBytes =
            "tokenIdEntropyBytes";

        static constexpr auto TokenSecretEntropyBytes =
            "tokenSecretEntropyBytes";

        static constexpr auto IdGenerationAttemptsLimit =
            "idGenerationAttemptsLimit";
    };

    struct UuidKeys
    {
        static constexpr auto GenerationAttemptsLimit =
            "generationAttemptsLimit";
    };

    struct SecurityKeys
    {
        static constexpr auto RegSessionsDurationSec =
            "regSessionsDurationSec";

        static constexpr auto UserSessionsDurationSec =
            "userSessionsDurationSec";

        static constexpr auto CodeEntryAttemptsLimit =
            "codeEntryAttemptsLimit";

        static constexpr auto MaxActiveSessionsPerUser =
            "maxActiveSessionsPerUser";
    };
};

struct File
{
    static constexpr auto Upload = "upload";
    static constexpr auto Download = "download";
    static constexpr auto Storage = "storage";
    static constexpr auto Session = "session";
    static constexpr auto Uuid = "uuid";

    struct TransferLimits
    {
        static constexpr auto MaxChunkSize = "maxChunkSize";
        static constexpr auto MaxFileSize = "maxFileSize";
    };

    struct StorageKeys
    {
        static constexpr auto MaxTotalStoragePerUser =
            "maxTotalStoragePerUser";

        static constexpr auto BaseStoragePath =
            "baseStoragePath";
    };

    struct SessionKeys
    {
        static constexpr auto UploadSessionsDurationSec =
            "uploadSessionsDurationSec";
    };

    struct UuidKeys
    {
        static constexpr auto GenerationAttemptsLimit =
            "generationAttemptsLimit";
    };
};

struct Email
{
    static constexpr auto Smtp = "smtp";
    static constexpr auto Credentials = "credentials";
    static constexpr auto Sender = "sender";

    struct SmtpKeys
    {
        static constexpr auto Host = "host";
        static constexpr auto Port = "port";
        static constexpr auto ConnectionType =
            "connectionType";
    };

    struct CredentialsKeys
    {
        static constexpr auto Email = "email";
        static constexpr auto AppPassword =
            "appPassword";
    };

    struct SenderKeys
    {
        static constexpr auto DisplayName =
            "displayName";
    };
};

}

#endif // CONFIG_JSON_KEYS_H
