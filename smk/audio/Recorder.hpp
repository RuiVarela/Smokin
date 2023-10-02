#pragma once

#include "Wav.hpp"
#include "../threading/Worker.hpp"

#include <vector>

namespace smk {

	class Recorder : private Worker {
	public:
		Recorder();
		~Recorder() override;

		void startRecording(AudioFormat const& format, std::string const& filename);
		void stopRecording();
		bool isRecording();

		uint64_t recordedMilliseconds();

		bool isAccepting();
		void push(float sample);

	protected:
		void step() override;
		void prePump() override;
		void postPump() override;

	private:
		std::string m_name;
		std::mutex m_mutex;
		AudioFormat m_format;
		std::vector<float> m_write;
		std::vector<float> m_read;
		size_t m_flush_size;
		uint64_t m_received_samples;

		Wav m_output;
		std::string m_filename;
		bool m_accepting;

		void writeToFile();
	};

}