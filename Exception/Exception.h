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

#ifndef __SMOKIN_EXCEPTION__
#define __SMOKIN_EXCEPTION__

#include <string>
#include <ostream>
#include <climits>

namespace Smokin
{
	#define MetaExceptionCopy(Class) \
			virtual Class* clone() const { return new Class(*this); }
	
	#define MetaExceptionType(Class) \
			virtual std::string type() const { return #Class; }

	#define MetaExceptionConstructor(BaseClass, Class) \
			Class(CodeType const code, std::string const& message) : BaseClass(code, message) {} \
			Class(std::string const& message) : BaseClass(message) {} \
			Class(CodeType const code) : BaseClass(code) {} \
			Class(CodeType const code, std::string message, Exception const& parent) : BaseClass(code, message, parent) {} \
			Class(std::string message, Exception const& parent) : BaseClass(message, parent) {} \
			Class(CodeType code, Exception const& parent) : BaseClass(code, parent) {}

	#define MetaException(BaseClass, Class) \
			MetaExceptionConstructor(BaseClass, Class) \
			MetaExceptionType(Class) \
			MetaExceptionCopy(Class)

	class Exception
	{
	public:
		typedef unsigned int CodeType;
		static unsigned int const code_not_defined = UINT_MAX;
	
		virtual ~Exception() { setParent(0); }

		Exception() { initialize(code_not_defined, "", 0); }
	
		Exception(CodeType const code, std::string const& message) { initialize(code, message, 0); }
		
		Exception(std::string const& message) { initialize(code_not_defined, message, 0); }
		
		Exception(CodeType const code) { initialize(code, "", 0); }
		
		Exception(CodeType const code, std::string message, Exception const& parent) { initialize(code, message, &parent); }
		
		Exception(std::string message, Exception const& parent) { initialize(code_not_defined, message, &parent); }
		
		Exception(CodeType code, Exception const& parent) { initialize(code, "", &parent); }
		
		Exception(Exception const& exception) { initialize(exception); }

		Exception const& operator=(Exception const& exception);

		//
		// clone() virtual method
		//
		MetaExceptionCopy(Exception)

		//
		// type() virtual method
		//
		MetaExceptionType(Exception)

		virtual void serialize(std::ostream& out) const;

		CodeType const& code() const { return code_; }
		std::string const& message() const { return message_; }
		Exception const* parent() const { return parent_; }

		//
		// This will clone parent if parent is not null
		// the current Exception parent will be deleted
		//
		void setParent(Exception const* parent);
		void setCode(CodeType const code) { code_ = code; }
		void setMessage(std::string const& message) { message_ = message; }

		bool hasParent() const { return (parent_ != 0); }
	private:	
		void initialize(Exception const& exception) { initialize(exception.code(), exception.message(), exception.parent()); }
		void initialize(CodeType const code, std::string const& message, Exception const* parent)
		{
			code_ = code;
			message_ = message;
			parent_ = 0;
			setParent(parent);
		}

		CodeType code_;
		std::string message_;
		Exception* parent_;
	};

};

inline std::ostream& operator<<(std::ostream& out, Smokin::Exception const& exception)
{
	exception.serialize(out);
	return out;
}

#endif // __SMOKIN_EXCEPTION__