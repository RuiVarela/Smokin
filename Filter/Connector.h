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

#ifndef __SMOKIN_CONNECTOR_H__
#define __SMOKIN_CONNECTOR_H__

namespace Smokin
{
	/*
	*	
	* OutputConnector
	* 
	* Base class for Links / Connectors
	* A OutputConnector ONLY outputs values
	*
	*/

	template <typename RT>
	class OutputConnector
	{
	public:
		typedef RT ReturnType;

		OutputConnector() {}
		virtual ~OutputConnector() {}
		virtual ReturnType operator()() = 0;
	};

	/*
	*	
	* Connector
	* 
	* Base class for Input/Output connectors
	* A Connector has one Input
	*
	*/

	template <typename RT, typename PT = RT>
	class Connector : public OutputConnector<RT>
	{
	public:
		static int const number_of_parameters = 1;
		typedef PT ParameterType;   

		Connector() {}
		virtual ~Connector() {}

		void set(ParameterType const& input_value) { value = input_value; }
		ParameterType const& get() const { return value; }
	protected:
		ParameterType value;
	};


	/* 
	*	DirectLinkConnector
	*
	*   Input Value in the connector is moved along the connection to the output.
	*
	*/

	template <typename RT, typename PT = RT>
	class DirectLinkConnector : public Connector<RT, PT>
	{
	public: 
		DirectLinkConnector() {}
		virtual ~DirectLinkConnector() {}
		virtual ReturnType operator()() { return value; }
	};

	typedef DirectLinkConnector<int> DirectLinkConnectorInt;
	typedef DirectLinkConnector<float> DirectLinkConnectorFloat;
	typedef DirectLinkConnector<double> DirectLinkConnectorDouble;
};

#endif //__SMOKIN_CONNECTOR_H_
