#include "Recorder.hpp"
#include "../File.hpp"
#include "../Log.hpp"

namespace smk {

	Recorder::Recorder()
		:m_name("Recorder"), m_accepting(false), m_flush_size(1024 * 10), m_received_samples(0)
	{
		setSleepMs(500);
	}

	Recorder::~Recorder() { }

	void Recorder::startRecording(AudioFormat const& format, std::string const& filename) {
		stopRecording();
		m_format = format;
		m_received_samples = 0;
		m_filename = filename;
		start();
	}

	void Recorder::stopRecording() {
		if (isPumping())
			stop();
	}

	bool Recorder::isRecording() {
		return isPumping();
	}

	uint64_t Recorder::recordedMilliseconds() {
		return audioMilliseconds(m_received_samples, m_format);
	}

	bool Recorder::isAccepting() {
		return isPumping() && m_accepting;
	}

	void Recorder::push(float sample) {
		if (m_accepting) {
			std::unique_lock<std::mutex> lock(m_mutex);

			m_write.push_back(sample);
			m_received_samples += 1;

			if (m_write.size() > m_flush_size)
				signalWorkArrived();
		}
	}

	void Recorder::step() {
		writeToFile();
	}

	void Recorder::prePump() {
		makeDirectoryForFile(str8To32(m_filename));

		Log::i(m_name, "Starting recorder (" + m_filename + ")...");
		m_accepting = m_output.open(m_format, m_filename);

		if (!m_accepting) {
			Log::e(m_name, "Failed to start output to (" + m_filename + ")...");
		}
	}

	void Recorder::postPump() {
		m_accepting = false;

		writeToFile();
		m_output.close();

		Log::i(m_name, "Stopped recorder...");
	}

	void Recorder::writeToFile() {
		m_read.clear();

		{
			std::unique_lock<std::mutex> lock(m_mutex);
			std::swap(m_write, m_read);
		}

		if (!m_read.empty()) {
			Log::e(m_name, "About to save " +  to_str8(m_read.size())  + " samples");
			m_output.write(m_read.data(), (int)m_read.size());
		}
	}
}