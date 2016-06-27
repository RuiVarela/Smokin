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

#include "Filter.h"
#include "RandomConnector.h"

#include <iostream>

using namespace std;
using namespace Smokin;

int main(int argc, char* argv[])
{
	/*
	*
	* Connectors Test
	*
	*/

	DirectLinkConnector<int> int_link;

	for (int i = 0; i != 10; ++i)
	{
		int_link.set(i);
		std::cout << "DirectLinkConnector<Int>: " << int_link() << std::endl;
	}

	/*
	*
	* AddFilter Test
	*
	*/

	DirectLinkConnector<int>* one_link = new DirectLinkConnector<int>();
	DirectLinkConnector<int>* another_link = new DirectLinkConnector<int>();
	DirectLinkConnector<int>* another_another_link = new DirectLinkConnector<int>();

	AddFilter<int>* add_filter = new AddFilter<int>();
	AddFilter<int>* another_add_filter = new AddFilter<int>();

	add_filter->addInput(one_link);
	add_filter->addInput(another_link);

	another_add_filter->addInput(add_filter);
	another_add_filter->addInput(another_another_link);


	one_link->set(56);
	another_link->set(6);
	another_another_link->set(3);
	std::cout << "AddFilter<int>: " << another_add_filter->operator ()() << std::endl;

	delete ( add_filter );
	delete ( one_link );
	delete ( another_link );

	/*
	*
	* CircularBuffer Test
	*
	*/

	CircularBuffer<int> circular_buffer(10);

	for (unsigned int i = 0; i != 12; ++i)
	{
		std::cout << "Circular Buffer" << std::endl;
		std::cout << "Pushing Value : " << i << std::endl;
		circular_buffer.push_back(i);

		std::cout << "Capacity : " << circular_buffer.capacity() << " Size : " << circular_buffer.size() << std::endl;

		for (unsigned int index = 0; index != circular_buffer.size(); ++index)
		{
			std::cout << "Index : " << index << " - " << circular_buffer[index] << std::endl;
		}
		std::cout << " Front : " << circular_buffer.front() << std::endl;
		std::cout << " Back : " << circular_buffer.back() << std::endl;
	}

	for (unsigned int i = 0; i != 5; ++i)
	{
		std::cout << "Circular Buffer" << std::endl;
		std::cout << "Popping Value : " << std::endl;
		circular_buffer.pop_front();

		std::cout << "Capacity : " << circular_buffer.capacity() << " Size : " << circular_buffer.size() << std::endl;

		for (unsigned int index = 0; index != circular_buffer.size(); ++index)
		{
			std::cout << "Index : " << index << " - " << circular_buffer[index] << std::endl;
		}
		std::cout << " Front : " << circular_buffer.front() << std::endl;
		std::cout << " Back : " << circular_buffer.back() << std::endl;
	}

	/*
	*
	* IncreaseAndDecayFilterBox Test
	*
	*/

	IncreaseAndDecayFilterBoxFloat increase_and_decay_filter_box(0.2f, 0.6f);

	for(unsigned int i = 0; i != 20; ++i)
	{
		increase_and_decay_filter_box.boxInput0()->set( float(i) );
		std::cout << "i : " << i << " Filtered Value : " << increase_and_decay_filter_box() << std::endl;
	}

	/*
	*
	* MedianFilter Test
	*
	*/

	std::cout << "MedianFilter Test " << std::endl;

	// Enable "Randomness"
	Smokin::StartRand();

	UniformGeneratorConnector<float> *uniform_connector = new UniformGeneratorConnector<float>();

	MedianFilter<float> median_filter;
	median_filter.addInput(uniform_connector);
	median_filter.setMaximumNumberOfSamples(1);

	for (unsigned int i = 0; i != 15; ++i)
	{
		std::cout << "Iteration: " << i << " Sample: " << median_filter() << std::endl;
	}

	delete(uniform_connector);

	std::cout << "MedianFilterBox Test " << std::endl;
	MedianFilterBoxFloat median_filter_box;
	median_filter_box.setMaximumNumberOfSamples(5);

	for (unsigned int i = 0; i != 15; ++i)
	{
		median_filter_box.boxInput0()->set( float(i) );
		std::cout << "Iteration: " << i << " Sample: " << median_filter_box() << std::endl;
	}




	char c;
	cin.get(c);

	return 0;
}

