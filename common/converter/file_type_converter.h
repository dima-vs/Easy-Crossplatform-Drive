#ifndef FILE_TYPE_CONVERTER_H
#define FILE_TYPE_CONVERTER_H

#include <QLatin1String>
#include <QString>
#include "domain/file_type.h"

namespace Common::Converter
{

inline const QLatin1String FILE_TYPE_STR      = QLatin1String("file");
inline const QLatin1String DIRECTORY_TYPE_STR = QLatin1String("directory");
inline const QLatin1String UNKNOWN_TYPE_STR   = QLatin1String("unknown");

class FileTypeConverter
{
public:
    static QString toString(Domain::FileType type)
    {
        switch (type)
        {
        case Domain::FileType::File:
            return FILE_TYPE_STR;
        case Domain::FileType::Directory:
            return DIRECTORY_TYPE_STR;
        default: return UNKNOWN_TYPE_STR;
        }
    }

    static Domain::FileType fromString(const QString& str)
    {
        if (str.toLower() == FILE_TYPE_STR)
            return Domain::FileType::File;
        if (str.toLower() == DIRECTORY_TYPE_STR)
            return Domain::FileType::Directory;
        return Domain::FileType::Unknown;
    }
};

}

#endif // FILE_TYPE_CONVERTER_H
