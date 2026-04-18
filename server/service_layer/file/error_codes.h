#ifndef FILE_ERROR_CODES_H
#define FILE_ERROR_CODES_H

namespace ErrorCode::File
{

enum class ServiceError
{
    CannotGetFileTree,
    FileAlreadyExist,
    FileNotFound,
    FileTooLarge,
    InvalidFileSize,
    UuidAlreadyExists,
    ChunkTooLarge,
    InvalidChunkSize,
    SessionDoesNotExists,
    InvalidContentRange,
    FailedToPerformDBOperation,
    FailedToPerformStorageOperation,
    SessionIsNotCompleted,
    FileNotCreated,
    UploadSessionAlreadyCompleted
};

}

#endif // FILE_ERROR_CODES_H
