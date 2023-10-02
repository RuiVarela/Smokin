#include "Worker.hpp"

static void setThreadName(const char* threadName);

namespace smk {

    Worker::Worker() {
        m_needs_to_set_name = false;
        m_pumping = false;

        setName("Worker_noname");
        setSleepMs(1);
    }

    Worker::~Worker() {
        stop();
    }

    void Worker::setName(std::string const& name) {
        m_name = name;
        m_needs_to_set_name = true;
    }

    void Worker::setSleepMs(int millis) {
        m_sleep_milliseconds = millis;
    }

    void Worker::start() {
        std::unique_lock<std::recursive_mutex> lock(m_runner_mutex);

        stop();

        m_pumping = true;
        m_runner = std::thread(&Worker::pump, this);
    }

    void Worker::stop() {
        std::unique_lock<std::recursive_mutex> lock(m_runner_mutex);

        signalPumpEnd();

        if (m_runner.joinable()) {
            signalWorkArrived();
            m_runner.join();
        }
    }

    bool Worker::isPumping() { return m_pumping; }

    void Worker::signalPumpEnd() {
        m_pumping = false;
    }

    void Worker::signalWorkArrived() {
        m_runner_event.set();
    }

    void Worker::threadSleep(const int ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

    void Worker::threadSpin() {
        threadSleep(0);
    }

    void Worker::pump() {
        checkSetNativeThreadName();

        prePump();

        checkSetNativeThreadName();

        while (isPumping()) {
            step();
            m_runner_event.waitAndReset(m_sleep_milliseconds);
        }

        postPump();
    }

    void Worker::step() { }
    void Worker::prePump() { }
    void Worker::postPump() { }

    void Worker::checkSetNativeThreadName()
    {
        if (m_needs_to_set_name) {
            setThreadName(m_name.c_str());
            m_needs_to_set_name = false;
        }
    }
}



#ifdef _WIN32
    #include <Windows.h>

    //
    // set thread name
    //
    const DWORD MS_VC_EXCEPTION=0x406D1388;

    #pragma pack(push,8)
    typedef struct tagTHREADNAME_INFO
    {
       DWORD dwType; // Must be 0x1000.
       LPCSTR szName; // Pointer to name (in user addr space).
       DWORD dwThreadID; // Thread ID (-1=caller thread).
       DWORD dwFlags; // Reserved for future use, must be zero.
    } THREADNAME_INFO;
    #pragma pack(pop)

    static void setThreadName(const char *threadName) {
        // DWORD dwThreadID = ::GetThreadId( static_cast<HANDLE>( t.native_handle() ) );

            THREADNAME_INFO info;
            info.dwType = 0x1000;
            info.szName = threadName;
            info.dwThreadID = GetCurrentThreadId();
            info.dwFlags = 0;

            __try
            {
            RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
            }
            __except(EXCEPTION_EXECUTE_HANDLER)
            {
            }
    }

 #elif defined(__linux__)

    #include <sys/prctl.h>

    static void setThreadName(const char *threadName) {
        prctl(PR_SET_NAME, threadName, 0,0,0);
    }

#elif defined(__APPLE__)

    #include <pthread.h>

    static void setThreadName(const char *threadName) {
        pthread_setname_np(threadName);
    }

#endif
