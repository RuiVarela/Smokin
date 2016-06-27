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

#ifndef __SMOKIN_FILTER_H__
#define __SMOKIN_FILTER_H__

#include <vector>

#include "CircularBuffer.h"
#include "Connector.h"

namespace Smokin
{
	/*
	*
	* Filter
	* Base Class For Filters
	*
	*/

	template <typename RT>
	class Filter: public OutputConnector<RT>
	{
	public:
		typedef OutputConnector<RT> FilterInput;

		Filter() : counter_(0) {}
		virtual ~Filter() {}

		void addInput(FilterInput *input) { inputs.push_back(input); }
		FilterInput* getInput(unsigned int input_number) { return inputs[input_number]; }
		unsigned int numberOfInputs() const { return (unsigned int) inputs.size(); }
		unsigned long int impulses() { return counter_; }
	protected:
		// Utility function to delete all inputs objects!
		// Filter will NOT call this function on destructor execution
		void deleteInputs()
		{
			while (!inputs.empty())
			{
				delete ( inputs.back() );
				inputs.pop_back();
			}
		}

		void increaseImpulseCount() { ++counter_; }

		std::vector<FilterInput*> inputs;
		unsigned long int counter_;
	};

	/*
	*
	* StateFilter
	* Base Class For Filters that have state 
	*
	* ST = StateType -> Instatiation of a "State Model" 
    *
	* State Model:
	*
	* A template model to implement state for filters. 
	* It "resembles" STL's vector usage. 
	* 
	*	ReferenceType operator[](size_type n)
	*	ConstReferenceType operator[](size_type n) const
	*	void reserve(size_type size)
	*	size_type capacity() const 
	*	size_type size() const 
	*	void clear()
	*   void push_back(ConstReferenceType element);
	*   ReferenceType front()
	*   ConstReferenceType front() const
	*   ReferenceType back()
	*   ConstReferenceType back() const
	*	size_type
	*/

	template <typename RT, typename ST = CircularBuffer<RT> >
	class SateFilter: public Filter<RT>
	{
	public:
		typedef ST StateType;
		typedef typename StateType::size_type size_type;

		void clearSamples() { samples.clear(); }

		void setMaximumNumberOfSamples(typename StateType::size_type number_of_samples) 
		{
			samples.reserve(number_of_samples);
		}

		size_type getMaximumNumberOfSamples() const
		{
			return samples.capacity();
		}

		size_type getNumberOfSamplesUsed() const
		{
			return samples.size();
		}
	protected:
		StateType samples;
	};


	/*
	*	
	* Simple operation filters
	*
	*/

	template <typename RT>
	struct AddFilter: public Filter<RT>
	{
		virtual ReturnType operator()()
		{
			ReturnType return_value;

			if (numberOfInputs())
			{
				return_value = getInput(0)->operator()();
				for (unsigned int i = 1; i != numberOfInputs(); ++i)
				{
					return_value += getInput(i)->operator()();
				}
			}

			return return_value;
		}
	};

	template <typename RT>
	struct SubtractFilter: public Filter<RT>
	{
		virtual ReturnType operator()()
		{
			ReturnType return_value;

			if (numberOfInputs())
			{
				return_value = getInput(0)->operator()();
				for (unsigned int i = 1; i != numberOfInputs(); ++i)
				{
					return_value -= getInput(i)->operator()();
				}
			}

			return return_value;
		}
	};

	template <typename RT>
	struct MultiplyFilter: public Filter<RT>
	{
		virtual ReturnType operator()()
		{
			ReturnType return_value;

			if (numberOfInputs())
			{
				return_value = getInput(0)->operator()();
				for (unsigned int i = 1; i != numberOfInputs(); ++i)
				{
					return_value *= getInput(i)->operator()();
				}
			}

			return return_value;
		}
	};

	/*
	*	
	* IncreaseAndDecayFilter
	* 
	* A smoothing filter. 
	* Uses a internal accumulator that is increased by a faction of the input value (increase factor) 
	* at each impulse. Then accumulator suffers a decay (decay factor).
	*
	* Uses 1 Input
	*/

