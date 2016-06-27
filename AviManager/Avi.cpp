#include "Avi.h"

#include <cassert>
#include <iostream>
#include <queue>
#include <list>
#include <cmath>

#define __STDC_CONSTANT_MACROS

extern "C" {	
#include <ffmpeg/avcodec.h>
#include <ffmpeg/avformat.h>
#include <ffmpeg/swscale.h>
}

//#if defined(_WIN32)
#ifdef _MSC_VER
#pragma comment(lib, "avutil-49.lib")
#pragma comment(lib, "avformat-51.lib")
#pragma comment(lib, "avcodec-51.lib")
#pragma comment(lib, "swscale-0.lib")
#endif

#include <OpenThreads/Mutex>
#include <OpenThreads/Thread>
#include <OpenThreads/ScopedLock>
#include <OpenThreads/Block>

#include <iostream>

#include "../MemoryPool/MemoryPool.h"

#define AviLogger(msg) std::cout << msg << std::endl
//#define AviLogger(msg)

#define DefaultPixelFormat PIX_FMT_RGB24

class MemoryPoolThreadSafe : public Smokin::MemoryPool
{
public:

	MemoryPoolThreadSafe(SizeType const pool_size = Smokin::memory_pool_default_pool_size, 
						 SizeType const block_size = Smokin::memory_pool_default_block_size)
						 :MemoryPool(pool_size, block_size)
	{}

	virtual void reblockPool(SizeType const block_size)
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mutex);
		MemoryPool::reblockPool(block_size);
	}
	
	virtual Smokin::MemoryPool::DataTypePointer requestRawMemoryBlock()
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mutex);
		return MemoryPool::requestRawMemoryBlock();
	}

	virtual void releaseRawMemoryBlock(DataTypePointer data)
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mutex);
		MemoryPool::releaseRawMemoryBlock(data);
	}

	virtual void releaseAllMemoryBlocks()
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mutex);
		MemoryPool::releaseAllMemoryBlocks();
	}
private:
	OpenThreads::Mutex mutex;
};

struct DataBlock
{
	DataBlock()
	{
		data = 0;
		size = 0;
		presentation_time = 0.0f;
		presentation_time_end = 0.0f;
		presented = false;
	}

	bool isUsed()
	{
		return (data != 0);
	}

	unsigned char* data;
	unsigned int size;
	float presentation_time;
	float presentation_time_end;
	bool presented;
};

class DataQueue
{
public:
	DataQueue()
	{}

	~DataQueue()
	{
		clear();
	}

	void push(DataBlock data_block)
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mutex);
		data_blocks.push_back(data_block);
	}

	DataBlock front()
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mutex);

		if (data_blocks.empty())
			return DataBlock();

		return data_blocks.front();
	}

	DataBlock pop()
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mutex);

		if (data_blocks.empty())
			return DataBlock();

		DataBlock data_block = data_blocks.front();
		data_blocks.pop_front();

		//full_block.release();

		return data_block;
	}

	bool empty()
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mutex);
		return data_blocks.empty();
	}

	void clear()
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mutex);
		data_blocks.clear();

		//full_block.release();
	}
	double queuedTime()
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mutex);

		if(data_blocks.empty())
			return 0.0;

		return (data_blocks.back().presentation_time - data_blocks.front().presentation_time);
	}

	unsigned int size()
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mutex);
		return (unsigned int) data_blocks.size();
	}

	//OpenThreads::Block &fullBlock()
	//{
	//	return full_block;
	//}
private:
	std::list<DataBlock> data_blocks;
	OpenThreads::Mutex mutex;
//	OpenThreads::Block full_block;
};

struct Smokin::Avi::AviData
{
	struct StreamData
	{
		StreamData()
			:index(0), codec_context(0), stream(0), stream_time_base(0)
		{}

		~StreamData()
		{}

		unsigned int index;
		AVCodecContext *codec_context;
		AVStream *stream;
		float stream_time_base;
	};

