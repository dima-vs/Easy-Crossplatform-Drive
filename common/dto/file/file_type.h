#ifndef FILE_TYPE_TMP_H
#define FILE_TYPE_TMP_H

#include <QString>

namespace DTO::File
{

inline const QString FILE_TYPE_STR      = "file";
inline const QString DIRECTORY_TYPE_STR = "directory";
inline const QString UNKNOWN_TYPE_STR   = "unknown";

enum class FileType
{
    File,
    Directory,
    Unknown
};

inline QString fileTypeToString(FileType ft)
{
    switch(ft)
    {
    case FileType::File:      return FILE_TYPE_STR;
    case FileType::Directory: return DIRECTORY_TYPE_STR;
    case FileType::Unknown:   return UNKNOWN_TYPE_STR;
    }
    return UNKNOWN_TYPE_STR;
}

inline FileType stringToFileType(const QString& s)
{
    if (s == FILE_TYPE_STR)      return FileType::File;
    if (s == DIRECTORY_TYPE_STR) return FileType::Directory;

    return FileType::Unknown;
}

}

#endif // FILE_TYPE_TMP_H
