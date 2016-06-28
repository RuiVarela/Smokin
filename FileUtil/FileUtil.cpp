/*
    Smokin::FileUtil - Utilities for file Handling.
    Copyright (C) 2007  Rui Varela - rui.filipe.varela@gmail.com

	This file is part of Smokin::FileUtil.

    Smokin::FileUtil is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2.1 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* 
 * Note, elements of this FileUtil have been copied directly from openscenegraph library www.openscenegraph.org (osgDB).
 * These elements are licensed under GPL (compatible with OSGPL) as above, with Copyright (C) 1998-2006 Robert Osfield.
 */

#include "FileUtil.h"

// currently this impl is for _all_ platforms, execpt as defined.
// the mac version will change soon to reflect the path scheme under osx, but
// for now, the above include is commented out, and the below code takes precedence.

#if defined(WIN32) && !defined(__CYGWIN__)
#include <Io.h>
#include <Windows.h>
#include <Winbase.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h> // for _mkdir

#define mkdir(x,y) _mkdir((x))
#define stat64 _stati64

// set up for windows so acts just like unix access().
#define F_OK 4

#else // unix

#if defined( __APPLE__ )
// I'm not sure how we would handle this in raw Darwin
// without the AvailablilityMacros.
#include <AvailabilityMacros.h>
// 10.5 defines stat64 so we can't use this #define
// By default, MAC_OS_X_VERSION_MAX_ALLOWED is set to the latest
// system the headers know about. So I will use this as the control
// variable. (MIN_ALLOWED is set low by default so it is 
// unhelpful in this case.) 
// Unfortunately, we can't use the label MAC_OS_X_VERSION_10_4
// for older OS's like Jaguar, Panther since they are not defined,
// so I am going to hardcode the number.
#if (MAC_OS_X_VERSION_MAX_ALLOWED <= 1040)
#define stat64 stat
#endif
#elif defined(__CYGWIN__) || defined(__FreeBSD__) || (defined(__hpux) && !defined(_LARGEFILE64_SOURCE))
#define stat64 stat
#endif
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

// set up _S_ISDIR()
#if !defined(S_ISDIR)
#  if defined( _S_IFDIR) && !defined( __S_IFDIR)
#    define __S_IFDIR _S_IFDIR
#  endif
#  define S_ISDIR(mode)    (mode&__S_IFDIR)
#endif

#include <errno.h>
#include <stack>
#include <fstream>


#if defined(WIN32) && !defined(__CYGWIN__)
#include <io.h>
#include <direct.h>

Smokin::DirectoryContents Smokin::getDirectoryContents(std::string const& directory_name)
{
	Smokin::DirectoryContents contents;

	WIN32_FIND_DATA data;
	HANDLE handle = FindFirstFile((directory_name + "\\*").c_str(), &data);
	if (handle != INVALID_HANDLE_VALUE)
	{
		do
		{
			contents.push_back(data.cFileName);
		}
		while (FindNextFile(handle, &data) != 0);

		FindClose(handle);
	}
	return contents;
}

#else
#include <dirent.h>
Smokin::DirectoryContents Smokin::getDirectoryContents(std::string const& directory_name)
{
	Smokin::DirectoryContents contents;

	DIR *handle = opendir(directory_name.c_str());
	if (handle)
	{
		dirent *rc;
		while((rc = readdir(handle))!=NULL)
		{
			contents.push_back(rc->d_name);
		}
		closedir(handle);
	}

	return contents;
}
#endif // unix getDirectoryContexts



#if defined(WIN32) && !defined(__CYGWIN__)
std::string Smokin::getCurrentDirectory()
{
	unsigned int size = GetCurrentDirectory(0, 0);
	char * current_dir = new char[size + 1];
	GetCurrentDirectory(size, current_dir);

	std::string current_directory = std::string(current_dir);
	delete[](current_dir);

	return current_directory;
}
#else
#include <direct.h> // for getcwd

std::string Smokin::getCurrentDirectory()
{
	char path[_MAX_PATH];
	getcwd(&path[0], _MAX_PATH);
	return std::string(path);
}

#endif //getCurrentDirectory()

