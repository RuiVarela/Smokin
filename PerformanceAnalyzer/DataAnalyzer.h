/*
	Smokin::PerformanceAnalyzer - Code Performance Analyzer.
	Copyright (C) 2007  Rui Varela - rui.filipe.varela@gmail.com

	This file is part of Smokin::PerformanceAnalyzer.

	Smokin::PerformanceAnalyzer is free software: you can redistribute it and/or modify
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

#ifndef __SMOKIN_DATA_ANALYZER__
#define __SMOKIN_DATA_ANALYZER__

#include "PerformanceAnalyzer.h"

#include <ostream>
#include <vector>
#include <iostream>
#include <limits>

namespace Smokin
{

//
//	Model DataAnalyzer
//
//	void begin() called before a query analysis
//	void end() called after a query analysis
//	void operator() (Smokin::PerformanceAnalyzer::QueryData const& query_data) // called for each data element in a query data set
//

	struct QueryCounterDataAnalyzer
	{
		typedef std::pair<std::string, int> StringIntPair;
		QueryCounterDataAnalyzer(){}

		void begin(std::string const& query_identifyer)
		{
			counter.push_back( StringIntPair(query_identifyer, 0) );		
		}

		void end() {}

		void operator() (Smokin::PerformanceAnalyzer::QueryData const& query_data) 
		{ 
			counter.back().second++; 
		}

		std::vector<StringIntPair> counter;
	};

	struct StatsDataAnalyzer
	{
		struct StatsData
		{
			StatsData()
			{
				max_time = std::numeric_limits<double>::min();
				min_time = std::numeric_limits<double>::max();
				average_time = 0.0;
				total_time = 0.0;
				begin_time = std::numeric_limits<double>::max();
				end_time = std::numeric_limits<double>::min();
				counter = 0;
			}

			std::string identifyer;
			double max_time;
			double min_time;
			double average_time;
			double total_time;
			double begin_time;
			double end_time;
			unsigned int counter;

			Smokin::PerformanceAnalyzer::QueryData max_element;
			Smokin::PerformanceAnalyzer::QueryData min_element;

		};

		StatsDataAnalyzer(){}

		void begin(std::string const& query_identifyer)
		{
			StatsData stats;
			stats.identifyer = query_identifyer;
			querys.push_back(stats);		
		}

		void end() 
		{
			if (querys.back().counter > 0)
			{
				querys.back().average_time = querys.back().total_time / float(querys.back().counter);
			}
		}

		void operator() (Smokin::PerformanceAnalyzer::QueryData const& query_data) 
		{ 
			double element_time = query_data.end_time - query_data.begin_time;

			if (query_data.begin_time < querys.back().begin_time)
			{
				querys.back().begin_time = query_data.begin_time;
			}

			if (query_data.end_time > querys.back().end_time)
			{
				querys.back().end_time = query_data.end_time;
			}

			if (element_time < querys.back().min_time)
			{
				querys.back().min_time = element_time;
				querys.back().min_element = query_data;
			}

			if (element_time > querys.back().max_time)
			{
				querys.back().max_time = element_time;
				querys.back().max_element = query_data;
			}

			querys.back().total_time += element_time;
			querys.back().counter++; 
		}

		std::vector<StatsData> querys;
	};



	// Printers
	struct StatsPrinterDataAnalyzer : public Smokin::StatsDataAnalyzer
	{
		StatsPrinterDataAnalyzer(std::ostream& out = std::cout) 
			: os(out)
		{}

		void end()
		{
			StatsDataAnalyzer::end();

			os << "[" << querys.size() << "] " << querys.back().identifyer
				<< " Total Queries : " << querys.back().counter << std::endl;

			os << "[" << querys.size() << "] " << querys.back().identifyer
				<< " Running Time : [" << querys.back().begin_time << " - " << querys.back().end_time << "] " 
				<< querys.back().total_time << std::endl;

			os << "[" << querys.size() << "] " << querys.back().identifyer
				<< " Min : " << querys.back().min_time << " Max : " << querys.back().max_time << " " << " Average : " 
				<<  querys.back().average_time << std::endl;
		}

		std::ostream& os;
	};

	struct QueryCounterPrinterDataAnalyzer : public Smokin::QueryCounterDataAnalyzer
	{
		QueryCounterPrinterDataAnalyzer(std::ostream& out = std::cout) 
			: os(out)
		{}

		void end()
		{
			QueryCounterDataAnalyzer::end();
			os << "[" << counter.size() << "] " << counter.back().first
				<< " has " << counter.back().second << " elements." << std::endl;
		}

		std::ostream& os;
	};

	struct PrinterDataAnalyzer
	{
		PrinterDataAnalyzer(std::ostream& out = std::cout) 
			: os(out), count(0) 
		{}

		void begin(std::string const& query_identifyer)
		{
			identifyer = query_identifyer;
			count = 0;
		}

		void end() {}

		void operator() (Smokin::PerformanceAnalyzer::QueryData const& query_data) 
		{ 
			os << identifyer << " [" << count << "] " 
			   << query_data.end_time - query_data.begin_time
			   << " [" << query_data.begin_time << " - " << query_data.end_time << "]" <<  std::endl; 
			++count; 
		}

		std::ostream& os;
		int count;
		std::string identifyer;
	};
};

#endif //__SMOKIN_DATA_ANALYZER__