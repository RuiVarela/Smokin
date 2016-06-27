/*
    Smokin::Filter - Easy Filter Design Library.
    Copyright (C) 2007  Rui Varela - rui.filipe.varela@gmail.com

	This file is part of Smokin::Filter.

    Smokin::Filter is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 2.1 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __SMOKIN_CIRCULAR_BUFFER_H__
#define __SMOKIN_CIRCULAR_BUFFER_H__

#include <cassert>

namespace Smokin
{
	template <typename T>
	class CircularBuffer
	{
	public:
		typedef unsigned int size_type;
		typedef T ElementType;
		typedef ElementType& ReferenceType;
		typedef ElementType const& ConstReferenceType;

		CircularBuffer(size_type reserve_space = 100) 
			:data(0)
		{
			reserve(reserve_space);
		}

		~CircularBuffer() 
		{ 
			free(); 
		}

		void reserve(size_type size)
		{
			free();
			data = new ElementType[size]();
			capacity_ = size;
		}

		size_type capacity() const 
		{ 
			return capacity_; 
		}
		size_type size() const 
		{ 
			return size_; 
		}

		void clear() 
		{ 
			end_index = 0; 
			start_index = 0; 
			size_ = 0; 
		}

		void push_back(ConstReferenceType element)
		{
			data[end_index] = element;
			nextIndex(end_index);

			if (size_ == capacity_)
			{
				nextIndex(start_index);
			}
			else
			{
				++size_;
			}			
		}

		void pop_front()
		{
			assert(size_);

			nextIndex(start_index);
			--size_;
		}

		ReferenceType operator[](size_type n)
		{
			assert(n >= 0);
			assert(n < size_);

			return data[ (start_index + n) % capacity_ ];
		}

		ConstReferenceType operator[](size_type n) const
		{
			assert(n >= 0);
			assert(n < size_);

			return data[ (start_index + n) % capacity_ ];
		}

		ReferenceType front() 
		{ 
			assert(size_);

			return data[start_index]; 
		}

		ConstReferenceType front() const 
		{ 
			assert(size_);

			return data[start_index]; 
		}

		ReferenceType back() 
		{
			assert(size_);

			return data[ (start_index + size_ - 1) % capacity_ ]; 
		}

		ConstReferenceType back() const 
		{ 
			assert(size_);

			return data[ (start_index + size_ - 1) % capacity_ ]; 
		}
	private:
		void free()
		{
			if (data)
			{
				delete[] data;
				data = 0;
			}
			clear();
		}

		void nextIndex(size_type &index)
		{
			assert(capacity_);
			index = (index + 1) % capacity_;
		}


		size_type capacity_;
		size_type size_;

		size_type end_index;
		size_type start_index;

		ElementType *data;
	};

};

#endif //__SMOKIN_CIRCULAR_BUFFER_H__