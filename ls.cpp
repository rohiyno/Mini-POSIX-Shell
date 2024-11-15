#include "headers.h"

int getDirectoryBlocks(bool showDetailsFlag, bool showHiddenFlag, string path)
{
    DIR *directory = opendir(path.c_str());
    struct stat fileStats;
    struct dirent *fileToGetBlocks;
    int size = 0;

    if (showDetailsFlag)
    {
        while ((fileToGetBlocks = readdir(directory)) != nullptr)
        {
            if (!showHiddenFlag && fileToGetBlocks->d_name[0] == '.')
            {
                continue;
            }
            string fullFilePath = path + "/" + fileToGetBlocks->d_name;
            if (stat(fullFilePath.c_str(), &fileStats) == 0)
            {
                size += fileStats.st_blocks;
            }
        }
    }
    return size;
}

void printFileDetails(string &path, struct dirent *files)
{
    struct stat fileStats;
    struct passwd *pw;
    struct group *gp;
    string fullFilePath = path + "/" + files->d_name;
    if (stat(fullFilePath.c_str(), &fileStats) == 0)
    {
        switch (fileStats.st_mode & S_IFMT)
        {
        case S_IFBLK:
            cout << "b";
            break;
        case S_IFCHR:
            cout << "c";
            break;
        case S_IFDIR:
            cout << "d";
            break;
        case S_IFIFO:
            cout << "p";
            break;
        case S_IFLNK:
            cout << "l";
            break;
        case S_IFSOCK:
            cout << "s";
            break;
        default:
            cout << "-";
            break;
        }
        cout << ((fileStats.st_mode & S_IRUSR) ? "r" : "-");
        cout << ((fileStats.st_mode & S_IWUSR) ? "w" : "-");
        cout << ((fileStats.st_mode & S_IXUSR) ? "x" : "-");
        cout << ((fileStats.st_mode & S_IRGRP) ? "r" : "-");
        cout << ((fileStats.st_mode & S_IWGRP) ? "w" : "-");
        cout << ((fileStats.st_mode & S_IXGRP) ? "x" : "-");
        cout << ((fileStats.st_mode & S_IROTH) ? "r" : "-");
        cout << ((fileStats.st_mode & S_IWOTH) ? "w" : "-");
        cout << ((fileStats.st_mode & S_IXOTH) ? "x " : "- ");
        cout << " " << fileStats.st_nlink;
        pw = getpwuid(fileStats.st_uid);
        cout << " " << pw->pw_name;
        gp = getgrgid(fileStats.st_gid);
        cout << "  " << gp->gr_name;
        string timeString = ctime(&fileStats.st_mtime);
        timeString.erase(timeString.find('\n'));
        cout << " " << fileStats.st_size << " " << timeString;
        cout << " " << files->d_name << endl;
    }
    else
    {
        cerr << "Error getting file details" << endl;
    }
}

void printDirectory(string path, bool showHiddenFlag, bool showDetailsFlag)
{

    DIR *directory = opendir(path.c_str());
    struct dirent *files;
    struct dirent *fileToGetBlocks;

    struct passwd *pw;
    struct group *gp;
    struct stat fileStats;

    if (!directory)
    {
        cerr << "Error opening directory" << endl;
        return;
    }

    int size = getDirectoryBlocks(showDetailsFlag, showHiddenFlag, path);
    if (showDetailsFlag)
    {
        cout << "total " << size << endl;
    }
    while ((files = readdir(directory)) != nullptr)
    {
        if (!showHiddenFlag && files->d_name[0] == '.')
        {
            continue;
        }
        if (showDetailsFlag)
        {
            printFileDetails(path, files);
        }
        else
        {
            string fileName = files->d_name;
            cout << fileName << endl;
        }
    }

    closedir(directory);
}

void printFile(string path, bool showDetailsFlag)
{
    if (showDetailsFlag)
    {
        struct stat fileStats;
        struct passwd *pw;
        struct group *gp;
        // string fullFilePath = path + "/" + files->d_name;
        if (stat(path.c_str(), &fileStats) == 0)
        {
            switch (fileStats.st_mode & S_IFMT)
            {
            case S_IFBLK:
                cout << "b";
                break;
            case S_IFCHR:
                cout << "c";
                break;
            case S_IFDIR:
                cout << "d";
                break;
            case S_IFIFO:
                cout << "p";
                break;
            case S_IFLNK:
                cout << "l";
                break;
            case S_IFSOCK:
                cout << "s";
                break;
            default:
                cout << "-";
                break;
            }
            cout << ((fileStats.st_mode & S_IRUSR) ? "r" : "-");
            cout << ((fileStats.st_mode & S_IWUSR) ? "w" : "-");
            cout << ((fileStats.st_mode & S_IXUSR) ? "x" : "-");
            cout << ((fileStats.st_mode & S_IRGRP) ? "r" : "-");
            cout << ((fileStats.st_mode & S_IWGRP) ? "w" : "-");
            cout << ((fileStats.st_mode & S_IXGRP) ? "x" : "-");
            cout << ((fileStats.st_mode & S_IROTH) ? "r" : "-");
            cout << ((fileStats.st_mode & S_IWOTH) ? "w" : "-");
            cout << ((fileStats.st_mode & S_IXOTH) ? "x " : "- ");
            cout << " " << fileStats.st_nlink;
            pw = getpwuid(fileStats.st_uid);
            cout << " " << pw->pw_name;
            gp = getgrgid(fileStats.st_gid);
            cout << " " << gp->gr_name;
            string timeString = ctime(&fileStats.st_mtime);
            timeString.erase(timeString.find('\n'));
            cout << " " << fileStats.st_size << " " << timeString;
            cout << " " << path << endl;
        }
        else
        {
            cerr << "Error getting file details" << endl;
        }
    }
    else
    {
        cout << path << endl;
    }
}

void printLsDetails(string path, bool showHiddenFlag, bool showDetailsFlag)
{
    struct stat pathStats;
    if (stat(path.c_str(), &pathStats) == 0)
    {
        if (pathStats.st_mode & S_IFDIR)
        {
            if (path != "." && path != "..")
            {
                cout << path << ":" << endl;
            }
            printDirectory(path, showHiddenFlag, showDetailsFlag);
            return;
        }
        else if (pathStats.st_mode & S_IFREG)
        {
            printFile(path, showDetailsFlag);
            return;
        }
    }
    else
    {
        cerr << "Error getting path stats" << endl;
        return;
    }
}

void listDirectory(vector<string> args)
{
    bool showHiddenFlag = false;
    bool showDetailsFlag = false;
    vector<string> directoryPaths;

    for (int i = 1; i < args.size(); ++i)
    {
        string argument = args[i];
        if (argument[0] == '-')
        {
            for (int i = 1; i < argument.length(); i++)
            {
                char flag = argument[i];
                if (flag == 'a')
                {
                    showHiddenFlag = true;
                }
                else if (flag == 'l')
                {
                    showDetailsFlag = true;
                }
            }
        }
        else
        {
            directoryPaths.push_back(args[i]);
        }
    }

    if (directoryPaths.empty())
    {
        printLsDetails(".", showHiddenFlag, showDetailsFlag);
    }
    else
    {
        for (int i = 0; i < directoryPaths.size(); i++)
        {
            string path = directoryPaths[i];
            if (path == "~")
            {
                path = SHELL_DIRECTORY_PATH;
            }
            printLsDetails(path, showHiddenFlag, showDetailsFlag);
            if (i < directoryPaths.size() - 1)
            {
                cout << endl;
            }
        }
    }
}
