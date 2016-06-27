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
#ifndef __SMOKIN_RANDOM_CONNECTOR_H__
#define __SMOKIN_RANDOM_CONNECTOR_H__

#include "Connector.h"
#include "NewranLib/newran.h"


namespace Smokin
{
	/*
	*	
	* Start "Randomness"
	* 
	* Randomness uses a MotherOfAll Uniform Generator
	* seed = [0,1]
	*
	*/

	void StartRand(double seed = 0.9375)
	{
		static NEWRAN::MotherOfAll urng(seed);
		NEWRAN::Random::Set(urng);
	};

	/*
	*	
	* UniformGeneratorOutputConnector
	* 
	* Normal Distribution
	*
	*/

	template <typename RT>
	class UniformGeneratorConnector: public OutputConnector<RT>
	{
	public:
		UniformGeneratorConnector() {}
		virtual ~UniformGeneratorConnector() {}

		virtual ReturnType operator()()	
		{
			return (ReturnType)( distribution.Next() );
		}

	private:
		NEWRAN::Uniform distribution;
	};

};

#endif //__SMOKIN_RANDOM_CONNECTOR_H__