	AviData()
		:number_of_video_streams(0), number_of_audio_streams(0), format_context(0), 
		frame(0), conversion_frame(0), conversion_frame_buffer(0),conversion_frame_buffer_size(0), audio_buffer_size(0)
	{
		memset(audio_buffer, 0, audio_buffer_max_size);
		image_convertion_context = 0;
	}

	~AviData()
	{
		if (frame)
		{
			av_free(frame);
		}

		if (conversion_frame)
		{
			av_free(conversion_frame);
		}

		if (conversion_frame_buffer)
		{
			av_free(conversion_frame_buffer);
		}

		for (unsigned int i = 0; i != number_of_video_streams; ++i)
		{
			if (video_streams[i].codec_context)
			{
				avcodec_close(video_streams[i].codec_context);
			}
		}

		for (unsigned int i = 0; i != number_of_audio_streams; ++i)
		{
			if (audio_streams[i].codec_context)
			{
				avcodec_close(audio_streams[i].codec_context);
			}
		}

		if(format_context)
		{
			av_close_input_file(format_context);
		}
	}

	// false on empty
	bool nextVideoBlock()
	{
		if ( !video_queue.empty() )
		{
			if (current_video_block.data)
			{
				video_memory_pool.releaseRawMemoryBlock( current_video_block.data );
				current_video_block.data = 0;
				current_video_block.presentation_time = 0;
				current_video_block.presentation_time_end = 0;
			}

			current_video_block = video_queue.pop();
			return true;
		}

		return false;
	}

	bool nextAudioBlock()
	{
		if( !audio_queue.empty())
		{
			if (current_audio_block.data)
			{
				audio_memory_pool.releaseRawMemoryBlock( current_audio_block.data );
				current_audio_block.data = 0;
				current_audio_block.presentation_time = 0;
				current_audio_block.presentation_time_end = 0;
			}

			current_audio_block = audio_queue.pop();
			return true;
		}

		return false;
	}

	bool changeVideoBlock(double time, unsigned int recursion_level = 0)
	{	
		if (current_video_block.isUsed())
		{
			if ( (time >= current_video_block.presentation_time) &&
				 (time < current_video_block.presentation_time_end) )
			{
				if (current_video_block.presented)
				{
					return false;
				}
				else
				{
					current_video_block.presented = true;
					return true;
				}
			}

			if ( current_video_block.presentation_time > time)
			{
				return false;
			}
		}

		if (video_queue.empty())
		{
			AviLogger("Video queue is empty. " << time);
		}

		if ( nextVideoBlock() )
		{
		/*	if (recursion_level)
			{
				AviLogger("Video Frame Drop : " << recursion_level << " " << time
					<< " " << current_video_block.presentation_time 
					<< " " <<current_video_block.presentation_time_end
					<< " " <<current_video_block.presentation_time_end -  current_video_block.presentation_time);
			}*/

			DataBlock next = video_queue.front();
			if (next.isUsed())
			{
				current_video_block.presentation_time_end = next.presentation_time;
			}

			return changeVideoBlock(time, recursion_level + 1);
		}

		return false;
	}

	bool changeAudioBlock(double time)
	{
		if (current_audio_block.isUsed())
		{
			if ( (time >= current_audio_block.presentation_time) &&
				 (time < current_audio_block.presentation_time_end) )
			{
				if (current_audio_block.presented)
				{
					return false;
				}
				else
				{
					current_audio_block.presented = true;
					return true;
				}
			}

			if ( current_audio_block.presentation_time > time)
			{
				return false;
			}
		}

		// find next valid audio block
		if( nextAudioBlock() )
		{	
			//DataBlock next = audio_queue.front();
			//if (next.isUsed())
			//{
			//	current_audio_block.presentation_time_end = next.presentation_time;
			//}

			//current_audio_block.presented = true;
			//return true;

			// we drop audio block, but get synch.. 
			// but we get sound click.. because we drop blocks...
			return changeAudioBlock(time);
		}

		return false;
	}

