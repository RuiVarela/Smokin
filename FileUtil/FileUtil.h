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

#ifndef __SMOKIN_FILE_UTIL___
#define __SMOKIN_FILE_UTIL___

#include <string>
#include <vector>

namespace Smokin
{
	enum FileType
	{
		FILE_NOT_FOUND,
		REGULAR_FILE,
		DIRECTORY
	};

	enum CaseSensitivity
	{
		CASE_SENSITIVE,
		CASE_INSENSITIVE
	};

	typedef std::vector<std::string> DirectoryContents;
	DirectoryContents getDirectoryContents(std::string const& directory_name);
	std::string getCurrentDirectory();

	bool fileExists(std::string const& filename);
	FileType fileType(std::string const& filename);

	bool makeDirectoryForFile(std::string const& path );
	bool makeDirectory(std::string const& path );

	std::string getFilePath(std::string const& filename);
	std::string getFileExtension(std::string const& filename);
	std::string getLowerCaseFileExtension(std::string const& filename);
	std::string getSimpleFileName(std::string const& fileName);
	std::string getNameLessExtension(std::string const& fileName);
	std::string getStrippedName(std::string const& fileName);

	std::string convertFileNameToWindowsStyle(std::string const& filename);
	std::string convertFileNameToUnixStyle(std::string const& filename);
	bool isFileNameNativeStyle(std::string const& fileName);
	std::string convertFileNameToNativeStyle(std::string const& filename);

	bool equalCaseInsensitive(std::string const& lhs, std::string const& rhs);
	bool equalCaseInsensitive(std::string const& lhs, char const* rhs);

	typedef std::vector<std::string> TextLines;
	bool readTextFile(std::string const& filename, TextLines& text_lines);




	//
	// Find Function functors
	// 
	
	template <typename Comparator>
	Smokin::DirectoryContents find(std::string const& directory, Comparator comparator, bool recursive = false);

	class EqualFilename
	{
	public:
		EqualFilename(std::string const& match_filename, CaseSensitivity match_case_sensitivity = CASE_SENSITIVE);
		bool operator()(std::string const& current_directory, std::string const& current_filename, std::string const& directory_filename);
	private:
		std::string filename;
		CaseSensitivity case_sensitivity;
	};

	class EqualExtension
	{
	public:
		EqualExtension(std::string const& match_extension);
		bool operator()(std::string const& current_directory, std::string const& current_filename, std::string const& directory_filename);
	private:
		std::string extension;
	};


	//
	// Implementation
	//

	template <typename Comparator>
	Smokin::DirectoryContents find(std::string const& directory, Comparator comparator, bool recursive)
	{
		bool needFollowingBackslash = false;
		bool needDirectoryName = true;

		Smokin::DirectoryContents dc;
		Smokin::DirectoryContents results;

		if (directory.empty())
		{
			dc = Smokin::getDirectoryContents(".");
			needFollowingBackslash = false;
			needDirectoryName = false;
		}
		else if (directory =="." || directory =="./" || directory == ".\\")
		{
			dc = Smokin::getDirectoryContents(".");
			needFollowingBackslash = false;
			needDirectoryName = false;
		}
		else
		{
			dc = Smokin::getDirectoryContents(directory);
			char lastChar = directory[directory.size() - 1];

			needFollowingBackslash = ! ((lastChar == '/') || (lastChar == '\\')) ;
			needDirectoryName = true;
		}

		for(Smokin::DirectoryContents::iterator itr = dc.begin(); itr != dc.end(); ++itr)
		{
			std::string path;
			if (!needDirectoryName) 
			{
				path = *itr;
			}
			else if (needFollowingBackslash)
			{
				path = directory + '/' + *itr;
			}
			else
			{
				path = directory + *itr;
			}

			if ( comparator(directory, *itr, path) )
			{
				results.push_back(path);
			}

			if ( recursive && (*itr != ".") && (*itr != "..") && (fileType(path) == Smokin::DIRECTORY) )
			{
				Smokin::DirectoryContents find_results = Smokin::find(path, comparator);
				for (unsigned int i = 0; i != find_results.size(); ++i)
				{
					results.push_back( find_results[i] );
				}
			}
		}

		return results;
	}

};

#endif __SMOKIN_FILE_UTIL___