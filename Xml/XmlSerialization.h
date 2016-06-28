/*
    Smokin::Xml - Easy Xml Serialization.
    Copyright (C) 2007  Rui Varela - rui.filipe.varela@gmail.com

	This file is part of Smokin::Xml.

    Smokin::Xml is free software: you can redistribute it and/or modify
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

#ifndef __SMOKIN_XML_SERIALIZATION__
#define __SMOKIN_XML_SERIALIZATION__

#include <string>
#include <vector>
#include <list>
#include <sstream>

#define SMOKIN_SERIALIZE(archive, variable) Smokin::serialize(archive, #variable, variable)

namespace Smokin
{
	struct XmlArchiveData;

	class XmlArchive
	{
	public:
		enum Mode
		{
			Read,
			Write,
			None
		};

		XmlArchive();
		~XmlArchive();

		bool open(std::string const& filename, Mode mode = Read);
		bool save();

		Mode mode() const { return mode_; }

		//
		// main archive handling methods. 
		// Should not be used directly.
		//

		//
		// beginNode
		// On Write : creates a node with "name".
		// On Read : reads the node with "name". 
		// If no name specified it reads the "current sibling",
		// "current sibling" is advanced on each call to the funcion with no name. 
		//
		void beginNode(std::string const& node_name = "");
		void endNode();
		std::string readText();
		void writeText(std::string const& text);
		unsigned int nodeElementCount() const;
	private:
		XmlArchiveData *data_;
		Mode mode_;
		std::string filename_;
	};

	//
	// Lexical cast for type convertion
	//
	template<typename OutputType, typename InputType> 
	OutputType xml_lexical_cast(InputType const& value)
	{
		std::stringstream stream;
		stream << value;

		OutputType output;
		stream >> output;
		return output;
	}

	//
	// Serialization for the default types
	//	unsigned int, int, float, double, bool
	//	std::string, std::vector, std::list
	//
	namespace Serialization
	{
		void serialize(XmlArchive& archive, std::string& value);
		void serialize(XmlArchive& archive, int& value);
		void serialize(XmlArchive& archive, unsigned int& value);
		void serialize(XmlArchive& archive, char& value);
		void serialize(XmlArchive& archive, unsigned char& value);
		void serialize(XmlArchive& archive, double& value);
		void serialize(XmlArchive& archive, float& value);
		void serialize(XmlArchive& archive, bool& value);

		//
		// Container serialization
		//
		template<typename T>
		void serialize(XmlArchive& archive, std::vector< T >& value)
		{
			if (archive.mode() == Smokin::XmlArchive::Write)
			{
				for (std::vector< T >::size_type iterator = 0; iterator != value.size(); ++iterator)
				{
					Smokin::serialize(archive, "element", value[iterator]);
				}
			}
			else if (archive.mode() == Smokin::XmlArchive::Read)
			{
				unsigned int size = archive.nodeElementCount();
				value.resize(size);
				for (unsigned int iterator = 0; iterator != size; ++iterator)
				{
					archive.beginNode();
					Smokin::Serialization::serialize(archive, value[iterator]);
					archive.endNode();
				}
			}
		}
		
		template<typename T>
		void serialize(XmlArchive& archive, std::list< T >& value)
		{
			if (archive.mode() == Smokin::XmlArchive::Write)
			{
				unsigned int counter = 0;
				for (std::list< T >::iterator iterator = value.begin();
					iterator != value.end();
					++iterator)
				{
					Smokin::serialize(archive, "element", *iterator);
					counter++;
				}
			}
			else if (archive.mode() == Smokin::XmlArchive::Read)
			{
				unsigned int size = archive.nodeElementCount();
				for (unsigned int iterator = 0; iterator != size; ++iterator)
				{
					value.push_back(T());
					archive.beginNode();
					Smokin::Serialization::serialize(archive, value.back());
					archive.endNode();
				}
			}
		}


		// 
		// Generic serialization. For implementing serialization inside the class.
		//
		template<typename T>
		void serialize(XmlArchive& archive, T& object)
		{
			object.serialize(archive);
		}
	};
	
	//
	// Serialization hook
	// This is called to automatically begin and end a node
	//
	template<typename T>
	void serialize(XmlArchive& archive, std::string const name, T& value)
	{
		archive.beginNode(name);
		Smokin::Serialization::serialize(archive, value);
		archive.endNode();
	}

	//
	// Exceptions
	//
	class XmlArchiveException
	{
	public:
		XmlArchiveException(unsigned int exception_code, std::string const& exception_message)
			:message_(exception_message), code_(exception_code)
		{}

		std::string const& message() const { return message_; }
		unsigned int code() const { return code_; }
	private:
		std::string message_;
		unsigned int code_;
	};
};

#endif //__SMOKIN_XML_SERIALIZATION__
