#pragma once

#include "Audio.hpp"

#include <string>
#include <vector>
#include <memory>

namespace smk {

	//
	// wav writer
	//
	class Wav {
	public:
		Wav();
		~Wav();

		bool open(AudioFormat const& format, std::string const& filename);
		void close();
		bool isOk();
		int write(float* data, int frames);

		static bool load(std::string const& filename, std::vector<float>& data, AudioFormat& format);
	private:
		struct PrivateImplementation;
		std::shared_ptr<PrivateImplementation> m;

		AudioFormat m_format;
		std::string m_filename;
		bool m_is_ok;
	};

}