#include "WorkQueue.hpp"
#include "../Timer.hpp"

namespace smk {

    //
    // WorkQueue
    //

    struct FunctionWork : public WorkQueue::BaseWork {
        std::function<void ()> function;

        void execute(WorkQueue* queue) override { function(); }
    };

    WorkQueue::WorkQueue() {
        setName("WorkQueue_noname");

        setSleepMs(100);

        m_running_mode = RunningMode::Normal;
        m_id_generator = 0;
    }

    WorkQueue::~WorkQueue(){ }

    uint64_t WorkQueue::enqueue(Job job) {
        uint64_t job_id = 0;

        if (isPumping() && job) {
            std::unique_lock<std::mutex> lock(m_work_mutex);

            if (m_running_mode == RunningMode::OneJobPerType) {
                uint64_t type_id = job->type_id;
                m_enqueued.remove_if([type_id](Job const& c) { return c->type_id == type_id; });
            }

            job->enqueue_id = ++m_id_generator;
            job->enqueue_ts = getCurrentMilliseconds();
            m_enqueued.push_back(job);

            job_id = job->enqueue_id;
        }

        signalWorkArrived();
        return job_id;
    }

    uint64_t WorkQueue::enqueue(std::function<void ()> function) {
        auto job = std::make_shared<FunctionWork>();
        job->function = function;
        job->move_to_results = false;

        return enqueue(job);
    }

    WaitEvent &WorkQueue::resultsEvent() { return m_complete_event; }

    WorkQueue::JobList WorkQueue::takeResults() {
        std::unique_lock<std::mutex> lock(m_work_mutex);

        JobList output;
        if (!m_results.empty())
            std::swap(m_results, output);

        return output;
    }

    bool WorkQueue::hasResult(const uint64_t enqueue_id) {
        std::unique_lock<std::mutex> lock(m_work_mutex);

        for (auto const& result : m_results)
            if (result->enqueue_id == enqueue_id)
                return true;

        return false;
    }

    void WorkQueue::setRunningMode(RunningMode mode) { m_running_mode = mode; }

    void WorkQueue::step()
    {
        Job job;

        // dequeue
        {
            std::unique_lock<std::mutex> lock(m_work_mutex);
            if (!m_enqueued.empty()) {
                job = m_enqueued.front();
                m_enqueued.pop_front();
            }
        }

        if (job && isPumping())
        {
            job->execute(this);

            // send to results
            if (job->move_to_results)
            {
                {
                    std::unique_lock<std::mutex> lock(m_work_mutex);
                    job->complete_ts = getCurrentMilliseconds();
                    m_results.push_back(job);
                }

                // signal the guys waiting for results
                m_complete_event.set();
            }


            // if we have more work to do, don't sleep
            signalWorkArrived();
        }
    }
}
