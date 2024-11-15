#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include <pwd.h>
#include <dirent.h>
#include <iomanip>
#include <fstream>
#include <sstream>
using namespace std;

vector<pid_t> background_processes;

// void display_prompt()
// {
//     char hostname[256];
//     gethostname(hostname, sizeof(hostname));

//     uid_t uid = geteuid();
//     struct passwd *pw = getpwuid(uid);
//     const char *username = pw ? pw->pw_name : "user";

//     char cwd[PATH_MAX];
//     getcwd(cwd, sizeof(cwd));
//     char *base_dir = basename(cwd);

//     std::cout << username << "@" << hostname << ":~";
//     if (std::string(base_dir) != std::string(username))
//         std::cout << "/" << base_dir;
//     std::cout << "> ";
// }

void change_directory(const std::string &path)
{
    if (chdir(path.c_str()) != 0)
    {
        std::cerr << "Error changing directory" << std::endl;
    }
}

void print_working_directory()
{
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != nullptr)
    {
        std::cout << cwd << std::endl;
    }
    else
    {
        std::cerr << "Error getting working directory" << std::endl;
    }
}

void echo(const std::vector<std::string> &args)
{
    for (const auto &arg : args)
    {
        std::cout << arg << ' ';
    }
    std::cout << std::endl;
}

void list_directory(const std::string &path, bool show_hidden, bool show_details)
{
    DIR *dir = opendir(path.c_str());
    if (!dir)
    {
        std::cerr << "Error opening directory" << std::endl;
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        if (!show_hidden && entry->d_name[0] == '.')
        {
            continue;
        }
        if (show_details)
        {
            struct stat file_stat;
            std::string full_path = path + "/" + entry->d_name;
            if (stat(full_path.c_str(), &file_stat) == 0)
            {
                std::cout << (S_ISDIR(file_stat.st_mode) ? "d" : "-")
                          << (file_stat.st_mode & S_IRUSR ? "r" : "-")
                          << (file_stat.st_mode & S_IWUSR ? "w" : "-")
                          << (file_stat.st_mode & S_IXUSR ? "x" : "-")
                          << " " << file_stat.st_size << " " << entry->d_name << std::endl;
            }
            else
            {
                std::cerr << "Error getting file details" << std::endl;
            }
        }
        else
        {
            std::cout << entry->d_name << std::endl;
        }
    }

    closedir(dir);
}

void execute_command(const std::vector<std::string> &args, bool run_in_background,
                     bool redirect_input, const std::string &input_file,
                     bool redirect_output, const std::string &output_file)
{
    pid_t pid = fork();

    if (pid == 0)
    {
        // Child process
        if (redirect_input)
        {
            int input_fd = open(input_file.c_str(), O_RDONLY);
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }

        if (redirect_output)
        {
            int output_fd = open(output_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }

        std::vector<char *> c_args;
        for (const auto &arg : args)
        {
            c_args.push_back(const_cast<char *>(arg.c_str()));
        }
        c_args.push_back(nullptr);

        execvp(c_args[0], c_args.data());

        std::cerr << "Error executing command" << std::endl;
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        // Parent process
        if (!run_in_background)
        {
            waitpid(pid, nullptr, 0);
        }
    }
    else
    {
        std::cerr << "Fork error" << std::endl;
    }
}

void print_process_info(pid_t pid)
{
    std::cout << "pid -- " << pid << std::endl;

    std::string command = "ps -o stat=,rss=,comm= -p " + std::to_string(pid);
    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe)
    {
        std::cerr << "Error executing ps command" << std::endl;
        return;
    }

    char line[256];
    if (fgets(line, sizeof(line), pipe))
    {
        char status, comm[256];
        int rss;
        sscanf(line, " %c %d %s", &status, &rss, comm);

        std::string status_str;
        switch (status)
        {
        case 'R':
            status_str = "Running";
            break;
        case 'S':
            status_str = "Sleeping";
            break;
        case 'D':
            status_str = "Waiting";
            break;
        case 'Z':
            status_str = "Zombie";
            break;
        case 'T':
            status_str = "Stopped";
            break;
        default:
            status_str = "Unknown";
        }

        std::cout << "Process Status -- {" << status_str;
        if (status == 'R' || status == 'S' || status == 'D')
        {
            if (std::find(background_processes.begin(), background_processes.end(), pid) !=
                background_processes.end())
            {
                std::cout << "+";
            }
        }
        std::cout << "}" << std::endl;

        std::cout << "memory -- " << rss << " {Virtual Memory}" << std::endl;
        std::cout << "Executable Path -- /proc/" << pid << "/exe" << std::endl;
    }

    pclose(pipe);
}

bool search_file(const std::string &current_dir, const std::string &target)
{
    DIR *dir = opendir(current_dir.c_str());
    if (!dir)
    {
        return false;
    }

    bool found = false;
    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        std::string name = entry->d_name;
        if (name == "." || name == "..")
        {
            continue;
        }

        std::string path = current_dir + "/" + name;
        struct stat st;
        if (lstat(path.c_str(), &st) == 0)
        {
            if (S_ISDIR(st.st_mode))
            {
                found = search_file(path, target);
                if (found)
                {
                    break;
                }
            }
            else if (name == target)
            {
                found = true;
                break;
            }
        }
    }

    closedir(dir);
    return found;
}

