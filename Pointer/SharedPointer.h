/*
    Smokin::Pointer - Smart Pointer Handling.
    Copyright (C) 2007  Rui Varela - rui.filipe.varela@gmail.com

	This file is part of Smokin::Pointer.

    Smokin::Pointer is free software: you can redistribute it and/or modify
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

#ifndef __SMOKIN_SHARED_POINTER__
#define __SMOKIN_SHARED_POINTER__

namespace Smokin
{
	class PointerReferenceCounter
	{
	public:
		typedef unsigned int CounterType;

		PointerReferenceCounter() :reference_count(0) {}
		~PointerReferenceCounter() {}

		void reference() { reference_count++; }
		CounterType unreference() { return --reference_count;}

		CounterType reference_count;
	};

	template <typename T>
	class SharedPointer
	{
	public:
		typedef T Type;
		typedef Type* PointerType;
		typedef Type const* ConstPointerType;
		typedef Type& ReferenceType;
		typedef Type const& ConstReferenceType;


		//
		// Constructors
		//
		SharedPointer()
			:pointer(0) 
		{
		    reference_counter = new PointerReferenceCounter();
            reference_counter->reference();
		}

		SharedPointer(PointerType data_pointer) 
		{
			pointer = data_pointer;
			reference_counter = new PointerReferenceCounter();
            reference_counter->reference();
		}

		SharedPointer(SharedPointer const& shared_pointer) 
			: reference_counter(shared_pointer.reference_counter), pointer(shared_pointer.pointer)
		{
			reference_counter->reference();
		}

		~SharedPointer() 
		{ 
			if (reference_counter->unreference() == 0)
			{
				delete(pointer);
				delete(reference_counter);
			}
		}

		ReferenceType operator*() const { return *pointer; }
        PointerType operator->() const { return pointer; }
        PointerType get() const { return pointer; }

		bool operator!() const { return pointer == 0; }
        bool valid() const { return pointer != 0; }

		bool operator == (SharedPointer const& shared_pointer) const { return (pointer == shared_pointer.pointer); }
        bool operator == (ConstPointerType data_pointer) const { return (pointer == data_pointer); }
		friend bool operator == (ConstPointerType data_pointer, SharedPointer const& shared_pointer) { return (data_pointer == shared_pointer.pointer); }
        
		bool operator != (SharedPointer const& shared_pointer) const { return (pointer != shared_pointer.pointer); }
        bool operator != (ConstPointerType data_pointer) const { return (pointer != data_pointer); }
		friend bool operator != (ConstPointerType data_pointer, SharedPointer const& shared_pointer) { return (data_pointer != shared_pointer.pointer); }
        
		void swap(ReferenceType shared_pointer) 
		{
			PointerReferenceCounter* temporary_reference_counter = reference_counter;
			reference_counter = shared_pointer.reference_counter;
			shared_pointer.reference_counter = temporary_reference_counter;

			PointerType temporary_pointer = pointer;
			pointer = shared_pointer.pointer;
			shared_pointer.pointer = temporary_pointer;
		}

		SharedPointer& operator = (SharedPointer const& shared_pointer)
		{
			if (this != &shared_pointer)
			{
				if (reference_counter->unreference() == 0)
				{
					delete(pointer);
					delete(reference_counter);
				}

				pointer = shared_pointer.pointer;
				reference_counter = shared_pointer.reference_counter;
				reference_counter->reference();
			}

			return *this;
		}

        SharedPointer& operator = (PointerType data_pointer)
        {
            if (pointer != data_pointer)
			{ 
				if (reference_counter->unreference() == 0)
				{
					delete(pointer);
					delete(reference_counter);
				}

				pointer = data_pointer;
				reference_counter = new PointerReferenceCounter();
				reference_counter->reference();
			}

            return *this;
        }


		typename PointerReferenceCounter::CounterType referenceCount() const { return reference_counter->reference_count; }

		//
		// Inheritance Support
		//

		template<class ConversionType>
		operator SharedPointer<ConversionType>() // implicit conversion operators.
		{
			return SharedPointer<ConversionType>(pointer, reference_counter);
		}

		//
		// This should never be called explicitly
		//
		SharedPointer(PointerType data_pointer, PointerReferenceCounter* counter) 
		{
			pointer = data_pointer;
			reference_counter = counter;
			reference_counter->reference();
		}
		PointerReferenceCounter* referenceCounter() const { return reference_counter; }

	private:
		PointerReferenceCounter* reference_counter;
		PointerType pointer;
	};


	template<class T>
	void swap(SharedPointer<T> &shared_pointer, SharedPointer<T> &another_shared_pointer) 
	{
		shared_pointer.swap(another_shared_pointer);
	}

	template<class T>
	T* get_pointer(SharedPointer<T> const& shared_pointer) 
	{
		return shared_pointer.get(); 
	}

	template<class T, class Y>
	SharedPointer<T> const& static_pointer_cast(SharedPointer<Y> const& shared_pointer)
	{ 
		return SharedPointer<T>( static_cast<T*>(shared_pointer.get()), shared_pointer.referenceCounter());
	}

	template<class T, class Y>
	SharedPointer<T> dynamic_pointer_cast(SharedPointer<Y> const& shared_pointer) 
	{
		T* pointer = dynamic_cast<T*>(shared_pointer.get());
		return (pointer ? SharedPointer<T>(pointer, shared_pointer.referenceCounter()) : SharedPointer<T>() );
	}

	template<class T, class Y>
	SharedPointer<T> const_pointer_cast(SharedPointer<Y> const& shared_pointer)
	{ 
		return SharedPointer<T>( const_cast<T*>(shared_pointer.get()), shared_pointer.referenceCounter());
	}
};


#endif //__SMOKIN_SHARED_POINTER__