bool Smokin::makeDirectory(std::string const& path )
{
	if (path.empty())
	{
		return false;
	}

	struct stat64 stbuf;
	if( stat64( path.c_str(), &stbuf ) == 0 )
	{
		if(S_ISDIR(stbuf.st_mode))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	std::string dir = path;
	std::stack<std::string> paths;
	while( true )
	{
		if( dir.empty() )
			break;

		if( stat64( dir.c_str(), &stbuf ) < 0 )
		{
			switch( errno )
			{
			case ENOENT:
			case ENOTDIR:
				paths.push( dir );
				break;

			default:
				return false;
			}
		}
		dir = getFilePath(std::string(dir));
	}

	while( !paths.empty() )
	{
		std::string dir = paths.top();

#if defined(WIN32)
		//catch drive name
		if (dir.size() == 2 && dir.c_str()[1] == ':') {
			paths.pop();
			continue;
		}
#endif

		if( mkdir( dir.c_str(), 0755 )< 0 )
		{
			return false;
		} 
		paths.pop();
	}
	return true;
}

bool Smokin::makeDirectoryForFile(std::string const& path )
{
	return makeDirectory( getFilePath( path ));
}

bool Smokin::fileExists(std::string const& filename)
{
	return access( filename.c_str(), F_OK ) == 0;
}

Smokin::FileType Smokin::fileType(std::string const& filename)
{
	struct stat64 fileStat;
	if ( stat64(filename.c_str(), &fileStat) != 0 ) 
	{
		return FILE_NOT_FOUND;
	} // end if

	if ( fileStat.st_mode & S_IFDIR )
		return DIRECTORY;
	else if ( fileStat.st_mode & S_IFREG )
		return REGULAR_FILE;

	return FILE_NOT_FOUND;
}

std::string Smokin::getFilePath(std::string const& fileName)
{
	std::string::size_type slash1 = fileName.find_last_of('/');
	std::string::size_type slash2 = fileName.find_last_of('\\');
	if (slash1==std::string::npos) 
	{
		if (slash2==std::string::npos) return std::string();
		return std::string(fileName,0,slash2);
	}
	if (slash2==std::string::npos) return std::string(fileName,0,slash1);
	return std::string(fileName, 0, slash1>slash2 ?  slash1 : slash2);
}

std::string Smokin::getSimpleFileName(std::string const& fileName)
{
	std::string::size_type slash1 = fileName.find_last_of('/');
	std::string::size_type slash2 = fileName.find_last_of('\\');
	if (slash1==std::string::npos) 
	{
		if (slash2==std::string::npos) return fileName;
		return std::string(fileName.begin()+slash2+1,fileName.end());
	}
	if (slash2==std::string::npos) return std::string(fileName.begin()+slash1+1,fileName.end());
	return std::string(fileName.begin()+(slash1>slash2?slash1:slash2)+1,fileName.end());
}


std::string Smokin::getFileExtension(std::string const& fileName)
{
	std::string::size_type dot = fileName.find_last_of('.');
	if (dot==std::string::npos) return std::string("");
	return std::string(fileName.begin()+dot+1,fileName.end());
}

std::string Smokin::getLowerCaseFileExtension(std::string const& filename)
{
	std::string ext = Smokin::getFileExtension(filename);
	for(std::string::iterator itr=ext.begin();
		itr!=ext.end();
		++itr)
	{
		*itr = tolower(*itr);
	}
	return ext;
}

std::string Smokin::getNameLessExtension(std::string const& fileName)
{
	std::string::size_type dot = fileName.find_last_of('.');
	if (dot==std::string::npos) return fileName;
	return std::string(fileName.begin(),fileName.begin()+dot);
}


std::string Smokin::getStrippedName(std::string const& fileName)
{
	std::string simpleName = Smokin::getSimpleFileName(fileName);
	return Smokin::getNameLessExtension( simpleName );
}
bool Smokin::equalCaseInsensitive(std::string const& lhs, std::string const& rhs)
{
	if (lhs.size() != rhs.size())
	{
		return false;
	}

	std::string::const_iterator litr = lhs.begin();
	std::string::const_iterator ritr = rhs.begin();

	while (litr!=lhs.end())
	{
		if (tolower(*litr) != tolower(*ritr))
		{
			return false;
		}

		++litr;
		++ritr;
	}
	return true;
}

bool Smokin::equalCaseInsensitive(std::string const& lhs, char const* rhs)
{
	if (rhs == 0 || lhs.size()!= strlen(rhs))
	{
		return false;
	}

	std::string::const_iterator litr = lhs.begin();
	char const* cptr = rhs;

	while (litr!=lhs.end())
	{
		if (tolower(*litr) != tolower(*cptr))
		{
			return false;
		}

		++litr;
		++cptr;
	}
	return true;
}
bool Smokin::readTextFile(std::string const& filename, Smokin::TextLines& text_lines)
{
	std::ifstream in(filename.c_str());

	if (in.good() )
	{
		while(in.good() && !in.eof())
		{
			std::string current_line;
			std::getline(in, current_line);
			text_lines.push_back(current_line);
		}
		in.close();
	}
	else
	{
		return false;
	}

	return true;
}

Smokin::EqualFilename::EqualFilename(std::string const& match_filename, Smokin::CaseSensitivity match_case_sensitivity)
:filename(match_filename), case_sensitivity(match_case_sensitivity)
{
}

bool Smokin::EqualFilename::operator()(std::string const& current_directory, std::string const& current_filename, std::string const& directory_filename)
{
	return ( (case_sensitivity == Smokin::CASE_INSENSITIVE && Smokin::equalCaseInsensitive(filename, current_filename)) || (filename == current_filename) );
}

Smokin::EqualExtension::EqualExtension(std::string const& match_extension)
:extension(match_extension)
{
}

bool Smokin::EqualExtension::operator()(std::string const& current_directory, std::string const& current_filename, std::string const& directory_filename)
{
	return Smokin::equalCaseInsensitive( Smokin::getFileExtension(current_filename), extension );
}


std::string Smokin::convertFileNameToWindowsStyle(std::string const& filename)
{
	std::string new_filename(filename);

	std::string::size_type slash = 0;

	while( (slash = new_filename.find_first_of('/', slash)) != std::string::npos)
	{
		new_filename[slash]= '\\';
	}
	return new_filename;
}

std::string Smokin::convertFileNameToUnixStyle(std::string const& filename)
{
	std::string new_filename(filename);

	std::string::size_type slash = 0;
	while( (slash = new_filename.find_first_of('\\', slash)) != std::string::npos)
	{
		new_filename[slash]='/';
	}

	return new_filename;
}

bool Smokin::isFileNameNativeStyle(std::string const& filename)
{
#if defined(WIN32) && !defined(__CYGWIN__)
	return filename.find('/') == std::string::npos; // return true if no unix style slash exist
#else
	return filename.find('\\') == std::string::npos; // return true if no windows style slash exist
#endif
}

std::string Smokin::convertFileNameToNativeStyle(std::string const& filename)
{
#if defined(WIN32) && !defined(__CYGWIN__)
	return convertFileNameToWindowsStyle(filename);
#else
	return convertFileNameToUnixStyle(filename);
#endif
}


