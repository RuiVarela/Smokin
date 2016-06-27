/*
    Smokin::SystemTimer - SystemTimer.
    Copyright (C) 2007  Rui Varela - rui.filipe.varela@gmail.com

	This file is part of Smokin::SystemTimer.

    Smokin::SystemTimer is free software: you can redistribute it and/or modify
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

#include "SystemTimer.h"

#include <list>
#include <windows.h>

void CALLBACK timerCallback(HWND hParent, UINT uMsg, UINT uEventID, DWORD dwTimer);

typedef std::list<Smokin::SystemTimer::TimerData*> SystemTimers;

static SystemTimers system_timers;

struct Smokin::SystemTimer::TimerData
{
	TimerData(Smokin::SystemTimer* parent)
		: timer(0)
	{
		system_timer = parent;
		system_timers.push_back(this);
	}

	~TimerData()
	{
		bool erased = false;
		SystemTimers::iterator iterator = system_timers.begin();
		
		while( ! ( (iterator == system_timers.end()) || erased ) )
		{
			if ( (*iterator)->timer == timer )
			{
				iterator = system_timers.erase(iterator);
				erased = true;
			}
			else
			{
				++iterator;
			}
		}
	}

	void setTimerDisabled()
	{
		system_timer->cancel();
	}

	UINT timer;
	Smokin::SystemTimer* system_timer;
};

void CALLBACK timerCallback(HWND hParent, UINT uMsg, UINT uEventID, DWORD dwTimer)
{
		bool found = false;
		SystemTimers::iterator iterator = system_timers.begin();
		
		while( ! ( (iterator == system_timers.end()) || found ) )
		{
			if ( (*iterator)->timer == uEventID )
			{
				found = true;
			}
			else
			{
				++iterator;
			}
		}

	(*iterator)->setTimerDisabled();
	(*iterator)->system_timer->onTimeOut();
}

Smokin::SystemTimer::SystemTimer()
{
	enabled_ = false;
	data_ = new TimerData(this);
}

Smokin::SystemTimer::SystemTimer(double time_out)
{
	enabled_ = false;
	data_ = new TimerData(this);
	setTimer(time_out);
}

Smokin::SystemTimer::~SystemTimer()
{
	cancel();
	delete(data_);
}

void Smokin::SystemTimer::cancel()
{
	if (enabled_)
	{
		KillTimer(0, data_->timer);
		enabled_ = false;
	}
}

void Smokin::SystemTimer::setTimer(double time_out)
{
	cancel();
	time_out_ = time_out;
	enabled_ = true;
	
	data_->timer = SetTimer(0, UINT_PTR(0), UINT(time_out_ * 1000), timerCallback);
}