	template <typename RT>
	class IncreaseAndDecayFilter: public Filter<RT>
	{
	public:
		IncreaseAndDecayFilter() { }
		IncreaseAndDecayFilter(ReturnType const& increase, ReturnType const& decay)
			:increase_factor(increase), decay_factor(decay)
		{}

		virtual ~IncreaseAndDecayFilter() {}

		ReturnType const& getIncreaseFactor() const { return increase_factor; }
		ReturnType const& getDecayFactor() const { return decay_factor; }
		void setIncreaseFactor(ReturnType const& factor){ increase_factor = factor; }
		void setDecayFactor(ReturnType const& factor){ decay_factor = factor; }

		virtual ReturnType operator()()
		{
			assert(numberOfInputs() == 1);

			if (impulses())
			{
				accumulator += getInput(0)->operator()() * increase_factor;
			}
			else
			{
				accumulator = getInput(0)->operator()();
			}

			increaseImpulseCount();

			ReturnType return_value = accumulator;
			accumulator *= decay_factor;

			return return_value;
		}
	private:
		ReturnType accumulator;
		ReturnType increase_factor;
		ReturnType decay_factor;
	};


	/*
	*	
	* MedianFilter
	* 
	* A smoothing filter. 
	* Eache impulse response is the median of the current NumberOfSamplesUsed
	*
	* Uses 1 Input
	*/

	template <typename RT, typename ST = CircularBuffer<RT> >
	class MedianFilter: public SateFilter<RT, ST>
	{
	public:
		MedianFilter() {}
		virtual ~MedianFilter() {}

		virtual ReturnType operator()()
		{
			assert(numberOfInputs() == 1);

			ReturnType input_value = getInput(0)->operator()();
			ReturnType return_value = input_value;

			for(size_type iterator = 0; iterator != samples.size(); ++iterator)
			{
				return_value += samples[iterator];
			}
			return_value /= ( 1 + samples.size() );
			samples.push_back( input_value );
			return return_value;
		}
	};


	/*
	*	
	* Filter Box
	* 
	* A filter box is a ready to run filter unit
	* You don't need to setup input connectors to this kind of filters, all is already done.
	*
	*/


	/*
	*	
	* IncreaseAndDecayFilterBox
	* 
	* FilterBox version of IncreaseAndDecayFilter
	*
	*/

	template <typename RT>
	class IncreaseAndDecayFilterBox: public IncreaseAndDecayFilter<RT>
	{
	public:
		IncreaseAndDecayFilterBox()
			: input(0)
		{ 
			initialize(); 
		}

		IncreaseAndDecayFilterBox(ReturnType const& increase, ReturnType const& decay)
			:IncreaseAndDecayFilter<RT>(increase, decay), input(0)
		{
			initialize();
		}

		virtual ~IncreaseAndDecayFilterBox() { deleteInputs(); }

		DirectLinkConnector<RT>* boxInput0() { return input; }
	private:
		void initialize()
		{
			input = new DirectLinkConnector<RT>();
			addInput(input);
		}
		DirectLinkConnector<RT> *input;
	};

	typedef IncreaseAndDecayFilterBox<float> IncreaseAndDecayFilterBoxFloat;
	typedef IncreaseAndDecayFilterBox<double> IncreaseAndDecayFilterBoxDouble;

	/*
	*	
	* MedianFilterBox
	* 
	* FilterBox version of MedianFilter
	*
	*/

	template <typename RT, typename ST = CircularBuffer<RT> >
	class MedianFilterBox: public MedianFilter<RT, ST>
	{
	public:
		MedianFilterBox()
			: input(0)
		{ 
			initialize(); 
		}

		virtual ~MedianFilterBox() { deleteInputs(); }
		DirectLinkConnector<RT>* boxInput0() { return input; }
	private:
		void initialize()
		{
			input = new DirectLinkConnector<RT>();
			addInput(input);
		}
		DirectLinkConnector<RT> *input;
	};

	typedef MedianFilterBox<float> MedianFilterBoxFloat;
	typedef MedianFilterBox<double> MedianFilterBoxDouble;

};
#endif __SMOKIN_FILTER_H_