	StreamData video_streams[maximum_number_of_streams];
	unsigned int number_of_video_streams;

	StreamData audio_streams[maximum_number_of_streams];
	unsigned int number_of_audio_streams;

	AVFormatContext* format_context;

	// video decoding
	AVFrame* frame;
	AVFrame* conversion_frame;
	uint8_t* conversion_frame_buffer;
	unsigned int conversion_frame_buffer_size;

	// audio decoding
	static unsigned int const audio_buffer_max_size = AVCODEC_MAX_AUDIO_FRAME_SIZE * sizeof(int16_t);
	unsigned char audio_buffer[audio_buffer_max_size];
	int audio_buffer_size;

	struct SwsContext *image_convertion_context;

	DataQueue audio_queue;
	DataQueue video_queue;

	DataBlock current_audio_block;
	DataBlock current_video_block;

	double last_video_update;
	double last_audio_update;

	MemoryPoolThreadSafe video_memory_pool;
	MemoryPoolThreadSafe audio_memory_pool;
};

class Smokin::Avi::DecodingThread : public OpenThreads::Thread
{
public:
	DecodingThread(Smokin::Avi &avi)
		:avi_(avi), running(true)
	{}

	~DecodingThread()
	{}

	void flush()
	{
		avi_.avi_data->audio_queue.clear();
		avi_.avi_data->video_queue.clear();
	}

	void setDone()
	{
		running = false;
	}

	virtual void run()
	{
		while(running)
		{
			if ( (avi_.avi_data->video_queue.queuedTime() < avi_.max_buffered_data_time) &&
				 (avi_.avi_data->audio_queue.queuedTime() < avi_.max_buffered_data_time) )
			{
				avi_.demultiplex();
			}
			else
			{
				microSleep( (unsigned int)((avi_.max_buffered_data_time / 8.0) * 1000));
			}
		}
	}

private:
	Smokin::Avi &avi_;
	bool running;
};

bool Smokin::Avi::subsystem_initialized = false;

Smokin::Avi::Avi()
{
	current_video_stream_ = 0;
	current_audio_stream_ = 0;

	total_time_ = 0;
	frame_rate_ = 0;
	audio_bits_ = 0;

	video_presentation_time = -1;
	video_presentation_duration = 0;
	packets_for_frame_counter = 0;

	decoding_worker = 0;

	max_buffered_data_time = 0.6f;

	audio_clock = 0.0;

	if (!subsystem_initialized)
	{
		av_register_all();
		subsystem_initialized = true;
	}

	avi_data = new Avi::AviData();

	playing_ = false;
}

Smokin::Avi::~Avi()
{
	stop();
	delete avi_data;
}

bool Smokin::Avi::isPlaying() const
{
	return playing_;
}

void Smokin::Avi::play()
{	
	stop();
	playing_ = true;

	float const time_threshold = 0.1f;

	avi_data->current_video_block.presented = true;
	avi_data->current_audio_block.presented = true;

	unsigned int block_size = avi_data->conversion_frame_buffer_size;
	float pool_size = ceil((max_buffered_data_time + time_threshold) * frameRate()) * block_size;
	avi_data->video_memory_pool.reallocPool((unsigned int)pool_size, block_size);

	block_size = 500;
	pool_size = (max_buffered_data_time + time_threshold) * (audioSampleRate() * audioChannels() * (audioBits() / 8)) ;
	//pool_size = ceil(pool_size / float(block_size) ) * block_size;
	avi_data->audio_memory_pool.reallocPool((unsigned int) pool_size, block_size);

	decoding_worker = new Smokin::Avi::DecodingThread(*this);
//	decoding_worker->setSchedulePriority(OpenThreads::Thread::THREAD_PRIORITY_MIN);

	decoding_worker->start();
}

