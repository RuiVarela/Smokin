#pragma once

#include "Text.hpp"

namespace smk
{
    using Path = str32;

    Path getFirstFolder(Path const& filename);
    Path getFilePath(Path const& filename);
    Path getFileExtension(Path const& filename);
    Path getLowerCaseFileExtension(Path const& filename);
    Path getSimpleFileName(Path const& fileName);
    Path getNameLessExtension(Path const& fileName);
    Path getStrippedName(Path const& fileName);

    Path sanitizeName(Path input, bool allow_spaces = false);
    bool isFileNameNativeStyle(Path const& filename);
    Path convertFileNameToWindowsStyle(Path const& filename);
    Path convertFileNameToUnixStyle(Path const& filename);
    Path convertFileNameToNativeStyle(Path const& filename);
    Path mergePaths(Path const& a, Path const& b);
    Path mergePaths(Path const& a, Path const& b, Path const& c);
    Path mergePaths(Path const& a, Path const& b, Path const& c, Path const& d);

    enum class FileType {
        FileNotFound,
        FileRegular,
        FileDirectory
    };

    enum class DirectorySorting {
        DirectorySortingNone,
        DirectorySortingAlphabetical,   // from A to Z
        DirectorySortingTime,           // from older to newer
        DirectorySortingSize,
    };

    using DirectoryContents =  std::vector<Path>;

    DirectoryContents getDirectoryContents(Path const& directory_name, DirectorySorting sorting = DirectorySorting::DirectorySortingNone, bool reverse = false);
    bool makeDirectory(str32 const& path);
    bool makeDirectoryForFile(str32 const& path);

    str32 executablePath();

    FileType fileType(str32 const& filename);
    bool fileSize(str32 const& filename);
    bool fileExists(str32 const& filename);
    bool deleteFile(str32 const& filename);
    bool copyFolder(str32 const& src, str32 const& dst);

    bool readRawBinary(str32 const& filename, std::vector<uint8_t>& buffer);
    bool writeRawBinary(str32 const& filename, std::vector<uint8_t> const& buffer);
    bool readRawText(str32 const& filename, str8& text);
    bool writeRawText(str32 const& filename, str8 const& text);

    bool readUtf8Text(str32 const& filename, str32& text);
    bool writeUtf8Text(str32 const& filename, str32 const& text);

    bool readUtf16Text(str32 const& filename, str32& text);
    bool writeUtf16Text(str32 const& filename, str32 const& text);

    bool saveCppBinary(str32 const& filename, uint8_t const* data, size_t size);
}
