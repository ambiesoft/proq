#include <vector>
#include <iostream>
#include <functional>
#include <cassert>

#ifdef _WIN32
#include <ShlObj.h>
#endif

#include "../../lsMisc/stdosd/stdosd.h"

using namespace Ambiesoft::stdosd;
using namespace std;

int main2(int argc, SYSTEM_CHAR_TYPE* argv[]);

int main(int argc, char* argv[])
{
#ifdef _WIN32
    {
        int argc=0;
        std::unique_ptr<LPWSTR, function<decltype(::LocalFree)>>
            arg(::CommandLineToArgvW(::GetCommandLine(), &argc), ::LocalFree);

        return main2(argc,arg.get());
    }
#else
    return main2(argc,argv);
#endif
}

struct ProcessState
{
    bool suspended_ = false;
    STDOSD_PID pid_;
    ProcessState(STDOSD_PID pid):pid_(pid){}
    ~ProcessState()
    {
        if(suspended_)
            stdResumeProcess(pid_);
    }

    bool suspend() {
        assert(!suspended_);
        suspended_ = stdSuspendProcess(pid_);
        return suspended_;
    }
    bool resume() {
        assert(suspended_);
        suspended_ = !stdResumeProcess(pid_);
        return !suspended_;
    }
    STDOSD_PID pid() const {
        return pid_;
    }
};


int main3(vector<ProcessState*>& pros);

int main2(int argc, SYSTEM_CHAR_TYPE* argv[])
{
    if(argc < 2)
    {
        cerr << "No Input Executable" << endl;
        return 1;
    }

    vector<ProcessState*> pros;
    for(auto&& v : stdGetAllProcesses(argv[1])) {
        pros.push_back(new ProcessState(v));
    }

    bool ret = main3(pros);
    for(auto&& pro : pros)
        delete pro;
    return ret;
}

int main3(vector<ProcessState*>& pros)
{
    if(pros.empty())
    {
        cerr << "No processes" << endl;
        return 1;
    }

    for(size_t i=0 ; i < pros.size(); ++i)
    {
        if(!pros[i]->suspend()) {
            cerr << "Failed to suspend:" << pros[i]->pid() << endl;
            return 1;
        }
    }

    // All Process suspended now
    for(size_t i=0 ; i < pros.size(); ++i){
        if(!pros[i]->resume()) {
            cerr << "Failed to resume:" << pros[i]->pid() << endl;
        }
        if(!stdWaitProcess(pros[i]->pid())) {
            cerr << "Faiiled to wait:" << pros[i]->pid() << endl;
        }
    }
    return 0;
}
