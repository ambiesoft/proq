
#ifndef PROCESSSTATE_H
#define PROCESSSTATE_H

#include "../../lsMisc/stdosd/stdosd.h"

struct ProcessState
{
    bool suspended_ = false;
    STDOSD_PID pid_;
    ProcessState(STDOSD_PID pid):pid_(pid){}
    ~ProcessState()
    {
        if(suspended_)
            Ambiesoft::stdosd::stdResumeProcess(pid_);
    }

    bool suspend() {
        assert(!suspended_);
        suspended_ = Ambiesoft::stdosd::stdSuspendProcess(pid_);
        return suspended_;
    }
    bool resume() {
        assert(suspended_);
        suspended_ = !Ambiesoft::stdosd::stdResumeProcess(pid_);
        return !suspended_;
    }
    STDOSD_PID pid() const {
        return pid_;
    }
};




#endif // PROCESSSTATE_H