void Smokin::Avi::stop()
{
	if (decoding_worker)
	{	
		
		decoding_worker->setDone();
		decoding_worker->flush();

		if (decoding_worker->isRunning())
		{
			decoding_worker->microSleep(500000);
			decoding_worker->cancel();
		}

		delete decoding_worker;
		decoding_worker = 0;
	}

	playing_ = false;
	
	video_presentation_time = -1;
	packets_for_frame_counter = 0;
	video_presentation_duration = 0;

	avi_data->last_video_update = 0.0;
	avi_data->last_audio_update = 0.0;

	audio_clock = 0.0;
}

void Smokin::Avi::setMaxBufferedDataTime(double time)
{
	max_buffered_data_time = (float)time;
}

double Smokin::Avi::maxBufferedDataTime() const
{
	return max_buffered_data_time;
}

Smokin::Avi::AviStatus Smokin::Avi::load(std::string const& filename)
{
	if(av_open_input_file(&avi_data->format_context, filename.c_str(), NULL, 0, NULL) != 0)
	{
		return AviErrorUnableToOpenFile;
	}

	if( av_find_stream_info(avi_data->format_context) < 0 )
	{
		return AviErrorUnableToFindStreamInformation;
	}

	// Dump information about file onto standard error
	// dump_format(avi_data->format_context, 0, filename.c_str(), 0);

	// find streams
	for(unsigned int i = 0; i < avi_data->format_context->nb_streams; ++i)
	{
		if (avi_data->format_context->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO) 
		{
			avi_data->video_streams[current_video_stream_].index = i;
			avi_data->video_streams[current_video_stream_].codec_context = avi_data->format_context->streams[i]->codec;
			avi_data->video_streams[current_video_stream_].stream = avi_data->format_context->streams[i];
			avi_data->video_streams[current_video_stream_].stream_time_base = float(avi_data->format_context->streams[i]->time_base.num) /
																			  float(avi_data->format_context->streams[i]->time_base.den);

			// find and open codec
			AVCodec *codec = avcodec_find_decoder(avi_data->video_streams[current_video_stream_].codec_context->codec_id);
			if(codec && 
			   (avcodec_open(avi_data->video_streams[current_video_stream_].codec_context, codec) >= 0) ) 
			{
				++avi_data->number_of_video_streams;
			}
		}
		else if (avi_data->format_context->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO)
		{
			avi_data->audio_streams[current_audio_stream_].index = i;
			avi_data->audio_streams[current_audio_stream_].codec_context = avi_data->format_context->streams[i]->codec;
			avi_data->audio_streams[current_audio_stream_].stream = avi_data->format_context->streams[i];
			avi_data->audio_streams[current_audio_stream_].stream_time_base = float(avi_data->format_context->streams[i]->time_base.num) /
																			  float(avi_data->format_context->streams[i]->time_base.den);
			
			// find and open codec
			AVCodec *codec = avcodec_find_decoder(avi_data->audio_streams[current_audio_stream_].codec_context->codec_id);
			if(codec && 
			   (avcodec_open(avi_data->audio_streams[current_audio_stream_].codec_context, codec) >= 0) ) 
			{

				++avi_data->number_of_audio_streams;
			}
		}
	}

	return AviOk;
}

