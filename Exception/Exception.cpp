/*
    Smokin::Exception - Traceable Exceptions.
    Copyright (C) 2007  Rui Varela - rui.filipe.varela@gmail.com

	This file is part of Smokin::Exception.

    Smokin::Exception is free software: you can redistribute it and/or modify
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

#include "Exception.h"

Smokin::Exception const& Smokin::Exception::operator=(Smokin::Exception const& exception) 
{
	setParent(0); // delete parent
	initialize(exception);
	return *this;
}

void Smokin::Exception::serialize(std::ostream& out) const
{
	out << type() << " : ";

	if (code() != code_not_defined)
	{
		out << "[" << code() << "] ";
	}

	if (message() != "")
	{
		out << message();
	}

	if (hasParent())
	{
		out << std::endl;
		parent_->serialize(out);
	}
}

void Smokin::Exception::setParent(Exception const* parent) 
{
	if (parent_)
	{
		delete (parent_);
	}

	parent_ =0;

	if (parent)
	{
		parent_ = parent->clone();
	}
}


