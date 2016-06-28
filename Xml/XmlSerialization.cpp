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

#include "XmlSerialization.h"
#include "tinyxml.h"

#include <vector>

typedef std::pair<TiXmlElement*, unsigned int> XmlElementPair;
struct Smokin::XmlArchiveData
{
	XmlArchiveData() {}
	~XmlArchiveData() {}

	TiXmlDocument document;
	std::vector< XmlElementPair > element_stack;
};

Smokin::XmlArchive::XmlArchive()
:data_(new Smokin::XmlArchiveData()), mode_(None)
{
}

Smokin::XmlArchive::~XmlArchive()
{
	delete(data_);
}

bool Smokin::XmlArchive::open(std::string const& filename, Smokin::XmlArchive::Mode mode)
{
	bool state = false;
	data_->document.Clear();
	data_->element_stack.clear();

	if (mode == Read)
	{
		mode_ = Read;
		filename_ = filename;

		if(data_->document.LoadFile(filename_))
		{
			TiXmlNode* node = data_->document.FirstChild("root");
			if (node)
			{
				TiXmlElement* current_element = node->ToElement();
				if (current_element)
				{
					data_->element_stack.push_back( XmlElementPair(current_element, 0) );
					state = true;
				}
			}
		}

	}
	else if(mode == Write)
	{
		mode_ = Write;
		filename_ = filename;

		TiXmlDeclaration *declaration = new TiXmlDeclaration("1.0", "UTF-8", "yes");
		data_->document.LinkEndChild(declaration);

		TiXmlElement *current_element = new TiXmlElement("root");
		data_->document.LinkEndChild(current_element);
		data_->element_stack.push_back( XmlElementPair(current_element, 0) );
		state = true;
	}
	else
	{
		state = false;
	}

	return state;
}

void Smokin::XmlArchive::beginNode(std::string const& node_name)
{
	if (mode_ == Write)
	{
		TiXmlElement *current_element = new TiXmlElement(node_name);
		data_->element_stack.back().first->LinkEndChild(current_element);
		data_->element_stack.push_back( XmlElementPair(current_element, 0) );
	}
	else if (mode_ == Read)
	{
		if (node_name != "")
		{
			TiXmlElement *current_element = data_->element_stack.back().first->FirstChildElement(node_name);
			if (!current_element)
			{
				throw XmlArchiveException(0001, "XmlElement with name " + node_name + " not found.");
			}
			data_->element_stack.push_back( XmlElementPair(current_element, 0) );
		}
		else
		{
			unsigned int count = data_->element_stack.back().second;
			data_->element_stack.back().second++;

			// goto element number .. "count"
			TiXmlNode* node = data_->element_stack.back().first->FirstChild();
			if (node)
			{
				TiXmlElement *current_element = node->ToElement();
				for( current_element;
					(current_element && (count != 0));
					 current_element = current_element->NextSiblingElement())
				{
					count--;
				}
				
				if (current_element)
				{
					data_->element_stack.push_back( XmlElementPair(current_element, 0) );
				}
			}
		}
	}
}

void Smokin::XmlArchive::endNode()
{
	data_->element_stack.pop_back();
}

unsigned int Smokin::XmlArchive::nodeElementCount() const
{
	unsigned int element_count = 0;

	TiXmlNode* node = data_->element_stack.back().first->FirstChild();
	if (node)
	{
		for(TiXmlElement *current_element = node->ToElement();
			current_element;
			current_element = current_element->NextSiblingElement())
		{
			element_count++;
		}
	}

	return element_count;
}

void Smokin::XmlArchive::writeText(std::string const& text)
{
	TiXmlText *xml_text = new TiXmlText(text);
	data_->element_stack.back().first->LinkEndChild(xml_text);
}

std::string Smokin::XmlArchive::readText()
{
	if (data_->element_stack.back().first->GetText())
	{
		return data_->element_stack.back().first->GetText();
	}
	else
	{
		return std::string();
	}
}

bool Smokin::XmlArchive::save()
{
	bool state = false;

	if (mode_ = Write)
	{
		state = data_->document.SaveFile(filename_);
	}

	return state;
}

void Smokin::Serialization::serialize(XmlArchive& archive, std::string& value)
{
	if (archive.mode() == Smokin::XmlArchive::Write)
	{
		archive.writeText(value);
	}
	else if (archive.mode() == Smokin::XmlArchive::Read)
	{
		value = archive.readText();
	}
}

template <typename T> 
void doSerialization(Smokin::XmlArchive& archive, T& value)
{
	if (archive.mode() == Smokin::XmlArchive::Write)
	{
		std::string as_text = Smokin::xml_lexical_cast<std::string>(value);
		Smokin::Serialization::serialize(archive, as_text);
	}
	else if (archive.mode() == Smokin::XmlArchive::Read)
	{
		std::string as_text;
		Smokin::Serialization::serialize(archive, as_text);
		value = Smokin::xml_lexical_cast<T>(as_text);
	}
}

template <typename T, typename Y> 
void doSerializationWithConversion(Smokin::XmlArchive& archive, T& value)
{
	Y temporary = Y(value);
	Smokin::Serialization::serialize(archive, temporary);
	value = T(temporary);
}
void Smokin::Serialization::serialize(Smokin::XmlArchive& archive, int& value) { doSerialization(archive, value); }
void Smokin::Serialization::serialize(XmlArchive& archive, unsigned int& value) { doSerialization(archive, value); }
void Smokin::Serialization::serialize(XmlArchive& archive, char& value) { doSerializationWithConversion<char, unsigned int>(archive, value); }
void Smokin::Serialization::serialize(XmlArchive& archive, unsigned char& value) { doSerializationWithConversion<unsigned char, unsigned int>(archive, value); }
void Smokin::Serialization::serialize(XmlArchive& archive, double& value) { doSerialization(archive, value); }
void Smokin::Serialization::serialize(XmlArchive& archive, float& value) { doSerialization(archive, value); }
void Smokin::Serialization::serialize(XmlArchive& archive, bool& value){ doSerialization(archive, value); }