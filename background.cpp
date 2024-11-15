#include "headers.h"

void executeBackgroundProcess(vector<string> args, bool runInBackground, int &foregroundJobPid, bool &foregroundJobRunning)
{
    int pid = fork();

    if (pid == 0)
    {
        char **argv = new char *[args.size() + 1];

        for (int i = 0; i < args.size(); ++i)
        {
            argv[i] = new char[args[i].size() + 1];
            strcpy(argv[i], args[i].c_str());
        }

        argv[args.size()] = nullptr;
        execvp(args[0].c_str(), argv);
        cerr << "Command not found: " << args[0] << endl;
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        // Parent process
        foregroundJobPid = pid;

        if (!runInBackground)
        {
            foregroundJobRunning = true;
            waitpid(pid, nullptr, 0);
        }
        else
        {
            cout << pid << endl;
        }
    }
    else
    {
        cerr << "Fork failed" << endl;
    }
}