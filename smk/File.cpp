#include "File.hpp"

#include <filesystem>
#include <fstream>


// executable path
#if defined(__APPLE__)
    #include <mach-o/dyld.h>
#elif defined(_WIN32)
    #include <windows.h>
#else
    #include <unistd.h>
#endif

namespace smk
{
    Path getFirstFolder(Path const& filename) {
        Path::size_type slash = filename.find_first_of('/');
        if (slash == Path::npos)
            slash = filename.find_first_of('\\');

        if (slash == Path::npos)
            return filename;
        return filename.substr(0, slash);
    }

    Path getFilePath(Path const& filename) {
        Path::size_type slash1 = filename.find_last_of('/');
        Path::size_type slash2 = filename.find_last_of('\\');

        if (slash1 == Path::npos) {
            if (slash2 == Path::npos)
                return Path();
            return Path(filename, 0, slash2);
        }

        if (slash2 == Path::npos)
            return Path(filename, 0, slash1);
        
        return Path(filename, 0, slash1 > slash2 ? slash1 : slash2);
    }

    Path getSimpleFileName(Path const& filename) {
		Path::size_type slash1 = filename.find_last_of('/');
		Path::size_type slash2 = filename.find_last_of('\\');

		if (slash1 == Path::npos){
			if (slash2 == Path::npos)
				return filename;
			return Path(filename.begin() + slash2 + 1, filename.end());
		}

		if (slash2 == Path::npos)
			return Path(filename.begin() + slash1 + 1, filename.end());

		return Path(filename.begin() + (slash1 > slash2 ? slash1 : slash2) + 1, filename.end());
    }

    Path getFileExtension(Path const& filename) {
        Path::size_type dot = filename.find_last_of('.');

        if (dot == Path::npos)
            return Path();
        
        return Path(filename.begin() + dot + 1, filename.end());
    }

    Path getLowerCaseFileExtension(Path const& filename) {
        Path output = getFileExtension(filename);
        lowercase(output);
        return output;
    }

    Path getNameLessExtension(Path const& filename) {
        Path::size_type dot = filename.find_last_of('.');

        if (dot == Path::npos)
            return filename;
        
        return Path(filename.begin(), filename.begin() + dot);
    }

    Path getStrippedName(Path const& filename) {
        Path simpleName = getSimpleFileName(filename);
        return getNameLessExtension(simpleName);
    }


	Path sanitizeName(Path input, bool allow_spaces) {
        lowercase(input);
        Path out;
        for(size_t i = 0; i < input.size(); ++i) {
            if ((allow_spaces && (input[i] == ' ')) || 
                (input[i] >= 'a' && input[i] <= 'z') || (input[i] >= '0' && input[i] <= '9')) {
                out += input[i];
            }
        }

        trim(out);
        return out;
    }

    bool isFileNameNativeStyle(Path const& filename) {
#if defined( _WIN32 )
		return filename.find('/') == Path::npos;
#else
		return filename.find('\\') == Path::npos;
#endif
    }

    Path convertFileNameToWindowsStyle(Path const& filename) {
        Path new_filename(filename);
        Path::size_type slash = 0;
        while ((slash = new_filename.find_first_of('/', slash)) != Path::npos) 
            new_filename[slash] = '\\';
       
        return new_filename;
    }

    Path convertFileNameToUnixStyle(Path const& filename) {
        Path new_filename(filename);
        Path::size_type slash = 0;

        while ((slash = new_filename.find_first_of('\\', slash)) != Path::npos)
            new_filename[slash] = '/';

        return new_filename;
    }

    Path convertFileNameToNativeStyle(Path const& filename) {
#if defined(_WIN32)
        return convertFileNameToWindowsStyle(filename);
#else 
        return convertFileNameToUnixStyle(filename);
#endif
    }

    Path mergePaths(Path const& a, Path const& b) {
        if (a.empty()) 
            return convertFileNameToNativeStyle(b);
        
        if (b.empty()) 
            return convertFileNameToNativeStyle(a);
        
        if (endsWith(a, U"\\") || startsWith(b, U"\\") || 
            endsWith(a, U"/")  || startsWith(b, U"/")) 
            return convertFileNameToNativeStyle(a + b);
       
        return convertFileNameToNativeStyle(a + U"/" + b);
    }

