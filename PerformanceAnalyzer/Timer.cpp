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

#include <iostream>
#include <stdio.h>
#include <string.h>

#include "Timer.h"

// follows are the constructors of the Timer class, once version
// for each OS combination.  The order is WIN32, FreeBSD, Linux, IRIX,
// and the rest of the world.
//
// all the rest of the timer methods are implemented within the header.

Smokin::Timer* Smokin::Timer::instance()
{
    static Timer s_timer;
    return &s_timer;
}

#ifdef WIN32

    #include <sys/types.h>
    #include <fcntl.h>
    #include <windows.h>
    #include <winbase.h>
    Smokin::Timer::Timer()
    {
        LARGE_INTEGER frequency;
        if(QueryPerformanceFrequency(&frequency))
        {
            _secsPerTick = 1.0/(double)frequency.QuadPart;
        }
        else
        {
            _secsPerTick = 1.0;
			std::cerr << "Error: Timer::Timer() unable to use QueryPerformanceFrequency, " << std::endl;
            std::cerr << "timing code will be wrong, Windows error code: "<< GetLastError() << std::endl;
        }
        
        setStartTick();        
    }

    Smokin::Timer_t Smokin::Timer::tick() const
    {
        LARGE_INTEGER qpc;
        if (QueryPerformanceCounter(&qpc))
        {
            return qpc.QuadPart;
        }
        else
        {
            std::cerr <<"Error: Timer::Timer() unable to use QueryPerformanceCounter, " << std::endl;
            std::cerr <<"timing code will be wrong, Windows error code: " << GetLastError() << std::endl;
            return 0;
        }
    }

#else

    #include <sys/time.h>

   Smokin:: Timer::Timer( void )
    {
        _secsPerTick = (1.0 / (double) 1000000);

        setStartTick();        
    }

   Smokin:: Timer_t Timer::tick() const
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return ((Timer_t)tv.tv_sec)*1000000+(Timer_t)tv.tv_usec;
    }

#endif

