#include <cmath>
#include <string>
#include <iostream>
#include <vector>

#include "SoundShouter.h"
#include "../Shouter/SoundShouter.h"

int main(int argc, char* argv[])
{

	WAVEFORMATEX m_WavFormat;
	m_WavFormat.wBitsPerSample  = 16;
	m_WavFormat.wFormatTag      = WAVE_FORMAT_PCM;
	m_WavFormat.nChannels       = 1;
	m_WavFormat.nSamplesPerSec  = 44100;
	m_WavFormat.nAvgBytesPerSec = (DWORD) m_WavFormat.nChannels * m_WavFormat.nSamplesPerSec * 2;
	m_WavFormat.nBlockAlign     = (WORD) (m_WavFormat.nChannels << 1);
	m_WavFormat.cbSize          = 0;

	Smokin::WaveOut wave_out(&m_WavFormat, 
							 Smokin::default_number_of_stream_buffers,
							 Smokin::default_stream_buffer_size);

	float sin_position = 0.f;
	while (true)
	{

#define	TWO_PI			(3.1415926f * 2.f)
#define	SIN_STEP		((TWO_PI * 440.f) / 44100.f)

		unsigned int const number_of_samples = 44100;
		signed short samples[number_of_samples];
		signed short *current_sample = &samples[0];

		for (unsigned int i = 0; i < number_of_samples; i++)
		{
			*current_sample++ = (signed short)(16384.f * sin(sin_position));
			sin_position += SIN_STEP;
		}

		if (sin_position >= TWO_PI) 
			sin_position -= TWO_PI;

		unsigned int total_bytes = (sizeof(signed short) / sizeof(char)) * number_of_samples;

		wave_out.Write( (PBYTE) samples, total_bytes );
		//Sleep(5000);
	}

	return 0;
}

