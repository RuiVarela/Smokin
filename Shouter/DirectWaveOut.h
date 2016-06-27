/*
    Smokin::DirectWaveOut - WaveOut Object with buffering support.
    Copyright (C) 2007  Rui Varela - rui.filipe.varela@gmail.com

	This file is part of Smokin::DirectWaveOut.

    Smokin::DirectWaveOut is free software: you can redistribute it and/or modify
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

#ifndef __SMOKIN_DIRECT_WAVE_OUT__
#define __SMOKIN_DIRECT_WAVE_OUT__

namespace Smokin
{
	class DirectWaveOut
	{
	public:
		DirectWaveOut();
		~DirectWaveOut();

		void open(unsigned int const rate, unsigned int const channels, unsigned int const bits_per_sample);
		void close();
		

		virtual void OnWaveOpen();
		virtual void OnWaveClose();
		virtual void OnWaveBlockDone();

		void write(unsigned char* data, unsigned int size);

		unsigned int rate() const { return rate_; }
		unsigned int channels() const { return channels_; }
		unsigned int bitsPerSample() const { return bits_per_sample_; }
		unsigned int bytesPerSecond() const { return bytes_per_second_; }

		bool isPlaying() { return playing_; }
		bool onReset() { return on_reset_; }
		
		unsigned long long int bytesPlayed();

		unsigned int bufferedBlocks() const;


		void releaseBlock();
	private:	
		
		void addBytesPlayed(unsigned int bytes);
	
		struct DirectWaveOutData;
		DirectWaveOutData* data_;

		unsigned int rate_;
		unsigned int channels_;
		unsigned int bits_per_sample_;
		unsigned int bytes_per_second_;

		bool playing_;
		bool on_reset_;
	};

};


#endif // __SMOKIN_DIRECT_WAVE_OUT__