#include "headers.h"

void executeCommand(vector<string> &args, int &inputFileDescriptor, int pipeFileDescriptor[], bool flag, bool redirectionFlag)
{

    int pid = fork();

    if (pid == -1)
    {
        perror("fork");
        exit(1);
    }
    else if (pid == 0)
    {

        dup2(inputFileDescriptor, STDIN_FILENO);
        close(inputFileDescriptor);

        if (flag)
        {
            dup2(pipeFileDescriptor[1], STDOUT_FILENO);
        }
        close(pipeFileDescriptor[0]);

        if (redirectionFlag)
        {
            redirection(args);
        }
        else
        {
            char **argv = new char *[args.size() + 1];

            for (int i = 0; i < args.size(); ++i)
            {
                argv[i] = new char[args[i].size() + 1];
                strcpy(argv[i], args[i].c_str());
            }
            argv[args.size()] = nullptr;
            execvp(args[0].c_str(), argv);
            perror("Execution failed");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        wait(NULL); /* Collect childs */
        close(pipeFileDescriptor[1]);
        inputFileDescriptor = pipeFileDescriptor[0];
    }
}

void executePipes(vector<string> args)
{

    int n = args.size();

    int inputFileDescriptor = 0;
    for (int i = 0; i < n; i++)
    {
        if (args[i] == "|")
        {
            continue;
        }
        int pipeFileDescriptor[2];
        if (pipe(pipeFileDescriptor) == -1)
        {
            perror("Error in creating pipe!");
            return;
        }
        string command = args[i];
        vector<string> tokenizeCommand;
        bool redirection = false;
        if (command.find('>') != string::npos || command.find('<') != string::npos)
        {
            tokenizeCommand = tokenizeForRedirection(command);
            redirection = true;
        }
        else
        {
            tokenizeCommand = tokenize(args[i]);
        }
        bool flag = i < n - 1 ? true : false;
        executeCommand(tokenizeCommand, inputFileDescriptor, pipeFileDescriptor, flag, redirection);
    }
}