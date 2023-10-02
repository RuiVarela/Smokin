#pragma once

#include <cassert>
#include <condition_variable>

namespace smk {

    class WaitEvent {
    public:
        WaitEvent(bool set = false)
            :m_mutex(), m_condition(), m_set_state(set), m_canceled(false)
        { }

        WaitEvent(const WaitEvent& barrier) = delete;
        WaitEvent(WaitEvent&& barrier) = delete;
        WaitEvent& operator=(const WaitEvent& barrier) = delete;
        WaitEvent& operator=(WaitEvent&& barrier) = delete;

        // don't delete this without making sure thread have left the building
        ~WaitEvent() noexcept {  cancel(); }

        void set() {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (m_canceled)
                return;

            m_set_state = true;
            m_condition.notify_all();
        }

        bool waitAndReset(int max_period_ms = 1000) {
            std::unique_lock<std::mutex> lock(m_mutex);

            if (!m_set_state && !m_canceled) {
                auto wait = std::chrono::milliseconds(max_period_ms);
                m_condition.wait_for(lock, wait, [this]() { return m_set_state || m_canceled; });
            }

            bool state = m_set_state;
            m_set_state = false;
            return state;
        }

        void cancel() {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_canceled = true;
            m_condition.notify_all();
        }
    private:
        std::mutex m_mutex;
        std::condition_variable m_condition;
        bool m_set_state;
        bool m_canceled;
    };

}
