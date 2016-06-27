/*
    Smokin::AcmDecoder - Win32 Audio Decoder for Acm codecs.
    Copyright (C) 2007  Rui Varela - rui.filipe.varela@gmail.com

	This file is part of Smokin::AcmDecoder.

    Smokin::AcmDecoder is free software: you can redistribute it and/or modify
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

#include "AcmDecoder.h"

#include <cassert>
#include <iostream>

#define LogError(msg) std::cout << msg << std::endl

Smokin::AcmDecoder::AcmDecoder(WAVEFORMATEX* format)
{
	assert(format);
	assert(format->nSamplesPerSec);

	memcpy((char*)&input_format, format, min(64, sizeof(WAVEFORMATEX) + format->cbSize));

    internal_format.nChannels		=	input_format.nChannels;
    internal_format.nSamplesPerSec	=	input_format.nSamplesPerSec;
    internal_format.nAvgBytesPerSec	=	2 * internal_format.nSamplesPerSec * internal_format.nChannels;
    internal_format.wFormatTag		=	WAVE_FORMAT_PCM;
    internal_format.nBlockAlign		=	2 * input_format.nChannels;
    internal_format.wBitsPerSample	=	16;
	internal_format.cbSize			=	0;

	HRESULT hr = acmStreamOpen(&source_stream, (HACMDRIVER)NULL, 
							   (WAVEFORMATEX*) &input_format, (WAVEFORMATEX*) &internal_format,  
							    NULL, 0, 0, 0);

	if(hr != S_OK)
	{
		if(hr == ACMERR_NOTPOSSIBLE)
		{
			LogError("Unappropriate audio format");
		}
			
		char s[256];
		sprintf(s, "AcmDecoder: acmStreamOpen error %d", hr);
		LogError(s);
	}
}

bool Smokin::AcmDecoder::convert(unsigned char* in_data, unsigned long in_size,
							    unsigned char* out_data, unsigned long out_size,
								unsigned long* size_read, unsigned long* size_written)
{
	if(!in_data)
	{
		LogError("Invalid input buffer (null)");
		return false;
	}

    if(!out_data)
	{
		LogError("Invalid input buffer (null)");
		return false;
	}
	
    //printf("ACM_Decoder: received request to convert %d bytes to %d bytes\n", in_size, out_size);
   
	DWORD source_size;
    acmStreamSize(source_stream, out_size, &source_size, ACM_STREAMSIZEF_DESTINATION);
    if(source_size > in_size)
		source_size = in_size;

    ACMSTREAMHEADER acm_stream_header;
    memset(&acm_stream_header, 0, sizeof(acm_stream_header));

    acm_stream_header.cbStruct		= sizeof(acm_stream_header);
    acm_stream_header.fdwStatus		= 0;
    acm_stream_header.dwUser		= 0; 
    acm_stream_header.pbSrc			= in_data; 
    acm_stream_header.cbSrcLength	= source_size; 
    acm_stream_header.pbDst			= out_data; 
    acm_stream_header.cbDstLength	= out_size; 
  
	//printf("ACM_Decoder: src: %X %X %X\n", acm_stream_header.cbStruct, source_size, out_size); 

	HRESULT hr = acmStreamPrepareHeader(source_stream, &acm_stream_header, 0);
    if(hr != S_OK)
    {
		LogError("AcmDecoder: acmStreamPrepareHeader error ");
		return false;
    }
    
	hr = acmStreamConvert(source_stream, &acm_stream_header, 0);
	if(hr != S_OK)
	{
		LogError("AcmDecoder: acmStreamConvert error ");
		return false;
	}

	//printf("Uncompressed %d bytes from %d\n", ash.cbDstLengthUsed, ash.cbSrcLengthUsed);
	if(acm_stream_header.cbSrcLengthUsed > in_size)
		acm_stream_header.cbSrcLengthUsed = in_size;	
    if(size_read)
		*size_read = acm_stream_header.cbSrcLengthUsed;

    if(size_written)
		*size_written = acm_stream_header.cbDstLengthUsed;

    hr = acmStreamUnprepareHeader(source_stream, &acm_stream_header, 0);

	if(hr != S_OK)
	{
		LogError("AcmDecoder: acmStreamUnprepareHeader error ");
		return false;
	}
		
    return true;
}

Smokin::AcmDecoder::~AcmDecoder()
{
	acmStreamClose(source_stream, 0);
}

WAVEFORMATEX* Smokin::AcmDecoder::internalFormat()
{
	return &internal_format;
}

int Smokin::AcmDecoder::getMinSize()
{
    long in_size = internal_format.nBlockAlign;
    unsigned long src_size = 0;

    acmStreamSize(source_stream, in_size, &src_size, ACM_STREAMSIZEF_SOURCE);

    return 2 * src_size;
}

int Smokin::AcmDecoder::getSourceSize(int detination_size)
{
    unsigned long src_size = 0;

    acmStreamSize(source_stream, detination_size, &src_size, ACM_STREAMSIZEF_DESTINATION);

    return src_size;
}

int Smokin::AcmDecoder::getDestinationSize(int source_size)
{
    unsigned long destination_size = 0;

    acmStreamSize(source_stream, source_size, &destination_size, ACM_STREAMSIZEF_SOURCE);

    return destination_size;
}