    Path mergePaths(Path const& a, Path const& b, Path const& c) {
        return mergePaths(mergePaths(a, b), c);
    }

    Path mergePaths(Path const& a, Path const& b, Path const& c, Path const& d) {
        return mergePaths(mergePaths(a, b, c), d);
    }

    DirectoryContents getDirectoryContents(Path const& directory_name, DirectorySorting sorting, bool reverse) {
        DirectoryContents contents;

        using SorterPair = std::pair<uint64_t, Path>;
        std::vector<SorterPair> sorted_elements;

        if (fileType(directory_name) != FileType::FileDirectory)
            return contents;
    
        std::filesystem::path path(directory_name);
        
        for (const auto &entry : std::filesystem::directory_iterator(path)) {
            Path name = getSimpleFileName(entry.path().u32string());

             if ((sorting == DirectorySorting::DirectorySortingNone) || (sorting == DirectorySorting::DirectorySortingAlphabetical) || !entry.is_regular_file()) {
                 
                 contents.push_back(name);
                 
             } else {

                uint64_t sorter = 0;

                if (sorting == DirectorySorting::DirectorySortingTime) {
                    auto write_time = entry.last_write_time();
                    sorter = std::chrono::duration_cast<std::chrono::seconds>(write_time.time_since_epoch()).count();
                } else if (sorting == DirectorySorting::DirectorySortingSize) {
                    sorter = entry.file_size();
                }

                sorted_elements.push_back(std::make_pair(sorter, name));

            }
        }

        if (sorting == DirectorySorting::DirectorySortingAlphabetical) {

            std::sort(contents.begin(), contents.end());

        } else if (!sorted_elements.empty()) {

             std::sort(sorted_elements.begin(), sorted_elements.end(), [](SorterPair const& l, SorterPair const& r) {
                 return l.first < r.first;
             });

             for (auto const &[sorter, name] : sorted_elements)
                 contents.push_back(name);
        }


        if (reverse)
            std::reverse(contents.begin(), contents.end());

        return contents;
    }

	bool makeDirectory(str32 const& path) {
        if (path.empty())
            return false;

        if (fileType(path) == FileType::FileDirectory)
            return false;

        std::filesystem::path fs_path(path);
        return std::filesystem::create_directories(fs_path);
    }

	bool makeDirectoryForFile(str32 const& path) {
        return makeDirectory(getFilePath(path));
    }

    str32 executablePath() {
        constexpr size_t max_path = 1024;

#if defined(WIN32)
        wchar_t buffer[max_path];
        DWORD size = max_path;
        size = GetModuleFileNameW(nullptr, buffer, size);
        if (size > 0) 
            return decodeUtf16((uint16_t const*)buffer, size);
             
#elif defined(__APPLE__)

        char buffer[max_path];
        uint32_t size = sizeof(buffer);
        memset(buffer, 0, size);
    
    
        if (_NSGetExecutablePath(buffer, &size) == 0)
            return decodeUtf8((uint8_t const*)buffer, size);
#else
        
        char buffer[max_path];
        size_t size = sizeof(buffer);
        memset(buffer, 0, size);
        
        int byte_count = readlink("/proc/self/exe", buffer, size);
        if (byte_count != -1) {
            return decodeUtf8(buffer, byte_count);
        }
#endif
        
        return str32();
    }


    FileType fileType(str32 const& filename) {
        std::filesystem::path path(filename);
        auto status = std::filesystem::status(path);
        if (std::filesystem::is_directory(status)) {
            return FileType::FileDirectory;
        }  else if (std::filesystem::is_regular_file(status)) {
            return FileType::FileRegular;
        }
        return FileType::FileNotFound;
    }

    bool fileSize(str32 const& filename) {
        std::filesystem::path path(filename);
        
        uint64_t size = 0;
        size = std::filesystem::file_size(path);
        return size;
    }

    bool fileExists(str32 const& filename) {
        std::filesystem::path path(filename);
        auto status = std::filesystem::status(path);
        return std::filesystem::exists(status);
    }

