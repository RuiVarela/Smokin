#ifndef __AVI_MANAGER_H__
#define __AVI_MANAGER_H__

#include <string>

struct AVPacket;

namespace Smokin
{
	class Avi
	{
	private:

		struct AviData;
		class DecodingThread;
		friend class DecodingThread;

	public:
		static int const maximum_number_of_streams = 5;

		enum AviStatus
		{ 
			AviOk,
			AviErrorUnableToOpenFile,
			AviErrorUnableToFindStreamInformation,
			AviErrorUnableToAllocateFrame,
			AviErrorUnableToDecodeAudioFrame,
			AviErrorUnableToDecodeVideoFrame,
			AviErrorUnsupportedAudioFormat,
			AviErrorUnableToSetupConversionContext,
			AviNoDecodedDataAvailable, // queue is empty!
			AviVideoDecoderNeedsMorePackets,
			AviEndOfPacketData,
			AviAudioFrameDecoded,
			AviVideoFrameDecoded,
			AviStreamsSynched,
			AviStreamsReSynched,
			AviNewAudioDataToPlay,
			AviNewVideoDataToPlay, 
			AviNewVideoAndAudioDataToPlay

		};

		Avi();
		~Avi();

		AviStatus load(std::string const& filename);

		unsigned int numberOfAudioStreams() const;
		unsigned int numberOfVideoStreams() const;

		AviStatus selectVideoStream(unsigned int video_stream_index);
		AviStatus selectAudioStream(unsigned int audio_stream_index);

		unsigned int width() const;
		unsigned int height() const;
		double totalTime() const;
		float frameRate() const;

		unsigned int audioSampleRate() const;
		unsigned int audioChannels() const;

		// bits per sample
		unsigned int audioBits() const;

		//
		// Change the audio and video data to be displayed at the specified time
		// for continuous/realtime playing
		//
		AviStatus updateVideo(double const time);
		AviStatus updateAudio(double const time);


		AviStatus nextAudioData();
		AviStatus nextVideoData();


		//
		//buffering for decoded data
		//
		//the decoded data queue(video or audio) , with time = 2.5 will have 
		// at maximum 2.5 seconds of data (video or audio) already decode.
		//
		// do not change this value while playing. stop the movie first.
		void setMaxBufferedDataTime(double time);
		double maxBufferedDataTime() const;

		// retrieve current data
		unsigned char* videoData();
		int videoDataSize();

		unsigned char* audioData();
		int audioDataSize();

		bool isPlaying() const;
		void play();
		void stop();
	private:
		AviStatus demultiplex();
		AviStatus decodeAudio(AVPacket *avi_packet);
		AviStatus decodeVideo(AVPacket *avi_packet);
		
		static bool subsystem_initialized;

		unsigned int current_video_stream_;
		unsigned int current_audio_stream_;

		unsigned int audio_bits_;
		double total_time_;
		float frame_rate_;
		
		AviData* avi_data;
	
		unsigned int packets_for_frame_counter;
		unsigned int video_presentation_time;
		unsigned int video_presentation_duration;

		DecodingThread* decoding_worker;

		bool playing_;

		float max_buffered_data_time;

		double audio_clock;
		void synchTime();
	};
};

#endif //__AVI_MANAGER_H__

