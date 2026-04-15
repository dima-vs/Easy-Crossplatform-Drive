#ifndef FILE_ERROR_CODES_H
#define FILE_ERROR_CODES_H

namespace ErrorCode::File
{

enum class ServiceError
{
    CannotGetFileTree,
    FileAlreadyExist,
    FileTooLarge,
    UuidAlreadyExists,
    ChunkTooLarge,
    InvalidChunkSize,
    SessionDoesNotExists,
    InvalidContentRange,
    FailedToPerformStorageOperation
};

}

#endif // FILE_ERROR_CODES_H