Smokin::Avi::AviStatus Smokin::Avi::selectVideoStream(unsigned int video_stream_index)
{
	assert(video_stream_index < numberOfVideoStreams());

	current_video_stream_ = video_stream_index;

	// free
	if (avi_data->frame)
	{
		av_free(avi_data->frame);
		avi_data->frame = 0;
	}

	if (avi_data->conversion_frame)
	{
		av_free(avi_data->conversion_frame);
		avi_data->conversion_frame = 0;
	}

	if (avi_data->conversion_frame_buffer)
	{
		av_free(avi_data->conversion_frame_buffer);
		avi_data->conversion_frame_buffer = 0;
	}

	avi_data->video_queue.clear();
	avi_data->video_memory_pool.releaseAllMemoryBlocks();

	avi_data->current_video_block.data = 0;
	avi_data->current_video_block.presentation_time = 0;
	avi_data->current_video_block.presentation_time_end = 0;

	// init
	avi_data->frame = avcodec_alloc_frame();

	if (!avi_data->frame)
	{
		return AviErrorUnableToAllocateFrame;
	}

	// RGB conversion structure
	avi_data->conversion_frame = avcodec_alloc_frame();

	if (!avi_data->conversion_frame)
	{
		return AviErrorUnableToAllocateFrame;
	}

	// Assign appropriate parts of buffer to image planes in conversion_frame
	// Note that conversion_frame is an AVFrame, but AVFrame is a superset
	// of AVPicture
	avi_data->conversion_frame_buffer_size = avpicture_get_size(DefaultPixelFormat, width(), height()); 
	avi_data->conversion_frame_buffer = (uint8_t*) av_malloc(avi_data->conversion_frame_buffer_size);

	if (!avi_data->conversion_frame_buffer)
	{
		return AviErrorUnableToAllocateFrame;
	}

	avpicture_fill((AVPicture*) avi_data->conversion_frame, avi_data->conversion_frame_buffer, 
				   DefaultPixelFormat, width(), height()); 

	// setup conversion contex

	//int flags = SWS_GAUSS;
	//int flags = SWS_BICUBLIN;
	//int flags = SWS_BICUBIC;
	int flags = 0;

	avi_data->image_convertion_context = sws_getContext(
		avi_data->video_streams[current_video_stream_].codec_context->width,
		avi_data->video_streams[current_video_stream_].codec_context->height, 
		avi_data->video_streams[current_video_stream_].codec_context->pix_fmt, 
		avi_data->video_streams[current_video_stream_].codec_context->width,
		avi_data->video_streams[current_video_stream_].codec_context->height,
		DefaultPixelFormat,
		flags, 
		0, 0, 0);

	if(avi_data->image_convertion_context == 0) 
	{
		return AviErrorUnableToSetupConversionContext;
	}

	frame_rate_ = float(1.0 / avi_data->video_streams[current_video_stream_].stream_time_base);

	video_presentation_time = -1;
	packets_for_frame_counter = 0;
	video_presentation_duration = 0;

	return AviOk;
}

Smokin::Avi::AviStatus Smokin::Avi::selectAudioStream(unsigned int audio_stream_index)
{
	assert(audio_stream_index < numberOfAudioStreams());

	current_audio_stream_ = audio_stream_index;

	if (avi_data->audio_streams[current_audio_stream_].codec_context->sample_fmt == SAMPLE_FMT_U8)
	{
		audio_bits_ = 8;
	}
	else if (avi_data->audio_streams[current_audio_stream_].codec_context->sample_fmt == SAMPLE_FMT_S16)
	{
		audio_bits_ = 16;
	}
	else if (avi_data->audio_streams[current_audio_stream_].codec_context->sample_fmt == SAMPLE_FMT_S24)
	{
		audio_bits_ = 24;
	}
	else if (avi_data->audio_streams[current_audio_stream_].codec_context->sample_fmt == SAMPLE_FMT_S32)
	{
		audio_bits_ = 32;
	}
	else
	{
		return AviErrorUnsupportedAudioFormat;
	}

	avi_data->audio_queue.clear();
	avi_data->audio_memory_pool.releaseAllMemoryBlocks();

	avi_data->current_audio_block.data = 0;
	avi_data->current_audio_block.presentation_time = 0;
	avi_data->current_audio_block.presentation_time_end = 0;

	audio_clock = 0.0;

	return AviOk;
}

unsigned char* Smokin::Avi::videoData()
{
	//return (unsigned char*) avi_data->conversion_frame_buffer;
	return avi_data->current_video_block.data;
}

int Smokin::Avi::videoDataSize()
{
	//return avi_data->conversion_frame_buffer_size;
	return avi_data->current_video_block.size;
}

unsigned char* Smokin::Avi::audioData()
{
//	return (unsigned char*) avi_data->audio_buffer;
	return avi_data->current_audio_block.data;
}
int Smokin::Avi::audioDataSize()
{
//return avi_data->audio_buffer_size;
	return avi_data->current_audio_block.size;
}

