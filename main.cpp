#include "headers.h"

const char *SHELL_DIRECTORY_PATH = "";

int main(int argc, char **argv)
{
    signal(SIGCHLD, SIG_IGN);
    signal(SIGINT, handleCtrlC);
    signal(SIGSTOP, handleCtrlZ);
    struct termios savedTermState;
    tcgetattr(STDIN_FILENO, &savedTermState);

    char buffer[256];
    if (getcwd(buffer, sizeof(buffer)) != nullptr)
    {
        SHELL_DIRECTORY_PATH = buffer;
    }
    else
    {
        std::cerr << "Failed to get the current directory" << std::endl;
        return 1;
    }

    loadCommandHistory();
    while (true)
    {
        displayPrompt();
        string input;

        if (!getline(cin, input, '\n'))
        {
            tcsetattr(STDIN_FILENO, TCSANOW, &savedTermState);
            cout << "Logged out!" << endl;
            break;
        }

        executeMultipleCommands(input);
        addToCommandHistory(input);
    }

    return 0;
}
