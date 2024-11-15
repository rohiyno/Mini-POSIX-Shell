#include "headers.h"

string currentDirectory = "";
string previousDirectory = "";

int changeDirectory(vector<string> args)
{
    if (currentDirectory == "")
    {
        currentDirectory = SHELL_DIRECTORY_PATH;
    }
    if (args.size() > 1)
    {
        cerr << "Invalid arguments for error handling" << endl;
        return 1;
    }
    else if (args.empty() || args[0] == "~")
    {
        if (chdir(SHELL_DIRECTORY_PATH) != 0)
        {
            std::cout << "Invalid directory path" << std::endl;
            return 1;
        }
        previousDirectory = currentDirectory;
        currentDirectory = SHELL_DIRECTORY_PATH;
    }
    else if (args[0] == "..")
    {
        previousDirectory = currentDirectory;
        unsigned long lastSlash = currentDirectory.find_last_of("/");

        if (lastSlash != std::string::npos)
        {
            currentDirectory = currentDirectory.substr(0, lastSlash);
            if (chdir(currentDirectory.c_str()) != 0)
            {
                std::cout << "Invalid path!!" << std::endl;
                return 1;
            }
        }
    }
    else if (args[0] == "-")
    {
        if (!previousDirectory.empty())
        {
            swap(currentDirectory, previousDirectory);
            chdir(currentDirectory.c_str());
        }
        cout << currentDirectory << endl;
        return 0;
    }
    else
    {
        previousDirectory = currentDirectory;
        if (chdir(args[0].c_str()) != 0)
        {
            std::cout << "Invalid directory path" << std::endl;
            return 1;
        }
        char buffer[1024];
        currentDirectory = getcwd(buffer, sizeof(buffer));
    }
    return 0;
}
