/*
    Smokin::SoundShouter - c++ Encapsulations for win32 WaveOut.
    Copyright (C) 2007  Rui Varela - rui.filipe.varela@gmail.com

	This file is part of Smokin::SoundShouter.

    Smokin::SoundShouter is free software: you can redistribute it and/or modify
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
 * Note, elements of this FileUtil have been copied directly from CDex ripper.
 * These elements are licensed under GPL as above, with Copyright (C) 1999-2002 Albert L. Faber.
 */

#pragma once


#include <windows.h>
#include <Mmsystem.h>

#pragma comment(lib, "Winmm.lib")

namespace Smokin
{
	static unsigned int const default_number_of_stream_buffers = 8;//45;
	static unsigned int const default_stream_buffer_size = 16384;//65536;

	class WaveBuffer 
	{
	public:
		WaveBuffer();
		~WaveBuffer();
		BOOL Init( HWAVEOUT hWave, int Size );
		void Done();
		BOOL Write( PBYTE pData, int nBytes, int& BytesWritten );
		void Flush();
	private:
		WAVEHDR      m_Hdr;
		HWAVEOUT     m_hWave;
		int          m_nBytes;
	};

	class WaveOut 
	{
	public:
		WaveOut(unsigned int rate, unsigned int channels, unsigned int bits_per_sample);
		WaveOut( LPCWAVEFORMATEX Format, int nBuffers, int BufferSize );
		~WaveOut();
		void Write( PBYTE Data, int nBytes );
		void Flush();
		void WaitToFinish();
		void Reset();
		void Abort() { m_bAbort = TRUE; }
		void Pause();
		void Resume();
		HWAVEOUT GetHwave() const { return m_hWave; }
	private:
		const HANDLE	m_hSem;
		const int		m_nBuffers;
		int				m_CurrentBuffer;
		BOOL			m_NoBuffer;
		WaveBuffer*		m_WavBuffer;
		HWAVEOUT		m_hWave;
		BOOL			m_bAbort;
		BOOL			m_bPaused;
	};
};