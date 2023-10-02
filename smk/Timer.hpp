#pragma once

#include <cstdint>
#include <string>
#include "math/RunningStats.hpp"

namespace smk
{
    int64_t getCurrentMilliseconds();
    int64_t getCurrentMicroseconds();

    std::string datetimeMarker();



    //
    // Elapsed Timer
    //
    class ElapsedTimer {
    public:
        ElapsedTimer();

        void start();

        void invalidate();
        bool isValid() const;

        int64_t elapsed() const;  // milliseconds
        bool hasExpired(int64_t timeout) const;
    private:
        int64_t m_start;
    };


    //
    // Duration Probe
    //
    class DurationProbe {
    public:
        struct Info {
            bool collected = false;

            float start_stop_duration = 0.0f;
            float start_stop_min = 0.0f;
            float start_stop_avg = 0.0f;
            float start_stop_max = 0.0f;

            float start_start_avg = 0.0f;
            float start_start_fps = 0.0f;
        };

        DurationProbe(int samples = 25);

        void start();
        Info stop(int warningInterval = 5000);
        std::string stopAndPrint(int warningInterval = 5000);

    private:
        RunningAverage<float> m_time_mean; // measures the time between start and stop calls
        RunningAverage<float> m_starts_time_mean; // measures the time between start calls
        float m_time_max;
        float m_time_min;

        ElapsedTimer m_clock_timer;
        ElapsedTimer m_show_timer;
    };
}
