#include "headers.h"

bool isValidInteger(string &str)
{
    for (char c : str)
    {
        if (!isdigit(c))
        {
            return false;
        }
    }
    return str.empty() ? false : true;
}

int getPid(vector<string> args)
{
    int pid = 0;
    if (args.size() < 2)
    {
        pid = getpid();
    }
    else if (isValidInteger(args[1]))
    {
        int result = kill(stoi(args[1]), 0);
        if (result != 0)
        {
            cerr << "invalid process id!";
        }
        else
        {
            pid = stoi(args[1]);
        }
    }
    return pid;
}
void printProcessInfo(vector<string> args)
{
    int pid = getPid(args);

    cout << "pid -- " << pid << endl;

    string processStatusFilePath = "/proc/" + to_string(pid) + "/status";
    ifstream processStatusFile(processStatusFilePath.c_str());
    string line;
    string processStatus;
    while (getline(processStatusFile, line))
    {
        if (line.compare(0, 6, "State:") == 0)
        {
            processStatus = line.substr(7);
            cout << "Process Status -- " << processStatus << endl;
            break;
        }
    }

    string processMemoryInfoPath = "/proc/" + to_string(pid) + "/statm";
    ifstream statmFile(processMemoryInfoPath.c_str());
    unsigned long virtualMemory;
    if (statmFile >> virtualMemory)
    {
        cout << "memory -- " << virtualMemory * sysconf(_SC_PAGESIZE) << " {Virtual Memory}" << endl;
    }
    else
    {
        cout << "memory -- N/A" << endl;
    }

    string executableFilePath = "/proc/" + to_string(pid) + "/exe";

    char buffer[1024];
    ssize_t len = readlink(executableFilePath.c_str(), buffer, sizeof(buffer) - 1);
    if (len != -1)
    {
        buffer[len] = '\0';
        cout << "Executable Path -- " << buffer << endl;
    }
    else
    {
        cout << "Executable Path -- N/A" << endl;
    }
}