#include "headers.h"

string removeLeadingAndTrailingSpaces(string fileName)
{
    int firstNonSpace = fileName.find_first_not_of(" ");
    if (firstNonSpace != string::npos)
    {
        fileName = fileName.substr(firstNonSpace);
    }

    int lastNonSpace = fileName.find_last_not_of(" ");
    if (lastNonSpace != string::npos)
    {
        fileName = fileName.substr(0, lastNonSpace + 1);
    }
    return fileName;
}

void redirection(vector<string> args)
{

    int outputRedirection = -1;
    int inputRedirection = -1;
    for (int i = 0; i < args.size(); ++i)
    {

        if (args[i] == "<")
        {
            // Input redirection
            if (i + 1 < args.size())
            {
                string fileName = removeLeadingAndTrailingSpaces(args[i + 1]);
                inputRedirection = open(fileName.c_str(), O_RDONLY);
                if (inputRedirection == -1)
                {
                    perror("Error opening input file");
                    continue; // Skip this command
                }
                args.erase(args.begin() + i, args.begin() + i + 2);
                i -= 2; // Adjust the index
            }
            else
            {
                cerr << "Syntax error: Missing input file after '<'." << endl;
                continue; // Skip this command
            }
        }
        else if (args[i] == ">>" || args[i] == ">")
        {
            // Output redirection
            if (i + 1 < args.size())
            {
                string fileName = removeLeadingAndTrailingSpaces(args[i + 1]);
                int flags = O_WRONLY | O_CREAT;

                if (args[i] == ">>")
                {
                    flags |= O_APPEND;
                }
                else
                {
                    flags |= O_TRUNC;
                }

                outputRedirection = open(fileName.c_str(), flags, 0644);

                if (outputRedirection == -1)
                {
                    perror("Error opening output file");
                    continue; // Skip this command
                }

                args.erase(args.begin() + i, args.begin() + i + 2);
                i -= 2; // Adjust the index
            }
            else
            {
                cerr << "Syntax error: Missing output file after '"
                     << args[i] << "'." << endl;
                continue; // Skip this command
            }
        }
    }

    // Execute the command
    int pid = fork();
    if (pid == 0)
    { // Child process
        if (outputRedirection != -1)
        {
            dup2(outputRedirection, STDOUT_FILENO);
            close(outputRedirection);
        }

        if (inputRedirection != -1)
        {
            dup2(inputRedirection, STDIN_FILENO);
            close(inputRedirection);
        }

        vector<string> tokenizeCommand = tokenize(args[0]);
        char **argv = new char *[tokenizeCommand.size() + 1];

        for (int i = 0; i < tokenizeCommand.size(); ++i)
        {
            argv[i] = new char[tokenizeCommand[i].size() + 1];
            strcpy(argv[i], tokenizeCommand[i].c_str());
        }

        argv[tokenizeCommand.size()] = nullptr;
        execvp(tokenizeCommand[0].c_str(), argv);
        perror("Execution failed");
        exit(EXIT_FAILURE);
    }
    else if (pid < 0)
    {
        perror("Fork failed");
    }
    else
    {
        // Parent process
        waitpid(pid, nullptr, 0);
    }
    return;
}
