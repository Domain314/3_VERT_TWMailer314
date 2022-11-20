
#ifndef TWMAILER314_BASIC_MTHREAD_HPP
#define TWMAILER314_BASIC_MTHREAD_HPP

#include <pthread.h>


class MThread {
public:
    MThread() {};
    virtual ~MThread() {};

    /** Returns true if the thread was successfully started, false if there was an error starting the thread */
    bool StartInternalThread() {
        return (pthread_create(&_thread, NULL, reinterpret_cast<void *(*)(void *)>(InternalThreadEntryFunc), this) == 0);

    }
    /** Will not return until the internal thread has exited. */
    void WaitForInternalThreadToExit() const {
        (void) pthread_join(_thread, NULL);
    }

protected:
    /** Implement this method in your subclass with the code you want your thread to run. */
    virtual void InternalThreadEntry() = 0;

private:
    static void * InternalThreadEntryFunc(void * This) {
        ((MThread *)This)->InternalThreadEntry(); return NULL;
    }

    pthread_t _thread;
};


#endif //TWMAILER314_BASIC_MTHREAD_HPP
