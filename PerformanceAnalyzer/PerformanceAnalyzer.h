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

#ifndef __SMOKIN_PERFORMANCE_ANALYZER__
#define __SMOKIN_PERFORMANCE_ANALYZER__

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cassert>
#include <limits>

#include "Timer.h"

//#define TIMMING_SCALE(timer) timer.time_s() /* Timming in seconds. */
#define TIMMING_SCALE(timer) timer.time_m() /* Timming in milliseconds. */
//#define TIMMING_SCALE(timer) timer.time_u() /* Timming in microseconds. */
//#define TIMMING_SCALE(timer) timer.time_n() /* Timming in nanoseconds. */

namespace Smokin
{
	class PerformanceAnalyzer
	{
	public :
		class Query;
		struct QueryData;

		typedef std::vector<QueryData> QueryResultSet;
		typedef std::map<std::string, QueryResultSet> QueryResultSetMap;

	public:
		PerformanceAnalyzer() {}
		~PerformanceAnalyzer() {}

		static double currentTime() { return TIMMING_SCALE(timer_); }

		static void beginTimming() { timer_.setStartTick(); }

		static unsigned int numberOfQueries() { return (unsigned int)(queries_.size()); }

		static void addQueryResult(Query const& query)
		{
			QueryResultSetMap::iterator query_map_position = queries_.find( query.identifier() );

			if (query_map_position == queries_.end())
			{
				queries_[ query.identifier() ] = QueryResultSet();
				queries_[ query.identifier() ].push_back( query.queryData() );
			}
			else
			{
				query_map_position->second.push_back(query.queryData() );
			}
		}

		static std::string queryIdentifier(unsigned int index)
		{
			assert( index < numberOfQueries() );
			assert( index >= 0 );

			unsigned int counter = 0;
			std::string identifier;

			QueryResultSetMap::iterator iterator = queries_.begin();
			while(iterator != queries_.end())
			{
				if (counter == index)
				{
					identifier = iterator->first;
					break;
				}

				++iterator;
				++counter;
			}

			return identifier;
		}

		static bool hasQuery(std::string const& query_identifyer)
		{
			QueryResultSetMap::iterator query_map_position = queries_.find( query_identifyer );
			return (query_map_position != queries_.end());
		}

		static QueryResultSet const& queryResultSet(std::string const query_identifyer)
		{
			assert( hasQuery(query_identifyer) );
			return queries_[ query_identifyer ];
		}

		template <typename T>
		static void analyzeQuery(std::string const& query_identifyer, T & analyzer)
		{
			assert( hasQuery(query_identifyer) );

			QueryResultSetMap::iterator query = queries_.find( query_identifyer );
			analyzer.begin(query->first);

			for (QueryResultSet::size_type i = 0; i != query->second.size(); ++i)
			{
				analyzer.operator() ( query->second[i] );
			}

			analyzer.end();
		}

		template <typename T>
		static void analyzeQueries(T & analyzer)
		{
			QueryResultSetMap::iterator iterator = queries_.begin();
			while(iterator != queries_.end())
			{
				analyzer.begin(iterator->first);

				for (QueryResultSet::size_type i = 0; i != iterator->second.size(); ++i)
				{
					analyzer.operator() ( iterator->second[i] );
				}

				analyzer.end();

				++iterator;
			}
		}

	private:
		static Timer timer_;
		static QueryResultSetMap queries_;

	public:

		struct QueryData
		{
			QueryData()
				:begin_time(0.0), end_time(0.0)
			{}
			double begin_time;
			double end_time;
		};

		class Query
		{
		public :
			Query() {}

			Query(std::string const &identifier)
				:identifier_(identifier)
			{}
			virtual ~Query() {};

			virtual void start() { query_data.begin_time = PerformanceAnalyzer::currentTime(); }
			virtual void end() 
			{
				query_data.end_time = PerformanceAnalyzer::currentTime(); 
				PerformanceAnalyzer::addQueryResult(*this);
			}

			std::string const& identifier() const { return identifier_; } 
			double startTime() const { return query_data.begin_time; }
			double endTime() const { return query_data.end_time; }
			QueryData const& queryData() const { return query_data; }
		private:
			std::string identifier_;
			QueryData query_data;
		};

		class DelayedQuery : public Query
		{
		public :
			DelayedQuery(std::string const &identifier,
				double const first_time = std::numeric_limits<double>::min(),
				double const last_time = std::numeric_limits<double>::max())
				:Query(identifier), first_time_(first_time), last_time_(last_time), do_timming_(false)
			{}

			virtual void start() 
			{
				double time = PerformanceAnalyzer::currentTime();

				do_timming_ = ((time >= first_time_) && (time <= last_time_));

				if (do_timming_)
				{
					Query::start();
				}
			}

			virtual void end() 
			{
				if (do_timming_)
				{
					Query::end();
				}
			}

		private :
			double first_time_;
			double last_time_;
			bool do_timming_;
		};

		class ToggleQuery : public Query
		{
		public :
			ToggleQuery() {}

			ToggleQuery(std::string const &identifier)
				:Query(identifier), do_start_(true)
			{}

			virtual void toggle()
			{
				if (do_start_)
				{
					start();
					do_start_ = false;
				}
				else
				{
					end();
					start();
				}
			}

		private :
			bool do_start_;
		};

		class ScopedQuery : public Query
		{
		public :
			ScopedQuery(std::string const &identifier)
				:Query(identifier)
			{
				start(); 
			}

			virtual ~ScopedQuery() { end(); }
		};

		class ScopedDelayedQuery : public DelayedQuery
		{
		public :
			ScopedDelayedQuery(std::string const &identifier,
				double const first_time = std::numeric_limits<double>::min(),
				double const last_time = std::numeric_limits<double>::max())
				:DelayedQuery(identifier, first_time, last_time)
			{
				start(); 
			}

			virtual ~ScopedDelayedQuery() { end(); }
		};

		class MarkerQuery 
		{
		public :
			typedef std::map<std::string, ToggleQuery> ToggleQueryMap;

			MarkerQuery(std::string const &identifier)
			{
				ToggleQueryMap::iterator element = toggle_queries_.find( identifier );

				if (element == toggle_queries_.end())
				{
					ToggleQuery toggle_query(identifier);
					toggle_query.toggle();

					toggle_queries_[identifier] = toggle_query;
				}
				else
				{
					element->second.toggle();
				}
			}

			~MarkerQuery() {}
		private:
			static ToggleQueryMap toggle_queries_;
		};

		

	};

};

#endif //__SMOKIN_PERFORMANCE_ANALYZER__
