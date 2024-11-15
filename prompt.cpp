#include "headers.h"

string getRelativePath(const std::string &fullPath)
{
    string homeDirectoryPath = SHELL_DIRECTORY_PATH;
    if (fullPath.find(homeDirectoryPath) == 0)
    {
        return fullPath.substr(homeDirectoryPath.length());
    }
    return fullPath;
}

void displayPrompt()
{
    char hostname[256];
    gethostname(hostname, sizeof(hostname));

    unsigned int uid = geteuid();
    struct passwd *pw = getpwuid(uid);
    const char *username = pw ? pw->pw_name : "user";
    char currentDirectory[PATH_MAX];
    getcwd(currentDirectory, sizeof(currentDirectory));

    string relativePath = getRelativePath(currentDirectory);

    std::cout << username << "@" << hostname << ":~" << ((!relativePath.empty()) ? relativePath : "") << "> ";
}
