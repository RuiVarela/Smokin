#pragma once

#include <string>
#include <thread>
#include <mutex>
#include <list>
#include <functional>

#include "WaitEvent.hpp"

namespace smk {

    class Worker {
    public:
        Worker();
        virtual ~Worker();

        void setName(std::string const& name);
        void setSleepMs(int millis);

        void start();
        void stop();
        bool isPumping();

        void signalPumpEnd();
        void signalWorkArrived();

        static void threadSleep(int const ms);
        static void threadSpin();

    protected:
        std::string m_name;

        // override these methods to add work on the thread
        virtual void step();
        virtual void prePump();
        virtual void postPump();

    private:
        void checkSetNativeThreadName();
        std::recursive_mutex m_runner_mutex;
        std::thread m_runner;
        WaitEvent m_runner_event;
        int m_sleep_milliseconds;
        bool m_pumping;
        bool m_needs_to_set_name = false;

        virtual void pump();
    };
}