unsigned int Smokin::Avi::numberOfAudioStreams() const
{
	return avi_data->number_of_audio_streams;
}
unsigned int Smokin::Avi::numberOfVideoStreams() const
{
	return avi_data->number_of_video_streams;
}

unsigned int Smokin::Avi::width() const
{
	assert(current_video_stream_ < numberOfVideoStreams());
	return avi_data->video_streams[current_video_stream_].codec_context->width;
}
unsigned int Smokin::Avi::height() const
{
	assert(current_video_stream_ < numberOfVideoStreams());
	return avi_data->video_streams[current_video_stream_].codec_context->height;
}
double Smokin::Avi::totalTime() const
{
	return total_time_;
}
float Smokin::Avi::frameRate() const
{
	return frame_rate_;
}

unsigned int Smokin::Avi::audioSampleRate() const
{
	assert(current_audio_stream_ < numberOfAudioStreams());
	return avi_data->audio_streams[current_audio_stream_].codec_context->sample_rate;
}

unsigned int Smokin::Avi::audioChannels() const
{
	assert(current_audio_stream_ < numberOfAudioStreams());
	return avi_data->audio_streams[current_audio_stream_].codec_context->channels;
}

unsigned int Smokin::Avi::audioBits() const
{
	assert(current_audio_stream_ < numberOfAudioStreams());
	return audio_bits_;
}

Smokin::Avi::AviStatus Smokin::Avi::nextAudioData()
{
	Smokin::Avi::AviStatus status = AviNoDecodedDataAvailable;

	if (avi_data->nextAudioBlock())
	{
		status = AviNewAudioDataToPlay;
	}

	return status;
}
Smokin::Avi::AviStatus Smokin::Avi::nextVideoData()
{
	Smokin::Avi::AviStatus status = AviNoDecodedDataAvailable;

	if (avi_data->nextVideoBlock())
	{
		status = AviNewVideoDataToPlay;
	}

	return status;
}

Smokin::Avi::AviStatus Smokin::Avi::updateVideo(double const time)
{	
	Smokin::Avi::AviStatus status = AviOk;
	
	if (avi_data->changeVideoBlock(time))
	{	
		status = AviNewVideoDataToPlay;
	}

	return status;
}

Smokin::Avi::AviStatus Smokin::Avi::updateAudio(double const time)
{
	Smokin::Avi::AviStatus status = AviOk;
	
	if (avi_data->changeAudioBlock(time))
	{
		status = AviNewAudioDataToPlay;
	}

	return status;
}

Smokin::Avi::AviStatus Smokin::Avi::demultiplex()
{
	Smokin::Avi::AviStatus status = AviOk;
	AVPacket packet;

	if (av_read_frame(avi_data->format_context, &packet) >= 0)
	{
		// Is this a packet from the video stream?
		if( current_video_stream_ < numberOfVideoStreams() &&
			(packet.stream_index == avi_data->video_streams[current_video_stream_].index) )
		{
			status = decodeVideo(&packet);
		}
		else if ( current_audio_stream_ < numberOfAudioStreams() &&
			(packet.stream_index == avi_data->audio_streams[current_audio_stream_].index) )
		{
			status = decodeAudio(&packet);
		}
	}
	else
	{
		return AviEndOfPacketData;
	}

	av_free_packet(&packet);

	return status;
}