    bool deleteFile(str32 const& filename) {
        std::filesystem::path path(filename);
        std::error_code ec;
        return std::filesystem::remove_all(path, ec) > 0;
        //return std::filesystem::remove(filename, ec)
    }

    bool copyFolder(str32 const& src, str32 const& dst) {
        std::error_code ec;
        std::filesystem::path src_path(src);
        std::filesystem::path dst_path(dst);

        std::filesystem::copy(src_path, dst_path,
            std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive,
            ec);

        return ec.operator bool();
    }

    bool readRawBinary(str32 const& filename, std::vector<uint8_t>& buffer) {
        std::filesystem::path path(filename);
        
        std::ifstream file_stream(path, std::fstream::binary);
        if (!file_stream)
            return false;

        file_stream.seekg(0, file_stream.end);
        auto length = file_stream.tellg();
        file_stream.seekg(0, file_stream.beg);

        buffer.resize(length);
        file_stream.read((char*)buffer.data(), length);

        file_stream.close();
        return true;
    }

    bool writeRawBinary(str32 const& filename, std::vector<uint8_t> const& buffer) {
        std::filesystem::path path(filename);
        
        std::ofstream out(path, std::ios::out | std::ios::trunc | std::ios::binary);
        if (out.good())
            out.write((const char*)buffer.data(), buffer.size());
        return out.good();
    }

    bool readRawText(str32 const& filename, str8& text) {
        std::vector<uint8_t> buffer;
        if (!readRawBinary(filename, buffer)) 
            return false;
        
        buffer.push_back(0);
        text = str8((const char*)buffer.data(), buffer.size() - 1);
        return true;
    }

    bool writeRawText(str32 const& filename, str8 const& text) {
        std::vector<uint8_t> buffer(text.size(), 0);
        if (text.size() > 0)
            memcpy(buffer.data(), text.c_str(), text.size());
        return writeRawBinary(filename, buffer);
    }

    bool readUtf8Text(str32 const& filename, str32& text) {
        std::vector<uint8_t> buffer;
        if (!readRawBinary(filename, buffer)) 
            return false;
        
        buffer.push_back(0);
        text = decodeUtf8(buffer.data(), buffer.size() - 1);
        return true;
    }

    bool writeUtf8Text(str32 const& filename, str32 const& text) {
        str8 data = encodeUtf8(text);
        return writeRawText(filename, data);
    }

    bool readUtf16Text(str32 const& filename, str32& text) {
        std::vector<uint8_t> buffer;
        if (!readRawBinary(filename, buffer)) 
            return false;
        
        buffer.push_back(0);
        buffer.push_back(0);
        text = decodeUtf16((uint16_t const*) buffer.data(), (buffer.size() - 2) / 2);
        return true;
    }

    bool writeUtf16Text(str32 const& filename, str32 const& text) {
        str16 utf16 = encodeUtf16(text);
        std::vector<uint8_t> buffer;
        
        uint8_t const* data = (uint8_t const*) utf16.data();
        for (size_t i = 0; i != utf16.size(); ++i) {
            buffer.push_back(*(data + i * 2 + 0));
            buffer.push_back(*(data + i * 2 + 1));
        }
        
        return writeRawBinary(filename, buffer);
    }

    bool saveCppBinary(str32 const& filename, uint8_t const* data, size_t size) {
        std::filesystem::path path(filename);

        std::ofstream out(path, std::ios::out | std::ios::trunc);

        constexpr size_t line_size = 25;

        constexpr size_t buffer_size = 50;
        char buffer[buffer_size];

        if (out.good()) {
            out << "const uint8_t data[" << size << "] = { ";

            for (size_t i = 0; i != size; ++i) {

                if (i % line_size == 0)
                    out << std::endl << "\t";

                memset(buffer, 0, buffer_size);
                snprintf(buffer, buffer_size, "0x%02X", data[i]);

                out << buffer;

                if (i != size - 1)
                    out << ", ";
            }

            out << std::endl << "};" << std::endl;
        }

        return out.good();
    }
}
