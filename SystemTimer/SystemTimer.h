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

#ifndef __SMOKIN_SYSTEM_TIMER__
#define __SMOKIN_SYSTEM_TIMER__

namespace Smokin
{
	class AbstractTimer
	{
	public:
		AbstractTimer() : time_out_(0), enabled_(false) {}
		virtual ~AbstractTimer() {}

		bool isEnabled() const { return enabled_; }
		double timeOut() const { return time_out_; }

		virtual void cancel() = 0;
		virtual void setTimer(double time_out) = 0;
		virtual void onTimeOut() = 0;
	protected:
		double time_out_;
		bool enabled_;
	};

	class UpdatableTimer : public AbstractTimer
	{
	public:
		UpdatableTimer() : time_clock(0.0), last_fire(0.0) {}

		void update(double time) 
		{
			time_clock = time; 
			if ( enabled_ && (time_clock >= (last_fire + timeOut())) )
			{
				enabled_ = false;
				onTimeOut();
			}
		}

		virtual void cancel() { enabled_ = false; }
		virtual void setTimer(double time_out) 
		{ 
			time_out_ = time_out;
			last_fire = time_clock; 
			enabled_ = true;
		}

		virtual void onTimeOut() = 0;
	private:
		double time_clock;
		double last_fire;
	};

	class SystemTimer : public AbstractTimer
	{
	public:
		struct TimerData;

		SystemTimer();
		SystemTimer(double time_out);

		virtual ~SystemTimer();
		virtual void cancel();
		virtual void setTimer(double time_out);
		virtual void onTimeOut() = 0;

	private:
		TimerData* data_;
		friend TimerData;
	};
};

#endif //__SMOKIN_SYSTEM_TIMER__