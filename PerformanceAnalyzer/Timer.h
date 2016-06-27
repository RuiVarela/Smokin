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

/* 
* Note, elements of Timer have been copied directly from openscenegraph Timer class www.openscenegraph.org .
* These elements are licensed under GPL (compatible with OSGPL) as above, with Copyright (C) 1998-2006 Robert Osfield.
*/

#ifndef __PERFORMANCE_TIMER__
#define __PERFORMANCE_TIMER__

namespace Smokin
{

#if defined(_MSC_VER)
	typedef __int64 Timer_t;
#else
	typedef unsigned long long int Timer_t;
#endif

	/** Timer class is used for measuring elapsed time or time between two points. */
	class Timer {
	public:

		Timer();
		~Timer() {}

		static Timer* instance();

		/** Get the timers tick value.*/
		Timer_t tick() const;

		/** Set the start.*/
		void setStartTick() { _startTick = tick(); }
		void setStartTick(Timer_t t) { _startTick = t; }
		Timer_t getStartTick() const { return _startTick; }


		/** Get elapsed time in seconds.*/
		inline double time_s() const { return delta_s(_startTick, tick()); }

		/** Get elapsed time in milliseconds.*/
		inline double time_m() const { return delta_m(_startTick, tick()); }

		/** Get elapsed time in micoseconds.*/
		inline double time_u() const { return delta_u(_startTick, tick()); }

		/** Get elapsed time in nanoseconds.*/
		inline double time_n() const { return delta_n(_startTick, tick()); }

		/** Get the time in seconds between timer ticks t1 and t2.*/
		inline double delta_s( Timer_t t1, Timer_t t2 ) const { return (double)(t2 - t1)*_secsPerTick; }

		/** Get the time in milliseconds between timer ticks t1 and t2.*/
		inline double delta_m( Timer_t t1, Timer_t t2 ) const { return delta_s(t1,t2)*1e3; }

		/** Get the time in microseconds between timer ticks t1 and t2.*/
		inline double delta_u( Timer_t t1, Timer_t t2 ) const { return delta_s(t1,t2)*1e6; }

		/** Get the time in nanoseconds between timer ticks t1 and t2.*/
		inline double delta_n( Timer_t t1, Timer_t t2 ) const { return delta_s(t1,t2)*1e9; }

		/** Get the the numer of ticks per second.*/
		inline double getSecondsPerTick() const { return _secsPerTick; }

	protected :

		Timer_t _startTick;
		double  _secsPerTick;
	};

};

#endif // __PERFORMANCE_TIMER__