void execute_pipeline(const std::vector<std::vector<std::string>> &commands)
{
    int num_pipes = commands.size() - 1;
    int pipes[num_pipes][2];

    // Create pipes
    for (int i = 0; i < num_pipes; ++i)
    {
        if (pipe(pipes[i]) == -1)
        {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    // Fork processes
    for (int i = 0; i < commands.size(); ++i)
    {
        bool run_in_background = false;

        if (i == commands.size() - 1 && commands[i].back() == "&")
        {
            run_in_background = true;
            commands[i].pop_back();
        }

        pid_t pid = fork();
        if (pid == 0)
        {
            // Child process

            // Set up pipes
            if (i < num_pipes)
            {
                if (dup2(pipes[i][1], STDOUT_FILENO) == -1)
                {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            // Close unnecessary pipe ends
            for (int j = 0; j < num_pipes; ++j)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // Redirect input if needed
            if (i > 0)
            {
                if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1)
                {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            // Execute command
            std::vector<std::string> args = parse_redirection(commands[i]);
            execvp(args[0].c_str(), const_cast<char *const *>(args.data()));
            std::cerr << "Command not found" << std::endl;
            exit(EXIT_FAILURE);
        }
        else if (pid < 0)
        {
            perror("fork");
        }

        // Close write end of the pipe for the current process
        if (i < num_pipes)
        {
            close(pipes[i][1]);
        }

        if (!run_in_background)
        {
            waitpid(pid, nullptr, 0);
        }
        else
        {
            background_processes.push_back(pid);
        }
    }

    // Close all pipe ends
    for (int i = 0; i < num_pipes; ++i)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
}
int main()
{
    while (true)
    {
        display_prompt();
        std::string input;
        std::getline(std::cin, input);

        if (input == "exit")
        {
            break;
        }

        bool run_in_background = false;
        bool redirect_input = false;
        bool redirect_output = false;
        std::string input_file, output_file;

        if (input.back() == '&')
        {
            run_in_background = true;
            input.pop_back();
        }

        vector<string> args;
        char *token = strtok(const_cast<char *>(input.c_str()), " ");
        while (token != nullptr)
        {
            if (strcmp(token, "<") == 0)
            {
                redirect_input = true;
                token = strtok(nullptr, " ");
                input_file = token;
            }
            else if (strcmp(token, ">") == 0)
            {
                redirect_output = true;
                token = strtok(nullptr, " ");
                output_file = token;
            }
            else
            {
                args.push_back(token);
            }
            token = strtok(nullptr, " ");
        }

        if (!args.empty())
        {
            if (args[0] == "cd")
            {
                if (args.size() > 2)
                {
                    std::cerr << "Invalid arguments for error handling" << std::endl;
                }
                else if (args.size() == 1)
                {
                    // change_directory(getenv("HOME"));
                }
                else
                {
                    if (args[1] == "~")
                    {
                        // change_directory(getenv("HOME"));
                    }
                    else if (args[1] == "-")
                    {
                        // Implement logic for previous directory if needed
                    }
                    else
                    {
                        change_directory(args[1]);
                    }
                }
            }
            else if (args[0] == "search")
            {
                if (args.size() != 2)
                {
                    std::cerr << "Invalid arguments for search" << std::endl;
                }
                else
                {
                    bool found = search_file(".", args[1]);
                    std::cout << (found ? "True" : "False") << std::endl;
                }
            }
            else if (args[0] == "pwd")
            {
                print_working_directory();
            }
            else if (args[0] == "echo")
            {
                args.erase(args.begin());
                echo(args);
            }
            else if (args[0] == "pinfo")
            {
                if (args.size() == 1)
                {
                    print_process_info(getpid());
                }
                else if (args.size() == 2)
                {
                    int target_pid = std::stoi(args[1]);
                    print_process_info(target_pid);
                }
            }
            else if (args[0] == "ls")
            {
                bool show_hidden = false;
                bool show_details = false;
                std::vector<std::string> dirs;

                for (size_t i = 1; i < args.size(); ++i)
                {
                    if (args[i][0] == '-')
                    {
                        for (char flag : args[i].substr(1))
                        {
                            if (flag == 'a')
                            {
                                show_hidden = true;
                            }
                            else if (flag == 'l')
                            {
                                show_details = true;
                            }
                        }
                    }
                    else
                    {
                        dirs.push_back(args[i]);
                    }
                }

                if (dirs.empty())
                {
                    list_directory(".", show_hidden, show_details);
                }
                else
                {
                    for (const auto &dir : dirs)
                    {
                        list_directory(dir, show_hidden, show_details);
                    }
                }
            }
            else
            {
                execute_command(args, run_in_background, redirect_input, input_file,
                                redirect_output, output_file);
            }
        }
    }

    return 0;
}