Smokin::Avi::AviStatus Smokin::Avi::decodeAudio(AVPacket* packet)
{
	Smokin::Avi::AviStatus status = AviAudioFrameDecoded;

	uint8_t *audio_packet_data = packet->data;
	int audio_packet_size = packet->size;

	int16_t* write_buffer = (int16_t*) &avi_data->audio_buffer[avi_data->audio_buffer_size];

	// sometimes the same packet must decoded several times to get audio data.
	while(audio_packet_size > 0) 
	{
		int data_size = avi_data->audio_buffer_max_size - avi_data->audio_buffer_size;
		assert(data_size <= avi_data->audio_buffer_max_size);

		int lenght = avcodec_decode_audio2(avi_data->audio_streams[current_audio_stream_].codec_context,
			write_buffer,
			&data_size, 
			audio_packet_data,
			audio_packet_size);

		if(lenght < 0) 
		{
			avi_data->audio_buffer_size = 0;
			status = AviErrorUnableToDecodeAudioFrame;
			break;
		}

		audio_packet_data += lenght;
		audio_packet_size -= lenght;

		// finished decoding
		if (data_size > 0)
		{
			//avi_data->audio_buffer_size += data_size;

			DataBlock audio_block;
			audio_block.size = data_size;

			// do we need audio pool reblock?
			if (data_size > avi_data->audio_memory_pool.blockSize())
			{
				avi_data->audio_memory_pool.reblockPool( data_size );
			}

			audio_block.presentation_time = audio_clock;

			int n = (audio_bits_ / 8) * avi_data->audio_streams[current_audio_stream_].codec_context->channels;
			audio_block.presentation_time_end = audio_block.presentation_time + 
												((double) data_size / 
											     (double) (n * avi_data->audio_streams[current_audio_stream_].codec_context->sample_rate)); 

			audio_clock = audio_block.presentation_time_end;

			try
			{
				audio_block.data = (uint8_t*) avi_data->audio_memory_pool.requestRawMemoryBlock();
				memcpy(audio_block.data, write_buffer, data_size);

				avi_data->audio_queue.push(audio_block);
			} 
			catch (Smokin::MemoryFullException memory_full)
			{
				AviLogger("Audio Memory Pool Full : " << memory_full);
			}
		}
	}
	return status;
}

Smokin::Avi::AviStatus Smokin::Avi::decodeVideo(AVPacket* packet)
{
	Smokin::Avi::AviStatus status = AviVideoFrameDecoded;

	int frame_finished;
	int bytes_decoded = avcodec_decode_video(avi_data->video_streams[current_video_stream_].codec_context,
											 avi_data->frame, &frame_finished,
											 packet->data, packet->size);

	if (bytes_decoded < 0)
	{
		return AviErrorUnableToDecodeVideoFrame;
	}

	if (bytes_decoded != packet->size)
	{
		AviLogger("Video bytes_decoded != packet->size");
	}

	// save the pts of the first packet of the current frame
	if (packets_for_frame_counter == 0)
	{
		video_presentation_time = packet->pts;
		video_presentation_duration = packet->duration;
	}

	if (frame_finished)
	{
		if (packet->dts != AV_NOPTS_VALUE)
		{
			video_presentation_time = packet->dts;
			video_presentation_duration = packet->duration;
		}

		packets_for_frame_counter = 0;

		DataBlock data_block;
		data_block.size = avi_data->conversion_frame_buffer_size;
		data_block.presentation_time = video_presentation_time * avi_data->video_streams[current_video_stream_].stream_time_base;
		data_block.presentation_time_end = data_block.presentation_time +
											video_presentation_duration * avi_data->video_streams[current_video_stream_].stream_time_base;

		try
		{
			data_block.data = (uint8_t*) avi_data->video_memory_pool.requestRawMemoryBlock();

			avpicture_fill((AVPicture*) avi_data->conversion_frame,(uint8_t*) data_block.data, 
				DefaultPixelFormat, width(), height()); 

			sws_scale(avi_data->image_convertion_context, 
				avi_data->frame->data, 
				avi_data->frame->linesize, 
				0, 
				avi_data->video_streams[current_video_stream_].codec_context->height, 
				avi_data->conversion_frame->data,
				avi_data->conversion_frame->linesize);

			avi_data->video_queue.push(data_block);
		} 
		catch (Smokin::MemoryFullException memory_full)
		{
			AviLogger("Video Memory Pool Full : " << memory_full);
		}

	}
	else
	{
		packets_for_frame_counter++;
		status = AviVideoDecoderNeedsMorePackets;
	}

	return status;
}
