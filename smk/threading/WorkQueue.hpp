#pragma once

#include <list>
#include <functional>
#include <cstdint>

#include "Worker.hpp"

namespace smk {

    class WorkQueue : public Worker {
    public:

        struct BaseWork {
            BaseWork() { }
            virtual ~BaseWork() { };
            virtual void execute(WorkQueue* queue) = 0;

            uint64_t type_id = 0;
            uint64_t enqueue_id = 0;
            int64_t enqueue_ts = 0;
            int64_t complete_ts = 0;
            bool move_to_results = true;
        private:
            // don't copy
            BaseWork(const BaseWork&) = delete;
            BaseWork& operator=(BaseWork const&) = delete;
        };
        using Job = std::shared_ptr<BaseWork>;
        using JobList = std::list<Job>;


        WorkQueue();
        ~WorkQueue() override;

        uint64_t enqueue(Job job);
        uint64_t enqueue(std::function<void()> function);


        WaitEvent &resultsEvent();
        JobList takeResults();
        bool hasResult(uint64_t const enqueue_id);

        enum RunningMode {
            Normal,         // FIFO worker
            OneJobPerType   // when enqueuing it clears all other enqueue jobs for the same type
        };
        void setRunningMode(RunningMode mode);

    protected:
        void step() override;

    private:
        uint64_t m_id_generator;
        std::mutex m_work_mutex;
        JobList m_enqueued;
        JobList m_results;
        WaitEvent m_complete_event;
        RunningMode m_running_mode;
    };


}


