#pragma once

#include <list>
#include <cmath>

namespace smk
{
	template <typename T = float>
	class RunningAverage {
	public:
		explicit RunningAverage(int window, T initial_value = T(0.0))
			:m_window(window)
		{
			reset(initial_value);
		}

		void reset(T initial_value) {
			m_average = T(0.0);
			m_sum = T(0.0);
			m_samples.clear();
			add(initial_value);
		}

		T add(T value) {
			m_samples.push_back(value);
			m_sum += value;

			if (m_samples.size() > m_window) {
				m_sum -= m_samples.front();
				m_samples.pop_front();
			}

			m_average = m_sum / T(m_samples.size());
			return m_average;
		}

		T average() const { return m_average; }
	private:
		int m_window;
		std::list<T> m_samples;
		T m_sum;
		T m_average;
	};


	template <typename T = float>
	class RunningRms {
	public:
		explicit RunningRms(int window, T initial_value = T(0.0))
			:m_window(window)
		{
			reset(initial_value);
		}

		void reset(T initial_value) {
			m_rms = T(0.0);
			m_sum_of_squares = T(0.0);

			m_samples.clear();
			add(initial_value);
		}

		T add(T value) {
			m_samples.push_back(value);
			m_sum_of_squares += value * value;

			if (m_samples.size() > m_window) {
				T front = m_samples.front();
				m_samples.pop_front();
				m_sum_of_squares -= front * front;
			}

			m_rms = std::sqrt(m_sum_of_squares / T(m_samples.size()));
			return m_rms;
		}

		T rms() const { return m_rms; }
	private:
		int m_window;
		std::list<T> m_samples;
		T m_sum_of_squares;
		T m_rms;
	};

}
