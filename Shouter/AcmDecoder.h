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


#pragma once

#include <windows.h>
#include <Vfw.h>

#pragma comment(lib, "Msacm32.lib")

namespace Smokin
{

	class AcmDecoder
	{
	public:
		AcmDecoder(WAVEFORMATEX* input_format);
		~AcmDecoder();

		/**
		* Minimal required output buffer size. Calls to Convert() will 
		* fail if you pass smaller output buffer to it.
		*/
		int getMinSize();

		/**
		* Estimates the amount ( in bytes ) of input data that's
		* required to produce specified amount of decompressed PCM data.
		*/
		int getSourceSize(int detination_size);

		int getDestinationSize(int source_size);

		/**
		* Decodes data. It is guaranteed that either size_read or size_written
		* will receive nonzero value if out_size>=GetMinSize(). 
		*/
		bool convert(unsigned char* in_data, unsigned long in_size,
					 unsigned char* out_data, unsigned long out_size,
					 unsigned long* size_read, unsigned long* size_written);

		WAVEFORMATEX* internalFormat();
	private:
		WAVEFORMATEX internal_format;
		HACMSTREAM source_stream;

		// The next two attributes should be together
		WAVEFORMATEX input_format;
		char input_format_extended_info[64];
	};

};
