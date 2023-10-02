#include "Timer.hpp"

#include <chrono>
#include <mutex>
#include <ctime>
#include <limits>

namespace smk {
    static std::once_flag g_initialization_flag;

    static std::chrono::high_resolution_clock::time_point g_clock_start;

    static void initialize() {
        g_clock_start = std::chrono::high_resolution_clock::now();
    }

    static void checkInitialization() {
        std::call_once(g_initialization_flag, initialize);
    }

    int64_t getCurrentMilliseconds()  {
        checkInitialization();
        
        return getCurrentMicroseconds() / 1000;
    }

    int64_t getCurrentMicroseconds() {
        checkInitialization();

        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - g_clock_start).count();
    }

    std::string datetimeMarker()
    {
        time_t rawtime;
        struct tm* timeinfo;
        constexpr size_t buffer_size = 80;
        char buffer[buffer_size];

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer, buffer_size, "%d-%m-%Y_%H.%M.%S", timeinfo);
        return std::string(buffer);
    }

    //
    // Elapsed Timer
    //
    ElapsedTimer::ElapsedTimer() {
        invalidate();
    }

    void ElapsedTimer::start() {
        m_start = getCurrentMilliseconds();
    }

    void ElapsedTimer::invalidate() {
        m_start = -1;
    }

    bool ElapsedTimer::isValid() const {
        return m_start >= 0;
    }

    int64_t ElapsedTimer::elapsed() const {
        return getCurrentMilliseconds() - m_start;
    }

    bool ElapsedTimer::hasExpired(int64_t timeout) const {
        // if timeout is -1, uint64_t(timeout) is LLINT_MAX, so this will be considered as never expired
        return uint64_t(elapsed()) > uint64_t(timeout);
    }


    //
    // Duration Probe
    //
    DurationProbe::DurationProbe(int samples)
        :m_time_mean(samples),
        m_starts_time_mean(samples)
    {
        m_clock_timer.start();
        m_show_timer.start();
        m_time_max = std::numeric_limits<float>::min();
        m_time_min = std::numeric_limits<float>::max();
    }

    void DurationProbe::start() {
        m_starts_time_mean.add(float(m_clock_timer.elapsed()));
        m_clock_timer.start();
    }

    DurationProbe::Info DurationProbe::stop(int warningInterval) {
        DurationProbe::Info info;

        float milliseconds = float(m_clock_timer.elapsed());
        float mean = m_time_mean.add(milliseconds);

        if (milliseconds > m_time_max)
            m_time_max = milliseconds;

        if (milliseconds < m_time_min)
            m_time_min = milliseconds;

        if (m_show_timer.hasExpired(warningInterval)) {
            float starts_mean = m_starts_time_mean.average();
            float fps = 0.0f;
            if (starts_mean > 0.0f)
                fps = 1000.0f / starts_mean;

            info.collected = true;
            info.start_stop_duration = milliseconds;
            info.start_stop_min = m_time_min;
            info.start_stop_avg = mean;
            info.start_stop_max = m_time_max;
            info.start_start_avg = starts_mean;
            info.start_start_fps = fps;

            m_time_max = std::numeric_limits<float>::min();
            m_time_min = std::numeric_limits<float>::max();
            m_show_timer.start();
        }

        return info;
    }

    std::string DurationProbe::stopAndPrint(int warningInterval) {
        DurationProbe::Info info = stop(warningInterval);

        if (info.collected) {
            char buffer[256];
            size_t buffer_size = sizeof(buffer);

            snprintf(buffer, buffer_size,
                "duration: %.3f min: %.3f avg: %.3f max: %.3f <> Start-Start avg: %.3f fps: %.3f",
                info.start_stop_duration, info.start_stop_min, info.start_stop_avg, info.start_stop_max,
                info.start_start_avg, info.start_start_fps
            );
            return buffer;
        }

        return "";
    }
}
