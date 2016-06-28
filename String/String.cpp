/*
    Smokin::String - String Utilities.
    Copyright (C) 2007  Rui Varela - rui.filipe.varela@gmail.com

	This file is part of Smokin::String.

    Smokin::String is free software: you can redistribute it and/or modify
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

#include "String.h"

#include <algorithm>
#include <cctype>

std::vector<std::string> Smokin::split(std::string const& input, std::string const& delimiter)
{
	StringElements output;

	if (input.size() != 0)
	{
		std::string::size_type start = 0;
		std::string::size_type end = 0;

		do
		{
			std::string::size_type end = input.find(delimiter, start);
			if (end == std::string::npos)
			{
				end = input.size();
			}

			std::string selection = input.substr(start, end - start);
			if (selection != "")
			{
				output.push_back(selection);
			}

			start = end + delimiter.size();

		} while ( start < input.size() );

	}
	return output;
}

void Smokin::trimRight(std::string &string)
{
	std::string whitespaces (" \t\f\v\n\r");
	std::string::size_type pos = string.find_last_not_of(whitespaces);

	if(pos != std::string::npos) 
	{
		string.erase(pos + 1);
	}
	else
	{
		string.clear();
	}
}

void Smokin::uppercase(std::string &string)
{
	std::transform(string.begin(), string.end(), string.begin(), (int(*)(int)) std::toupper);
}

void Smokin::lowercase(std::string &string)
{
	std::transform(string.begin(), string.end(), string.begin(), (int(*)(int)) std::tolower);
}

void Smokin::trimLeft(std::string &string)
{
	std::string whitespaces (" \t\f\v\n\r");
	std::string::size_type pos = string.find_first_not_of(whitespaces);

	if(pos != std::string::npos) 
	{
		string.erase(0, pos);
	}
}

void Smokin::replace(std::string &target, std::string const& match, std::string const& replace_string)
{
	std::string::size_type position = target.find(match);

	while(position != std::string::npos)
	{
		target.replace(position, match.size(), replace_string);
		position = target.find(match);
	}
}