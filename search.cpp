#include "headers.h"

bool searchPath(string &currentDiectory, string &target)
{

    DIR *directory = opendir(currentDiectory.c_str());
    if (!directory)
    {
        cerr << "Error opening directory" << endl;
        return false;
    }

    bool found = false;
    struct dirent *files;
    while ((files = readdir(directory)) != nullptr)
    {
        struct stat fileStats;
        string fileName = files->d_name;
        if (fileName == "." || fileName == "..")
        {
            continue;
        }

        string path = currentDiectory + "/" + fileName;

        if (lstat(path.c_str(), &fileStats) == 0)
        {
            if (fileName == target)
            {
                found = true;
                break;
            }
            else if (S_ISDIR(fileStats.st_mode))
            {
                found = searchPath(path, target);
                if (found)
                {
                    break;
                }
            }
        }
    }

    closedir(directory);
    return found;
}

void search(vector<string> args)
{
    if (args.size() != 2)
    {
        std::cerr << "Invalid arguments for search" << std::endl;
    }
    else
    {
        string currentDirectory = ".";
        bool found = searchPath(currentDirectory, args[1]);
        cout << (found ? "True" : "False") << endl;
    }
}