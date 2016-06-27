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

#include "DirectWaveOut.h"

#include <windows.h>
#include <Mmsystem.h>
#include <queue>
#include <string>
#include <vector>

#pragma comment(lib, "Winmm.lib")

#include <iostream>

//TODO last audio block on close/open when we call open on an already playing state, 
// can pass the block from the old waveout to the new waveout 

#ifdef NDEBUG

#define WaveLogger(msg)
#define mutexCheckError(state, message)
#define MmSystemCheckError(state, message)

#else //NDEBUG

#define WaveLogger(msg) std::cout << msg << std::endl

BOOL mutexCheckError(BOOL state, std::string message)
{
	if (!state)
	{
		WaveLogger(message << "Mutex error.");
	}

	return state;
}

bool mutexCheckError(DWORD result, std::string message)
{
	switch ( result )	
	{
	case WAIT_TIMEOUT : 
		WaveLogger(message << "The time-out interval elapsed, and the object's state is nonsignaled.");
		return false;
		break;
	case WAIT_FAILED : 
		WaveLogger(message << "Wait Failed.");
		return false;
		break;		
	}

	return true;
}

bool MmSystemCheckError(MMRESULT result, std::string message)
{
	if ( result != MMSYSERR_NOERROR )
	{
		switch ( result )	
		{
		case MMSYSERR_ALLOCATED : 
			WaveLogger(message << "Specified resource is already allocated.");
			break;
		case MMSYSERR_INVALHANDLE : 
			WaveLogger(message << "Specified device handle is invalid.");
			break;
		case MMSYSERR_NODRIVER : 
			WaveLogger(message << "No device driver is present.");
			break;
		case MMSYSERR_BADDEVICEID : 
			WaveLogger(message << "Specified device identifier is out of range.");
			break;
		case MMSYSERR_NOMEM : 
			WaveLogger(message << "Unable to allocate or lock memory.");
			break;
		case WAVERR_UNPREPARED : 
			WaveLogger(message << "Data block pointed to by WAVEHDR unprepared.");
			break;
		case WAVERR_BADFORMAT :
			WaveLogger(message << "Attempted to open with an unsupported waveform-audio format.");
			break;
		case WAVERR_SYNC : 
			WaveLogger(message << "Wave device is synchronous.");
			break;
		case MMSYSERR_NOTSUPPORTED : 
			WaveLogger(message << "Specified device is synchronous and does not support pausing.");
			break;
		default:
			WaveLogger(message << "Unknown return code " << result);
		}

		return false;
	}
	else
	{
		return true;
	}
}

#endif //NDEBUG



void CALLBACK waveCallback(HWAVEOUT hWave, UINT uMsg, DWORD_PTR dwUser, DWORD dw1, DWORD dw2)
{
	Smokin::DirectWaveOut* direct_wave_out = (Smokin::DirectWaveOut*) dwUser;

	switch (uMsg)
	{
	case WOM_DONE:
		{
			if (direct_wave_out->isPlaying() && !direct_wave_out->onReset())
			{
				direct_wave_out->releaseBlock();
				direct_wave_out->OnWaveBlockDone();
			}
		}
		break;

	case WOM_OPEN:
		direct_wave_out->OnWaveOpen();
		break;

	case WOM_CLOSE:
		direct_wave_out->OnWaveClose();
		break;

	default:
		break;

	}
}

struct AudioBlock
{
	AudioBlock() 
	{
		wave = 0;
		hdr.lpData = 0;
		do_not_unprepare = false;
	}

	~AudioBlock()
	{
		unprepare();
	}

	void prepare(HWAVEOUT wave, unsigned int size)
	{
		this->wave = wave;
	
		unprepare();

		hdr.lpData = (LPSTR) malloc(size);
		hdr.dwBufferLength = size;

		MMRESULT result = waveOutPrepareHeader(wave, &hdr, sizeof( WAVEHDR ) );
		MmSystemCheckError(result, "waveOutPrepareHeader : ");
	}

	void unprepare()
	{
		if (hdr.lpData)
		{
			if (!do_not_unprepare)
			{
				MMRESULT result = waveOutUnprepareHeader(wave, &hdr, sizeof(WAVEHDR));
				MmSystemCheckError(result, "waveOutUnprepareHeader : ");
			}

			free( (void*) hdr.lpData);
			hdr.lpData = 0;
		}

		hdr.dwBufferLength  = 0;
		hdr.dwBytesRecorded = 0;
		hdr.dwUser = 0;
		hdr.dwFlags = 0;
		hdr.dwLoops = 0;
		hdr.lpNext = 0;
		hdr.reserved = 0;
	}

	void write(unsigned char* data)
	{
		memcpy(hdr.lpData, data, hdr.dwBufferLength);

		MMRESULT result = waveOutWrite(wave, &hdr, sizeof(WAVEHDR));
		MmSystemCheckError(result, "waveOutWrite : ");
	}

	HWAVEOUT wave;
	WAVEHDR hdr;
	bool do_not_unprepare;
};

struct Smokin::DirectWaveOut::DirectWaveOutData
{
	DirectWaveOutData()
	{
		handle_wave_out = 0;

		mutex = CreateMutex(0, FALSE, 0);
		mutexCheckError(mutex != NULL, "DirectWaveOutData CreateMutex : ");

		bytes_counter_mutex = CreateMutex(0, FALSE, 0);
		mutexCheckError(bytes_counter_mutex != NULL, "DirectWaveOutData CreateMutex : ");
	}

