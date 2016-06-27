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

#include "SoundShouter.h"

static LONG glLastCount = 0;

Smokin::WaveBuffer::WaveBuffer() 
{
	glLastCount = 0;
}

BOOL Smokin::WaveBuffer::Init(HWAVEOUT hWave, int Size)
{
    m_hWave  = hWave;
    m_nBytes = 0;

    /*  Allocate a buffer and initialize the header. */
    m_Hdr.lpData = (LPSTR)LocalAlloc( LMEM_FIXED, Size );
    
	if ( NULL == m_Hdr.lpData )
	{
        return FALSE;
    }

    m_Hdr.dwBufferLength  = Size;
    m_Hdr.dwBytesRecorded = 0;
    m_Hdr.dwUser = 0;
    m_Hdr.dwFlags = 0;
    m_Hdr.dwLoops = 0;
    m_Hdr.lpNext = 0;
    m_Hdr.reserved = 0;

    /*  Prepare it. */
    waveOutPrepareHeader( hWave, &m_Hdr, sizeof( WAVEHDR ) );

    return TRUE;
}

Smokin::WaveBuffer::~WaveBuffer()
{
    if ( m_Hdr.lpData )
	{
        waveOutUnprepareHeader(m_hWave, &m_Hdr, sizeof(WAVEHDR));
        LocalFree( m_Hdr.lpData );
    }
}

void Smokin::WaveBuffer::Flush()
{
    // ASSERT(m_nBytes != 0);
    m_nBytes = 0;
    waveOutWrite(	m_hWave, 
					&m_Hdr,
					sizeof( WAVEHDR ) );
}

BOOL Smokin::WaveBuffer::Write(PBYTE pData, int nBytes, int& BytesWritten)
{
    // ASSERT((DWORD)m_nBytes != m_Hdr.dwBufferLength);
    BytesWritten = min((int)m_Hdr.dwBufferLength - m_nBytes, nBytes);
    CopyMemory((PVOID)(m_Hdr.lpData + m_nBytes), (PVOID)pData, BytesWritten);
    m_nBytes += BytesWritten;
    if (m_nBytes == (int)m_Hdr.dwBufferLength) {
        /*  Write it! */
        m_nBytes = 0;
        waveOutWrite(m_hWave, &m_Hdr, sizeof(WAVEHDR));
        return TRUE;
    }
    return FALSE;
}



void CALLBACK WaveCallback(HWAVEOUT hWave, UINT uMsg, DWORD dwUser, 
                           DWORD dw1, DWORD dw2)
{
    if (uMsg == WOM_DONE)
	{
        ReleaseSemaphore((HANDLE)dwUser, 1, &glLastCount);
    }
}

/*
    WaveOut
*/
Smokin::WaveOut::WaveOut(unsigned int rate, unsigned int channels, unsigned int bits_per_sample) :
    m_nBuffers( default_number_of_stream_buffers ),
    m_CurrentBuffer( 0 ),
    m_NoBuffer( TRUE ),
    m_hSem( CreateSemaphore( NULL, default_number_of_stream_buffers, default_number_of_stream_buffers, NULL ) ),
    m_WavBuffer( new WaveBuffer[ default_number_of_stream_buffers ] ),
    m_hWave( NULL ),
	m_bAbort( FALSE ),
	m_bPaused( FALSE )
{

	WAVEFORMATEX wave_format;
	wave_format.wBitsPerSample  = bits_per_sample;
	wave_format.wFormatTag      = WAVE_FORMAT_PCM;
	wave_format.nChannels       = channels;
	wave_format.nSamplesPerSec  = rate;
	wave_format.nAvgBytesPerSec = (DWORD) wave_format.nChannels * wave_format.nSamplesPerSec * 2;
	wave_format.nBlockAlign     = (wave_format.wBitsPerSample * wave_format.nChannels) / 8 ;
	wave_format.cbSize          = sizeof(WAVEFORMATEX);

	glLastCount= m_nBuffers-1;

    /*  Create wave device. */
    waveOutOpen(	&m_hWave,
					WAVE_MAPPER,
					&wave_format,
					(DWORD)WaveCallback,
					(DWORD)m_hSem,
					CALLBACK_FUNCTION | WAVE_ALLOWSYNC);

	/*  Initialize the wave buffers. */
	for (int i = 0; i < m_nBuffers; i++)
	{
		m_WavBuffer[i].Init( m_hWave, default_stream_buffer_size );
	}
}

