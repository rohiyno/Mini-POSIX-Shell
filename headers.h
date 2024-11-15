#ifndef HEADERS_H
#define HEADERS_H
#include <iostream>
#include <unistd.h>
#include <bits/stdc++.h>
#include <sys/types.h>
#include <ncurses.h>
#include <libgen.h>
#include <dirent.h>
#include <termios.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
using namespace std;
extern const char *SHELL_DIRECTORY_PATH;

void displayPrompt();
void listDirectory(vector<string> args);
void pwd();
int changeDirectory(vector<string> args);
void echo(vector<string> args);
void executeBackgroundProcess(vector<string> args, bool runInBackground, int &foregroundJobPid, bool &foregroundJobRunning);
void printProcessInfo(vector<string> args);
void search(vector<string> args);
vector<string> tokenize(string input);
vector<string> tokenizeForRedirection(string input);
vector<string> tokenizeForPipe(string input);
void redirection(vector<string> args);
void addToCommandHistory(const string &command);
void loadCommandHistory();
void displayCommandHistory(int num);
void executePipes(vector<string> args);
vector<string> tokenizeForColon(string input);
void executeMultipleCommands(const string &input);
void handleCtrlZ(int signum);
void handleCtrlC(int signum);
#endif