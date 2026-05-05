#ifndef JSON_KEYS_FILE_H
#define JSON_KEYS_FILE_H

namespace JsonKeys::File
{


static constexpr auto Id = "id";
static constexpr auto Name = "name";
static constexpr auto ParentId = "parentId";
static constexpr auto Size = "size";
static constexpr auto CreatedAt = "createdAt";
static constexpr auto Type = "type";

struct Tree
{
    static constexpr auto IsDirectory = "isDirectory";
    static constexpr auto Children = "children";
};

struct Upload
{
    static constexpr auto Overwrite = "overwrite";
    static constexpr auto ChunkSize = "chunkSize";
    static constexpr auto UploadId = "uploadId";
    static constexpr auto ExpiresAt = "expiresAt";
};

struct Rename
{
    static constexpr auto NewName = "newName";
    static constexpr auto NewParentId = "newParentId";
};


}

#endif // JSON_KEYS_FILE_H