	~DirectWaveOutData()
	{
		DWORD result = WaitForSingleObject(mutex, (DWORD)0);
		mutexCheckError(result, "~DirectWaveOutData WaitForSingleObject : ");
		CloseHandle(mutex);

		result = WaitForSingleObject(bytes_counter_mutex, (DWORD)0);
		mutexCheckError(result, "~DirectWaveOutData WaitForSingleObject : ");
		CloseHandle(bytes_counter_mutex);
	}

	HWAVEOUT handle_wave_out;
	HANDLE mutex;
	HANDLE bytes_counter_mutex;

	std::queue<AudioBlock*> audio_blocks;
};


Smokin::DirectWaveOut::DirectWaveOut()
:data_(new DirectWaveOutData())
{
	rate_ = 0;
	channels_ = 0;
	bits_per_sample_ = 0;
	bytes_per_second_ = 0;

	playing_ = false;

	on_reset_ = false;
}

Smokin::DirectWaveOut::~DirectWaveOut()
{
	close();

	delete(data_);
}

void Smokin::DirectWaveOut::open(unsigned int const rate, unsigned int const channels, unsigned int const bits_per_sample)
{
	if (isPlaying())
	{
		close();
	}

	DWORD mutex_result = WaitForSingleObject(data_->mutex, INFINITE);
	mutexCheckError(mutex_result, "WaitForSingleObject : ");

	rate_ = rate;
	channels_ = channels;
	bits_per_sample_ = bits_per_sample;

	bytes_per_second_  = rate_ * (bits_per_sample_ / 8) * channels_; 

	WAVEFORMATEX wave_format;
	wave_format.wBitsPerSample  = bits_per_sample_;
	wave_format.wFormatTag      = WAVE_FORMAT_PCM;
	wave_format.nChannels       = channels_;
	wave_format.nSamplesPerSec  = rate_;
	wave_format.nAvgBytesPerSec = (DWORD) wave_format.nChannels * wave_format.nSamplesPerSec * 2;
	wave_format.nBlockAlign     = (wave_format.wBitsPerSample * wave_format.nChannels) / 8 ;
	wave_format.cbSize          = sizeof(WAVEFORMATEX);

	MMRESULT result = waveOutOpen(&data_->handle_wave_out, WAVE_MAPPER, &wave_format, 
								 (DWORD_PTR) waveCallback, (DWORD_PTR) this, CALLBACK_FUNCTION | WAVE_ALLOWSYNC);

	MmSystemCheckError(result, "waveOutOpen : ");

	BOOL release_state = ReleaseMutex(data_->mutex);
	mutexCheckError(release_state, "ReleaseMutex : ");

	playing_ = true;
}

void Smokin::DirectWaveOut::close()
{
	DWORD result = WaitForSingleObject(data_->mutex, INFINITE);
	mutexCheckError(result, "WaitForSingleObject : ");

	playing_ = false;

	if (data_->handle_wave_out)
	{
		MMRESULT result = waveOutReset(data_->handle_wave_out);
		MmSystemCheckError(result, "waveOutReset : ");

		while(!data_->audio_blocks.empty())
		{
			AudioBlock* block = data_->audio_blocks.front();
			data_->audio_blocks.pop();
			delete(block);
		}

		waveOutClose(data_->handle_wave_out);
	}

	data_->handle_wave_out = 0;

	BOOL release_state = ReleaseMutex(data_->mutex);
	mutexCheckError(release_state, "ReleaseMutex : ");
}


void Smokin::DirectWaveOut::write(unsigned char* data, unsigned int size)
{
	DWORD result = WaitForSingleObject(data_->mutex, INFINITE);
	mutexCheckError(result, "WaitForSingleObject : ");

	if (isPlaying())
	{
		data_->audio_blocks.push( new AudioBlock() );
		data_->audio_blocks.back()->prepare(data_->handle_wave_out, size);
		data_->audio_blocks.back()->write(data);
	}

	on_reset_ = false;

	BOOL release_state = ReleaseMutex(data_->mutex);
	mutexCheckError(release_state, "ReleaseMutex : ");
}

void Smokin::DirectWaveOut::OnWaveOpen()
{
}

void Smokin::DirectWaveOut::OnWaveClose()
{
}

void Smokin::DirectWaveOut::OnWaveBlockDone()
{
}

void Smokin::DirectWaveOut::releaseBlock()
{
	DWORD result = WaitForSingleObject(data_->mutex, INFINITE);
	mutexCheckError(result, "WaitForSingleObject : ");

	if ( !data_->audio_blocks.empty() )
	{
		AudioBlock* block = data_->audio_blocks.front();
		data_->audio_blocks.pop();
		delete(block);
	}

	BOOL release_state = ReleaseMutex(data_->mutex);
	mutexCheckError(release_state, "ReleaseMutex : ");
}

unsigned long long int Smokin::DirectWaveOut::bytesPlayed()
{
	DWORD mutex_result = WaitForSingleObject(data_->mutex, INFINITE);
	mutexCheckError(mutex_result, "WaitForSingleObject : ");

	MMTIME mm_time; 
	mm_time.wType = TIME_BYTES;

	MMRESULT result = waveOutGetPosition(data_->handle_wave_out, &mm_time, sizeof(mm_time) );
	MmSystemCheckError(result, "waveOutGetPosition : ");

	BOOL release_state = ReleaseMutex(data_->mutex);
	mutexCheckError(release_state, "ReleaseMutex : ");

	return mm_time.u.cb;
}

unsigned int Smokin::DirectWaveOut::bufferedBlocks() const
{
	return (unsigned int) data_->audio_blocks.size();
}
