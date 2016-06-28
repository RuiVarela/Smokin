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

#ifndef __SMOKIN_STRING__
#define __SMOKIN_STRING__

#include <string>
#include <sstream>
#include <vector>

namespace Smokin
{
	template<typename OutputType, typename InputType> 
	OutputType lexical_cast(InputType const& value);

	template<typename OutputType, typename InputType> 
	OutputType lexical_cast(InputType const& value, OutputType const& default_value);

	typedef std::vector<std::string> StringElements;
	StringElements split(std::string const& input, std::string const& delimiter);

	void trimRight(std::string &string);
	void trimLeft(std::string &string);
	void trim(std::string &string);

	void uppercase(std::string &string);
	void lowercase(std::string &string);

	void replace(std::string &target, std::string const& match, std::string const& replace_string);


	//
	// Implementation
	//
	template<typename OutputType, typename InputType> 
	OutputType lexical_cast(InputType const& value)
	{
		std::stringstream stream;
		stream << value;

		OutputType output;
		stream >> output;
		return output;
	}

	template<typename OutputType, typename InputType> 
	OutputType lexical_cast(InputType const& value, OutputType const& default_value)
	{
		std::stringstream stream;
		stream << value;

		OutputType output;
		stream >> output;

		if (!stream)
		{
			output = default_value;
		}

		return output;
	}

	inline void trim(std::string &string)
	{
		trimLeft(string);
		trimRight(string);
	}
};

#endif //__SMOKIN_STRING__