Smokin::WaveOut::WaveOut(LPCWAVEFORMATEX Format, int nBuffers, int BufferSize) :
    m_nBuffers( nBuffers ),
    m_CurrentBuffer( 0 ),
    m_NoBuffer( TRUE ),
    m_hSem( CreateSemaphore( NULL, nBuffers, nBuffers, NULL ) ),
    m_WavBuffer( new WaveBuffer[ nBuffers ] ),
    m_hWave( NULL ),
	m_bAbort( FALSE ),
	m_bPaused( FALSE )
{
	glLastCount= m_nBuffers-1;

    /*  Create wave device. */
    waveOutOpen(	&m_hWave,
					WAVE_MAPPER,
					Format,
					(DWORD)WaveCallback,
					(DWORD)m_hSem,
					CALLBACK_FUNCTION );

    /*  Initialize the wave buffers. */
    for (int i = 0; i < nBuffers; i++)
	{
        m_WavBuffer[i].Init( m_hWave, BufferSize );
    }
}


Smokin::WaveOut::~WaveOut()
{
    /*  First, get the buffers back. */
    waveOutReset( m_hWave );

    /*  Free the buffers. */
    delete [] m_WavBuffer;

    /*  Reset the device, just to be sure */
    waveOutReset( m_hWave );

    /*  Close the wave device. */
    waveOutClose(m_hWave);

    /*  Free the semaphore. */
    CloseHandle( m_hSem );
}

void Smokin::WaveOut::Flush()
{
    if (!m_NoBuffer)
	{
        m_WavBuffer[m_CurrentBuffer].Flush();

        m_NoBuffer = TRUE;

        m_CurrentBuffer = (m_CurrentBuffer + 1) % m_nBuffers;

    }
}

void Smokin::WaveOut::Reset()
{
    waveOutReset( m_hWave );

	// clear all pending buffers

}

void Smokin::WaveOut::Pause( )
{
    m_bPaused = TRUE;
}

void Smokin::WaveOut::Resume( )
{
    m_bPaused = FALSE;
}


void Smokin::WaveOut::Write( PBYTE pData, int nBytes )
{
    while ( nBytes != 0 )
	{

        /*  Get a buffer if necessary. */
        if ( m_NoBuffer )
		{
            WaitForSingleObject( m_hSem, INFINITE );
            m_NoBuffer = FALSE;
        }

        /*  Write into a buffer. */
        int nWritten;

		if ( !m_bPaused )
		{
			if ( m_WavBuffer[ m_CurrentBuffer ].Write( pData, nBytes, nWritten ) )
			{
				m_NoBuffer = TRUE;
				m_CurrentBuffer = (m_CurrentBuffer + 1) % m_nBuffers;
				nBytes -= nWritten;
				pData += nWritten;
			} else {
				// ASSERT(nWritten == nBytes);
				break;
			}
		}
		else
		{
			// when paused, through everything in the byte bucket
			// we have to re-seek anyways due to the reset
			nBytes = 0;
		}
    }
}


void Smokin::WaveOut::WaitToFinish()
{
	if ( FALSE == m_bAbort )
	{
		// Wait till all data has been played
		while ( glLastCount< (m_nBuffers-1) )
		{
			::Sleep(10);
		}
	}

	while ( waveOutClose(m_hWave) == WAVERR_STILLPLAYING )
	{
		::Sleep(10);
	}

}