#pragma once

#include <cassert>
#include <condition_variable>

namespace smk {

    class Barrier {
    public:
        Barrier(std::size_t nb_threads)
            : m_mutex(), m_condition(), m_threads(nb_threads), m_canceled(false)
        {
            assert(0u != m_threads);
        }

        Barrier(const Barrier& barrier) = delete;
        Barrier(Barrier&& barrier) = delete;
        Barrier& operator=(const Barrier& barrier) = delete;
        Barrier& operator=(Barrier&& barrier) = delete;

        // don't delete this without making sure thread have left the building
        ~Barrier() noexcept { cancel(); }

        void wait() {
            std::unique_lock<std::mutex> lock(m_mutex);
            assert(0u != m_threads);

            if (m_canceled)
                return;

            if (0u == --m_threads) {
                m_condition.notify_all();
            } else {
                m_condition.wait(lock, [this]() { return ((0u == m_threads) || m_canceled); });
            }
        }

        void cancel() {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_canceled = true;
            m_condition.notify_all();
        }
    private:
        std::mutex m_mutex;
        std::condition_variable m_condition;
        std::size_t m_threads;
        bool m_canceled;
    };
}
