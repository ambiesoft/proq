#include <vector>
#include <iostream>
#include <functional>
#include <cassert>
#include <sstream>

#ifdef _WIN32
#include <ShlObj.h>
#endif

#include "../../lsMisc/stdosd/stdosd.h"

#include "processstate.h"

using namespace Ambiesoft::stdosd;
using namespace std;

#define APP_NAME "proq"
#define APP_VERSION "1.0.0"

struct Options {
    bool bVerbose = false;
} gOptions;

int main2(int argc, SYSTEM_CHAR_TYPE* argv[]);
int main3(vector<ProcessState*>& pros);

string getCurrentDateTime()
{
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);

    stringstream ss;
    ss << std::put_time(now, "%x") << " " << std::put_time(now, "%X");
    return ss.str();

    char buffer[80];
    std::strftime(buffer, 80, "%Y/%m/%d %H:%M:%S", now);

    return buffer;
}

void WriteLog(const char* pMessage, STDOSD_PID pid)
{
    if(!gOptions.bVerbose)
        return;

    cout << getCurrentDateTime() << "\t" << pMessage << ":" << pid << endl;
}
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


void showHelp()
{
    cout << APP_NAME << " v" << APP_VERSION << endl;
}
int main2(int argc, SYSTEM_CHAR_TYPE* argv[])
{
    if(argc < 2)
    {
        cerr << "No Input Executable" << endl;
        return 1;
    }

    vector<SYSTEM_STRING_TYPE> exes;

    for(int i=1 ; i < argc; ++i) {
        SYSTEM_STRING_TYPE arg = argv[i];
        if(stdStartWith(arg, STDOSD_SYSTEM_CHAR_LITERAL("-"))) {
            if(
                arg==STDOSD_SYSTEM_CHAR_LITERAL("--help") ||
                arg==STDOSD_SYSTEM_CHAR_LITERAL("-h") ||
                arg==STDOSD_SYSTEM_CHAR_LITERAL("-v") ||
                arg==STDOSD_SYSTEM_CHAR_LITERAL("--version")) {
                showHelp();
                return 0;
            } else if(arg==STDOSD_SYSTEM_CHAR_LITERAL("--verbose")) {
                gOptions.bVerbose = true;
            } else {
                stdcerr << STDOSD_SYSTEM_CHAR_LITERAL("Unknown option:") << arg << endl;
                return 1;
            }
        } else {
            exes.push_back(arg);
        }
    }

    vector<ProcessState*> pros;
    for(auto&& exe : exes) {
        for(auto&& v : stdGetAllProcesses(exe)) {
            pros.push_back(new ProcessState(v));
        }
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
        WriteLog("suspended", pros[i]->pid());
    }

    // All Process suspended now
    for(size_t i=0 ; i < pros.size(); ++i){
        if(!pros[i]->resume()) {
            cerr << "Failed to resume:" << pros[i]->pid() << endl;
        }
        WriteLog("resumed", pros[i]->pid());

        WriteLog("waiting", pros[i]->pid());
        if(!stdWaitProcess(pros[i]->pid())) {
            cerr << "Faiiled to wait:" << pros[i]->pid() << endl;
            return 1;
        }
        WriteLog("finished", pros[i]->pid());
    }
    return 0;
}
