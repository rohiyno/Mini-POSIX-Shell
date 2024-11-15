#include "headers.h"

bool foregroundJobRunning = false;
int foregroundJobPid = 0;

void handleCtrlZ(int signum)
{
    if (foregroundJobRunning)
    {
        kill(foregroundJobPid, SIGSTOP);
        cout << "Foreground process stopped and moved to the background." << std::endl;
        foregroundJobRunning = false;
    }
}

void handleCtrlC(int signum)
{
    if (foregroundJobRunning)
    {
        kill(foregroundJobPid, SIGINT);
        cout << "Foreground process interrupted with Ctrl-C." << std::endl;
        foregroundJobRunning = false;
    }
}

void executeCommand(const string &input)
{
    vector<string> args = tokenize(input);
    if (args.empty())
        return;

    string command = args[0];
    transform(command.begin(), command.end(), command.begin(), ::tolower);

    if (args.back() == "&")
    {
        args.pop_back();
        bool runInBackground = true;
        executeBackgroundProcess(args, runInBackground, foregroundJobPid, foregroundJobRunning);
    }
    else if (command == "exit")
    {
        addToCommandHistory(input);
        exit(1);
    }
    else if (command == "cd")
    {
        changeDirectory(vector<string>(args.begin() + 1, args.end()));
    }
    else if (command == "pwd")
    {
        pwd();
    }
    else if (command == "echo")
    {
        echo(args);
    }
    else if (command == "ls")
    {
        listDirectory(args);
    }
    else if (command == "pinfo")
    {
        printProcessInfo(args);
    }
    else if (command == "search")
    {
        search(args);
    }
    else if (command == "history")
    {
        addToCommandHistory(input);
        int numberOfRecords = (args.size() > 1) ? stoi(args[1]) : 20;
        if (numberOfRecords >= 0 && numberOfRecords <= 20)
        {
            displayCommandHistory(numberOfRecords);
        }
        else
        {
            cerr << "Provide a number between 0 to 20!" << endl;
        }
    }
    else
    {
        bool runInBackground = false;
        executeBackgroundProcess(args, runInBackground, foregroundJobPid, foregroundJobRunning);
    }
}

void executeMultipleCommands(const string &input)
{
    vector<string> tokenizeCommands = tokenizeForColon(input);
    for (const string &singleCommandInput : tokenizeCommands)
    {
        if (singleCommandInput == ";")
            continue;

        if (singleCommandInput.find('|') != string::npos)
        {
            vector<string> args = tokenizeForPipe(singleCommandInput);
            executePipes(args);
        }
        else if (singleCommandInput.find('>') != string::npos || singleCommandInput.find('<') != string::npos)
        {
            vector<string> args = tokenizeForRedirection(singleCommandInput);
            redirection(args);
        }
        else if (!singleCommandInput.empty())
        {
            executeCommand(singleCommandInput);
        }
    }
}