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

#ifndef __SMOKIN_BASE_EXCEPTION__
#define __SMOKIN_BASE_EXCEPTION__

#include "Exception.h"

namespace Smokin
{
	class AllocationException : public Smokin::Exception
	{
	public:
		MetaException(Exception, AllocationException);

		AllocationException(){}
		virtual ~AllocationException() {}
	};

	class MemoryFullException : public Smokin::Exception
	{
	public:
		MetaException(Exception, MemoryFullException);

		MemoryFullException(){}
		virtual ~MemoryFullException() {}
	};

};

#endif // __SMOKIN_BASE_EXCEPTION__